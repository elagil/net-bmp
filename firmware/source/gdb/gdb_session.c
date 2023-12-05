// Copyright 2023 elagil

/**
 * @file
 * @brief   The GDB session module.
 * @details Handles a GDB session to a host. There can only be a single session.
 * Implements a transparent channel over which the GDB Remote Serial Debugging protocol is
 * implemented. This implementation uses either a TCP/IP connection, or USB CDC to implement the channel.
 *
 * @addtogroup gdb
 * @{
 */

#include "gdb_session.h"

#include <string.h>

#include "common/common.h"
#include "gdb.h"
#include "gdb_packet.h"
#include "network.h"

/**
 * @brief The GDB session. There can only be one connection at a time.
 */
struct gdb_session g_gdb_session;

enum gdb_session_state gdb_session_get_state(void) { return g_gdb_session.state; }

void gdb_session_write(void) {
    bool b_success = g_gdb_session.p_write_cb(gdb_packet_get_buffer(&g_gdb_session.output_packet),
                                              gdb_packet_get_length(&g_gdb_session.output_packet));

    gdb_packet_mark_sent(&g_gdb_session.output_packet);

    if (b_success) {
        return;
    }

    switch (g_gdb_session.transport) {
        case GDB_SESSION_TRANSPORT_TCP_IP:
            g_gdb_session.state = GDB_SESSION_STATE_ABORTED;
            break;

        default:
            break;
    }

    // FIXME: Add retries, if not TCP.
}

/**
 * @brief Handle data from the stub in the GDB session.
 *
 * @param p_input A pointer to the input data (from reception)
 * @param input_length The input data length.
 * @returns size_t The length of input data that was actually consumed.
 */
size_t gdb_session_handle(const char* p_input, const size_t input_length) {
    ASSERT_VERBOSE(g_gdb_session.state == GDB_SESSION_STATE_ACTIVE, "Session inactive.");
    ASSERT_PTR_NOT_NULL(p_input);

    palSetLine(LINE_LED_GREEN);

    size_t consumed_length = 0u;

    enum gdb_packet_result result =
        gdb_packet_read_stream(&g_gdb_session.input_packet, p_input, input_length, &consumed_length);

    switch (result) {
        case GDB_PACKET_RESULT_OK:
            gdb_packet_write_ack(&g_gdb_session.output_packet, GDB_PACKET_CHAR_ACK);
            gdb_session_write();
            gdb_execute(&g_gdb_session);
            break;

        case GDB_PACKET_RESULT_CHECKSUM_ERROR:
            gdb_packet_write_ack(&g_gdb_session.output_packet, GDB_PACKET_CHAR_NACK);
            gdb_session_write();
            break;

        default:
            break;
    }

    palClearLine(LINE_LED_GREEN);
    return consumed_length;
}

/**
 * @brief Lock the GDB session.
 *
 * @param transport The transport that attempts to lock the session.
 * @param p_write_cb A callback function that allows the GDB session to write onto the stream interface.
 * @return bool True, if the session could be locked, false if it was already.
 */
bool gdb_session_lock(enum gdb_session_transport transport, p_gdb_write_cb_t p_write_cb) {
    bool b_locked = (MSG_OK == chBSemWaitTimeout(&g_gdb_session.lock, TIME_IMMEDIATE));

    if (b_locked) {
        gdb_packet_init(&g_gdb_session.input_packet, GDB_PACKET_TYPE_INBOUND);
        gdb_packet_init(&g_gdb_session.output_packet, GDB_PACKET_TYPE_OUTBOUND);

        g_gdb_session.state      = GDB_SESSION_STATE_ACTIVE;
        g_gdb_session.transport  = transport;
        g_gdb_session.p_write_cb = p_write_cb;
        palSetLine(LINE_LED_AMBER);
    }

    return b_locked;
}

/**
 * @brief Release the GDB session.
 */
void gdb_session_release(void) {
    g_gdb_session.properties.b_target_is_running  = false;
    g_gdb_session.properties.b_is_extended_remote = false;
    g_gdb_session.properties.b_non_stop           = false;
    g_gdb_session.transport                       = GDB_SESSION_TRANSPORT_NONE;
    g_gdb_session.state                           = GDB_SESSION_STATE_IDLE;

    chBSemSignal(&g_gdb_session.lock);
    palClearLine(LINE_LED_AMBER);
}

/**
 * @brief Initialize the GDB session.
 */
void gdb_session_init(void) {
    chBSemObjectInit(&g_gdb_session.lock, true);
    gdb_session_release();
}

/**
 * @}
 */
