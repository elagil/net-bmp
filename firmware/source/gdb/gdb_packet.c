// Copyright 2023 elagil

/**
 * @file
 * @brief   The GDB packet module.
 * @details Allows reading and writing of GDB inbound and outbound packets.
 *
 * @addtogroup gdb
 * @{
 */

#include "gdb_packet.h"

#include <string.h>

#include "common/common.h"
#include "common/hex.h"

/**
 * @brief Initialize a packet.
 *
 * @param p_packet A pointer to the packet.
 * @param type The type of packet (inbound or outbound).
 */
void gdb_packet_init(struct gdb_packet* p_packet, enum gdb_packet_type type) {
    ASSERT_PTR_NOT_NULL(p_packet);

    p_packet->length             = 0u;
    p_packet->checksum           = 0u;
    p_packet->reference_checksum = 0u;

    p_packet->type  = type;
    p_packet->state = GDB_PACKET_STATE_INIT;
}

/**
 * @brief Add a character to the GDB packet buffer.
 *
 * @param p_packet A pointer to the packet.
 * @param character The character to add.
 * @param b_update_checksum If true, updates the packet checksum. The checksum should only be updated for payload
 * characters, not for overhead (e.g. START/STOP, or the checksum characters themselves).
 * @return enum gdb_packet_result The packet result.
 */
static enum gdb_packet_result gdb_packet_add_char(struct gdb_packet* p_packet, const char character,
                                                  bool b_update_checksum) {
    if (p_packet->length >= GDB_PACKET_MAX_BUFFER_length) {
        return GDB_PACKET_RESULT_OVERFLOW;
    }

    p_packet->buffer[p_packet->length] = character;
    p_packet->length++;

    if (b_update_checksum) {
        p_packet->checksum += character;
    }

    return GDB_PACKET_RESULT_OK;
}

/**
 * @brief Parse a character from a GDB packet.
 *
 * @param p_packet A pointer to the packet.
 * @param character The character to parse.
 */
void gdb_packet_parse_char(struct gdb_packet* p_packet, char character) {
    switch (p_packet->state) {
        case GDB_PACKET_STATE_COLLECT_CHECKSUM_UPPER:
            gdb_packet_add_char(p_packet, character, false);
            p_packet->reference_checksum = char_from_hex_nibble(character) << 4u;

            p_packet->state = GDB_PACKET_STATE_COLLECT_CHECKSUM_LOWER;
            break;

        case GDB_PACKET_STATE_COLLECT_CHECKSUM_LOWER:
            gdb_packet_add_char(p_packet, character, false);
            p_packet->reference_checksum |= char_from_hex_nibble(character);

            p_packet->state = GDB_PACKET_STATE_COMPLETE;
            break;

        default:
            gdb_packet_add_char(p_packet, character, true);
            break;
    }
}

/**
 * @brief Read a single character from a GDB packet.
 *
 * @param p_packet A pointer to the packet.
 * @param character The character to read.
 * @return enum gdb_packet_result The packet result.
 */
static enum gdb_packet_result gdb_packet_read_char(struct gdb_packet* p_packet, char character) {
    switch (character) {
        case GDB_PACKET_CHAR_START:
            gdb_packet_add_char(p_packet, character, false);

            p_packet->state = GDB_PACKET_STATE_COLLECT;
            break;

        case GDB_PACKET_CHAR_STOP:
            gdb_packet_add_char(p_packet, character, false);

            p_packet->state = GDB_PACKET_STATE_COLLECT_CHECKSUM_UPPER;
            break;

        case GDB_PACKET_CHAR_CTRL_C:
            p_packet->state = GDB_PACKET_STATE_ABORT;
            break;

        case GDB_PACKET_CHAR_ACK:
        case GDB_PACKET_CHAR_NACK:
            if (p_packet->state == GDB_PACKET_STATE_PENDING_ACK) {
                // TODO(elagil): Handle ACK.
            } else {
                gdb_packet_parse_char(p_packet, character);
            }
            break;

        default:
            gdb_packet_parse_char(p_packet, character);
            break;
    }

