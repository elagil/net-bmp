// Copyright 2023 elagil

/**
 * @file
 * @brief   The GDB packet module headers.
 *
 * @addtogroup gdb
 * @{
 */

#ifndef SOURCE_GDB_GDB_PACKET_H_
#define SOURCE_GDB_GDB_PACKET_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include "common/common.h"

#define GDB_PACKET_PREIX_LENGTH      1u  // The prefix is simply the '$' symbol.
#define GDB_PACKET_OVERHEAD_length   4u  // Start, stop, and two checksum bytes.
#define GDB_PACKET_MAX_BUFFER_length 2048u

enum gdb_packet_char {
    GDB_PACKET_CHAR_START              = '$',
    GDB_PACKET_CHAR_STOP               = '#',
    GDB_PACKET_CHAR_ACK                = '+',
    GDB_PACKET_CHAR_NACK               = '-',
    GDB_PACKET_CHAR_ESC                = '}',
    GDB_PACKET_CHAR_RUN_LENGTH_START   = '*',
    GDB_PACKET_CHAR_NOTIFICATION_START = '%',
    GDB_PACKET_CHAR_CTRL_C             = '\x03',
    GDB_PACKET_CHAR_EOT                = '\x04',
};

enum gdb_packet_result {
    GDB_PACKET_RESULT_OK,
    GDB_PACKET_RESULT_COLLECTING,
    GDB_PACKET_RESULT_OVERFLOW,
    GDB_PACKET_RESULT_CHECKSUM_ERROR,
};

enum gdb_packet_state {
    GDB_PACKET_STATE_INIT,
    GDB_PACKET_STATE_COLLECT,
    GDB_PACKET_STATE_COLLECT_CHECKSUM_UPPER,
    GDB_PACKET_STATE_COLLECT_CHECKSUM_LOWER,
    GDB_PACKET_STATE_COMPLETE,
    GDB_PACKET_STATE_SENT,
    GDB_PACKET_STATE_ABORT,
    GDB_PACKET_STATE_PENDING_ACK,
};

enum gdb_packet_type {
    GDB_PACKET_TYPE_INBOUND,
    GDB_PACKET_TYPE_OUTBOUND,
};

/**
 * @brief A GDB packet, received from the stub, or as prepared for transmission.
 */
struct gdb_packet {
    char   buffer[GDB_PACKET_MAX_BUFFER_length];
    size_t length;

    uint8_t checksum;
    uint8_t reference_checksum;

    enum gdb_packet_type  type;
    enum gdb_packet_state state;
};

void                   gdb_packet_init(struct gdb_packet* p_packet, enum gdb_packet_type type);
enum gdb_packet_result gdb_packet_read_stream(struct gdb_packet* p_packet, const char* p_input,
                                              const size_t input_length, size_t* p_consumed_length);

enum gdb_packet_result gdb_packet_write_start(struct gdb_packet* p_packet);
enum gdb_packet_result gdb_packet_write_payload(struct gdb_packet* p_packet, const char* p_characters);
enum gdb_packet_result gdb_packet_write_payload_as_hex(struct gdb_packet* p_packet, const char* p_characters);
enum gdb_packet_result gdb_packet_write_stop(struct gdb_packet* p_packet);

enum gdb_packet_result gdb_packet_write(struct gdb_packet* p_packet, const char* p_characters);
enum gdb_packet_result gdb_packet_write_ack(struct gdb_packet* p_packet, const char character);

/**
 * @brief Mark a packet as sent.
 *
 * @param p_packet A pointer to the packet.
 */
static inline void gdb_packet_mark_sent(struct gdb_packet* p_packet) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_VERBOSE(p_packet->state = GDB_PACKET_STATE_COMPLETE, "Incomplete packet cannot have been sent.");

    p_packet->state = GDB_PACKET_STATE_SENT;
}

/**
 * @brief Get the full length of a packet.
 *
 * @param p_packet A pointer to the packet.
 * @return size_t The length of the packet.
 */
static inline size_t gdb_packet_get_length(struct gdb_packet* p_packet) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_VERBOSE((p_packet->state == GDB_PACKET_STATE_COMPLETE) || (p_packet->state == GDB_PACKET_STATE_SENT),
                   "Packet not complete.");

    return p_packet->length;
}

/**
 * @brief Get the length of a packet, excluding overhead (start, stop, checksum characters).
 *
 * @param p_packet A pointer to the packet.
 * @return size_t The length of the packet.
 */
static inline size_t gdb_packet_get_payload_length(struct gdb_packet* p_packet) {
    size_t packet_length = gdb_packet_get_length(p_packet);

    ASSERT_VERBOSE(packet_length >= GDB_PACKET_OVERHEAD_length, "Packet length too small.");
    return packet_length - GDB_PACKET_OVERHEAD_length;
}

/**
 * @brief Get a packet's buffer pointer at a specified offset.
 *
 * @param p_packet A pointer to the packet.
 * @param offset The offset, from which to get the character.
 * @return char* The offset buffer pointer.
 */
static inline char* gdb_packet_get_buffer_offset(struct gdb_packet* p_packet, size_t offset) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_VERBOSE(offset < gdb_packet_get_length(p_packet), "Offset exceeds buffer length.");

    return &p_packet->buffer[offset];
}

/**
 * @brief Get a packet's buffer pointer at a specified offset from the start of the payload.
 *
 * @param p_packet A pointer to the packet.
 * @param offset The offset, from which to get the character.
 * @return char* The offset buffer pointer.
 */
static inline char* gdb_packet_get_buffer_payload_offset(struct gdb_packet* p_packet, size_t offset) {
    return gdb_packet_get_buffer_offset(p_packet, offset + GDB_PACKET_PREIX_LENGTH);
}

/**
 * @brief Get a packet's buffer pointer.
 *
 * @param p_packet A pointer to the packet.
 * @return char* The buffer pointer.
 */
static inline char* gdb_packet_get_buffer(struct gdb_packet* p_packet) {
    ASSERT_PTR_NOT_NULL(p_packet);

    return gdb_packet_get_buffer_offset(p_packet, 0u);
}

/**
 * @brief Get a packet's payload buffer pointer.
 *
 * @param p_packet A pointer to the packet.
 * @return char* The payload buffer pointer.
 */
static inline char* gdb_packet_get_buffer_payload(struct gdb_packet* p_packet) {
    return gdb_packet_get_buffer_payload_offset(p_packet, 0u);
}

/**
 * @brief Get the command character from a packet.
 *
 * @param p_packet A pointer to the packet.
 * @return char The command character.
 */
static inline char gdb_packet_get_command(struct gdb_packet* p_packet) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_VERBOSE(p_packet->type == GDB_PACKET_TYPE_INBOUND, "Packet is not inbound.");
    ASSERT_VERBOSE((p_packet->state == GDB_PACKET_STATE_COMPLETE) || (p_packet->state == GDB_PACKET_STATE_SENT),
                   "Packet not complete.");

    return *gdb_packet_get_buffer_offset(p_packet, GDB_PACKET_PREIX_LENGTH);
}

#endif  // SOURCE_GDB_GDB_PACKET_H_

/**
 * @}
 */
