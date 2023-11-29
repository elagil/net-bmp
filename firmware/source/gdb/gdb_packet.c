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

    p_packet->written_char_count = 0u;
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
static enum gdb_packet_result gdb_packet_add_character(struct gdb_packet* p_packet, const char character,
                                                       bool b_update_checksum) {
    if (p_packet->written_char_count >= GDB_PACKET_MAX_BUFFER_SIZE) {
        return GDB_PACKET_RESULT_OVERFLOW;
    }

    p_packet->buffer[p_packet->written_char_count] = character;
    p_packet->written_char_count++;

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
void gdb_packet_parse_character(struct gdb_packet* p_packet, char character) {
    switch (p_packet->state) {
        case GDB_PACKET_STATE_COLLECT_CHECKSUM_UPPER:
            gdb_packet_add_character(p_packet, character, false);
            p_packet->reference_checksum = uint_from_hex_nibble(character) << 4u;

            p_packet->state = GDB_PACKET_STATE_COLLECT_CHECKSUM_LOWER;
            break;

        case GDB_PACKET_STATE_COLLECT_CHECKSUM_LOWER:
            gdb_packet_add_character(p_packet, character, false);
            p_packet->reference_checksum |= uint_from_hex_nibble(character);

            p_packet->state = GDB_PACKET_STATE_COMPLETE;
            break;

        default:
            gdb_packet_add_character(p_packet, character, true);
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
static enum gdb_packet_result gdb_packet_read_single(struct gdb_packet* p_packet, char character) {
    switch (character) {
        case GDB_PACKET_CHAR_START:
            gdb_packet_add_character(p_packet, character, false);

            p_packet->state = GDB_PACKET_STATE_COLLECT;
            break;

        case GDB_PACKET_CHAR_STOP:
            gdb_packet_add_character(p_packet, character, false);

            p_packet->state = GDB_PACKET_STATE_COLLECT_CHECKSUM_UPPER;
            break;

        case GDB_PACKET_CHAR_CTRL_C:
            p_packet->state = GDB_PACKET_STATE_ABORT;
            break;

        case GDB_PACKET_CHAR_ACK:
        case GDB_PACKET_CHAR_NACK:
            if (p_packet->state == GDB_PACKET_STATE_PENDING_ACK) {
                // TODO: Handle ACK.
            } else {
                gdb_packet_parse_character(p_packet, character);
            }
            break;

        default:
            gdb_packet_parse_character(p_packet, character);
            break;
    }

    if (p_packet->state != GDB_PACKET_STATE_COMPLETE) {
        return GDB_PACKET_RESULT_COLLECTING;
    }

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
 * @param size The size of the character input.
 * @param p_consumed_size A pointer to the size of input that was consumed.
 * @return enum gdb_packet_result The packet result.
 */
enum gdb_packet_result gdb_packet_read_from_stream(struct gdb_packet* p_packet, const char* p_characters,
                                                   const size_t size, size_t* p_consumed_size) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_PTR_NOT_NULL(p_characters);
    ASSERT_PTR_NOT_NULL(p_consumed_size);

    gdb_packet_init(p_packet, GDB_PACKET_TYPE_INBOUND);
    enum gdb_packet_result result = GDB_PACKET_RESULT_COLLECTING;

    *p_consumed_size = 0u;
    for (size_t character_index = 0u; character_index < size; character_index++) {
        result = gdb_packet_read_single(p_packet, p_characters[character_index]);
        (*p_consumed_size)++;

        if ((result == GDB_PACKET_RESULT_OK) || (result == GDB_PACKET_RESULT_CHECKSUM_ERROR)) {
            // A full packet was consumed - with or without success.
            break;
        }
    }

    return result;
}

/**
 * @brief Write characters to a GDB packet.
 * @details Freshly initializes the provided packet. This function is meant for generating GDB packets for transmission
 * towards the GDB stub.
 *
 * @param p_packet A pointer to the packet.
 * @param p_characters A pointer to the characters to write. Must be NUL-terminated.
 * @return enum gdb_packet_result The packet result.
 */
enum gdb_packet_result gdb_packet_write(struct gdb_packet* p_packet, const char* p_characters) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_PTR_NOT_NULL(p_characters);

    gdb_packet_init(p_packet, GDB_PACKET_TYPE_OUTBOUND);
    p_packet->state = GDB_PACKET_STATE_COLLECT;

    RETURN_IF_NOT(gdb_packet_add_character(p_packet, GDB_PACKET_CHAR_START, false), GDB_PACKET_RESULT_OK);

    for (size_t character_index = 0u; character_index < strlen(p_characters); character_index++) {
        RETURN_IF_NOT(gdb_packet_add_character(p_packet, p_characters[character_index], true), GDB_PACKET_RESULT_OK);
    }

    RETURN_IF_NOT(gdb_packet_add_character(p_packet, GDB_PACKET_CHAR_STOP, false), GDB_PACKET_RESULT_OK);
    RETURN_IF_NOT(gdb_packet_add_character(p_packet, hex_nibble_from_uint(GET_UPPER_NIBBLE(p_packet->checksum)), false),
                  GDB_PACKET_RESULT_OK);
    RETURN_IF_NOT(gdb_packet_add_character(p_packet, hex_nibble_from_uint(GET_LOWER_NIBBLE(p_packet->checksum)), false),
                  GDB_PACKET_RESULT_OK);

    p_packet->state = GDB_PACKET_STATE_COMPLETE;
    return GDB_PACKET_RESULT_OK;
}

/**
 * @brief Write a single character to a GDB packet. Used for ACK and NACK.
 * @details Freshly initializes the provided packet. This function is meant for generating GDB packets for transmission
 * towards the GDB stub.
 *
 * @param p_packet A pointer to the packet.
 * @param character The character to write.
 * @return enum gdb_packet_result The packet result.
 */
enum gdb_packet_result gdb_packet_write_single(struct gdb_packet* p_packet, const char character) {
    ASSERT_PTR_NOT_NULL(p_packet);

    gdb_packet_init(p_packet, GDB_PACKET_TYPE_OUTBOUND);
    RETURN_IF_NOT(gdb_packet_add_character(p_packet, character, false), GDB_PACKET_RESULT_OK);

    p_packet->state = GDB_PACKET_STATE_COMPLETE;
    return GDB_PACKET_RESULT_OK;
}