    if (p_packet->state != GDB_PACKET_STATE_COMPLETE) {
        return GDB_PACKET_RESULT_COLLECTING;
    }

    // NUL-terminate the packet string.
    RETURN_IF_NOT(gdb_packet_add_char(p_packet, '\0', false), GDB_PACKET_RESULT_OK);

    if (p_packet->reference_checksum != p_packet->checksum) {
        return GDB_PACKET_RESULT_CHECKSUM_ERROR;
    }

    return GDB_PACKET_RESULT_OK;
}

/**
 * @brief Read characters from a stream into a GDB packet.
 * @details Detects GDB packet boundaries.
 *
 * @param p_packet A pointer to the packet.
 * @param p_characters A pointer to the characters to consume.
 * @param length The length of the character input.
 * @param p_consumed_length A pointer to the length of input that was consumed.
 * @return enum gdb_packet_result The packet result.
 */
enum gdb_packet_result gdb_packet_read_stream(struct gdb_packet* p_packet, const char* p_characters,
                                              const size_t length, size_t* p_consumed_length) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_PTR_NOT_NULL(p_characters);
    ASSERT_PTR_NOT_NULL(p_consumed_length);

    gdb_packet_init(p_packet, GDB_PACKET_TYPE_INBOUND);
    enum gdb_packet_result result = GDB_PACKET_RESULT_COLLECTING;

    *p_consumed_length = 0u;
    for (size_t character_index = 0u; character_index < length; character_index++) {
        result = gdb_packet_read_char(p_packet, p_characters[character_index]);
        (*p_consumed_length)++;

        if ((result == GDB_PACKET_RESULT_OK) || (result == GDB_PACKET_RESULT_CHECKSUM_ERROR)) {
            // A full packet was consumed - with or without success.
            break;
        }
    }

    return result;
}

/**
 * @brief Write characters to a GDB packet - initialize the packet and write the start character.
 *
 * @param p_packet A pointer to the packet.
 * @return enum gdb_packet_result The packet result.
 */
enum gdb_packet_result gdb_packet_write_start(struct gdb_packet* p_packet) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_VERBOSE(p_packet->state != GDB_PACKET_STATE_COMPLETE, "Complete packet was not sent yet.");

    gdb_packet_init(p_packet, GDB_PACKET_TYPE_OUTBOUND);
    p_packet->state = GDB_PACKET_STATE_COLLECT;

    RETURN_IF_NOT(gdb_packet_add_char(p_packet, GDB_PACKET_CHAR_START, false), GDB_PACKET_RESULT_OK);
    return GDB_PACKET_RESULT_OK;
}

/**
 * @brief Write characters to a GDB packet - the payload itself. Can be called repeatedly.
 *
 * @param p_packet A pointer to the packet.
 * @param p_characters A pointer to the NUL-terminated string to write to the packet.
 * @return enum gdb_packet_result The packet result.
 */
enum gdb_packet_result gdb_packet_write_payload(struct gdb_packet* p_packet, const char* p_characters) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_PTR_NOT_NULL(p_characters);
    ASSERT_VERBOSE(p_packet->state == GDB_PACKET_STATE_COLLECT, "Invalid packet state.");

    for (size_t character_index = 0u; character_index < strlen(p_characters); character_index++) {
        RETURN_IF_NOT(gdb_packet_add_char(p_packet, p_characters[character_index], true), GDB_PACKET_RESULT_OK);
    }
    return GDB_PACKET_RESULT_OK;
}

/**
 * @brief Write characters to a GDB packet - the payload itself - as hex values. Can be called repeatedly.
 *
 * @param p_packet A pointer to the packet.
 * @param p_characters A pointer to the NUL-terminated string to write to the packet as hex values.
 * @return enum gdb_packet_result The packet result.
 */
