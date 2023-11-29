#ifndef GDB_GDB_PACKET_H_
#define GDB_GDB_PACKET_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#include "common/common.h"

#define GDB_PACKET_OVERHEAD_SIZE   4u  // Start, stop, and two checksum bytes.
#define GDB_PACKET_MAX_BUFFER_SIZE 2048u

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
    GDB_PACKET_CHAR_NULL               = '\0',
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
    char   buffer[GDB_PACKET_MAX_BUFFER_SIZE];
    size_t written_char_count;

    uint8_t checksum;
    uint8_t reference_checksum;

    enum gdb_packet_type  type;
    enum gdb_packet_state state;
};

void                   gdb_packet_init(struct gdb_packet* p_packet, enum gdb_packet_type type);
enum gdb_packet_result gdb_packet_read_from_stream(struct gdb_packet* p_packet, const char* p_input,
                                                   const size_t input_size, size_t* p_consumed_size);

enum gdb_packet_result gdb_packet_write(struct gdb_packet* p_packet, const char* p_characters);
enum gdb_packet_result gdb_packet_write_single(struct gdb_packet* p_packet, const char character);

/**
 * @brief Get the command character from a packet.
 *
 * @param p_packet A pointer to the packet.
 * @return char The command character.
 */
static inline char gdb_packet_get_command(struct gdb_packet* p_packet) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_VERBOSE(p_packet->type == GDB_PACKET_TYPE_INBOUND, "Packet is not inbound.");
    ASSERT_VERBOSE(p_packet->state == GDB_PACKET_STATE_COMPLETE, "Packet not complete.");

    return p_packet->buffer[1u];
}

/**
 * @brief Get the full size of a packet.
 *
 * @param p_packet A pointer to the packet.
 * @return size_t The size of the packet.
 */
static inline size_t gdb_packet_get_size(struct gdb_packet* p_packet) {
    ASSERT_PTR_NOT_NULL(p_packet);
    ASSERT_VERBOSE(p_packet->state == GDB_PACKET_STATE_COMPLETE, "Packet not complete.");

    return p_packet->written_char_count;
}

/**
 * @brief Get the size of a packet, excluding overhead (start, stop, checksum characters).
 *
 * @param p_packet A pointer to the packet.
 * @return size_t The size of the packet.
 */
static inline size_t gdb_packet_get_payload_size(struct gdb_packet* p_packet) {
    size_t packet_size = gdb_packet_get_size(p_packet);

    ASSERT_VERBOSE(packet_size >= GDB_PACKET_OVERHEAD_SIZE, "Packet size too small.");
    return packet_size - GDB_PACKET_OVERHEAD_SIZE;
}

/**
 * @brief Get a packet's buffer pointer.
 *
 * @param p_packet A pointer to the packet.
 * @return char* The buffer pointer.
 */
static inline char* gdb_packet_get_buffer(struct gdb_packet* p_packet) {
    ASSERT_PTR_NOT_NULL(p_packet);

    return p_packet->buffer;
}

#endif  // GDB_GDB_PACKET_H_