enum gdb_packet_result gdb_packet_write_payload_as_hex(struct gdb_packet* p_packet, const char* p_characters) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_PTR_NOT_NULL(p_characters);
    ASSERT_VERBOSE(p_packet->state == GDB_PACKET_STATE_COLLECT, "Invalid packet state.");

    for (size_t character_index = 0u; character_index < strlen(p_characters); character_index++) {
        char character = p_characters[character_index];

        char upper_nibble = hex_nibble_from_char(GET_UPPER_NIBBLE(character));
        char lower_nibble = hex_nibble_from_char(GET_LOWER_NIBBLE(character));

        RETURN_IF_NOT(gdb_packet_add_char(p_packet, upper_nibble, true), GDB_PACKET_RESULT_OK);
        RETURN_IF_NOT(gdb_packet_add_char(p_packet, lower_nibble, true), GDB_PACKET_RESULT_OK);
    }

    return GDB_PACKET_RESULT_OK;
}

/**
 * @brief Write characters to a GDB packet - finalize the packet.
 *
 * @param p_packet A pointer to the packet.
 * @return enum gdb_packet_result The packet result.
 */
enum gdb_packet_result gdb_packet_write_stop(struct gdb_packet* p_packet) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_VERBOSE(p_packet->state == GDB_PACKET_STATE_COLLECT, "Invalid packet state.");

    RETURN_IF_NOT(gdb_packet_add_char(p_packet, GDB_PACKET_CHAR_STOP, false), GDB_PACKET_RESULT_OK);
    RETURN_IF_NOT(gdb_packet_add_char(p_packet, hex_nibble_from_char(GET_UPPER_NIBBLE(p_packet->checksum)), false),
                  GDB_PACKET_RESULT_OK);
    RETURN_IF_NOT(gdb_packet_add_char(p_packet, hex_nibble_from_char(GET_LOWER_NIBBLE(p_packet->checksum)), false),
                  GDB_PACKET_RESULT_OK);
    RETURN_IF_NOT(gdb_packet_add_char(p_packet, '\0', false), GDB_PACKET_RESULT_OK);

    p_packet->state = GDB_PACKET_STATE_COMPLETE;
    return GDB_PACKET_RESULT_OK;
}

/**
 * @brief Write characters to a GDB packet.
 * @details Freshly initializes the provided packet. This function is meant for generating GDB packets for
 * transmission towards the GDB stub.
 *
 * @param p_packet A pointer to the packet.
 * @param p_characters A pointer to the characters to write. Must be NUL-terminated.
 * @return enum gdb_packet_result The packet result.
 */
enum gdb_packet_result gdb_packet_write(struct gdb_packet* p_packet, const char* p_characters) {
    RETURN_IF_NOT(gdb_packet_write_start(p_packet), GDB_PACKET_RESULT_OK);
    RETURN_IF_NOT(gdb_packet_write_payload(p_packet, p_characters), GDB_PACKET_RESULT_OK);
    RETURN_IF_NOT(gdb_packet_write_stop(p_packet), GDB_PACKET_RESULT_OK);

    return GDB_PACKET_RESULT_OK;
}

/**
 * @brief Write a single ACK or NACK character to a GDB packet.
 * @details Freshly initializes the provided packet. This function is meant for generating GDB packets for
 * transmission towards the GDB stub.
 *
 * @param p_packet A pointer to the packet.
 * @param character The character to write.
 * @return enum gdb_packet_result The packet result.
 */
enum gdb_packet_result gdb_packet_write_ack(struct gdb_packet* p_packet, const char character) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_VERBOSE((character == GDB_PACKET_CHAR_ACK) || (character == GDB_PACKET_CHAR_NACK), "Unsupported character.");

    gdb_packet_init(p_packet, GDB_PACKET_TYPE_OUTBOUND);
    RETURN_IF_NOT(gdb_packet_add_char(p_packet, character, false), GDB_PACKET_RESULT_OK);

    p_packet->state = GDB_PACKET_STATE_COMPLETE;
    return GDB_PACKET_RESULT_OK;
}

/**
 * @}
 */
