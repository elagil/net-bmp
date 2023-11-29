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
#include "gdb_command.h"
#include "gdb_packet.h"
#include "network.h"

/**
 * @brief The GDB session. There can only be one connection at a time.
 */
struct gdb_session g_gdb_session;

/**
 * @brief Receive data from the stub in the GDB session.
 *
 * @param p_input A pointer to the input data (from reception)
 * @param input_size The input data size.
 * @returns size_t The size of input data that was actually consumed.
 */
size_t gdb_session_receive(const char* p_input, const size_t input_size) {
    ASSERT_PTR_NOT_NULL(p_input);
    ASSERT_VERBOSE(g_gdb_session.state == GDB_SESSION_STATE_IDLE, "Invalid session state.");

    size_t consumed_size = 0u;

    enum gdb_packet_result result =
        gdb_packet_read_from_stream(&g_gdb_session.input_packet, p_input, input_size, &consumed_size);

    switch (result) {
        case GDB_PACKET_RESULT_OK:
            g_gdb_session.state = GDB_SESSION_STATE_SEND_ACK;
            break;

        case GDB_PACKET_RESULT_CHECKSUM_ERROR:
            g_gdb_session.state = GDB_SESSION_STATE_SEND_NACK;
            break;

        default:
            g_gdb_session.state = GDB_SESSION_STATE_IDLE;
            break;
    }

    return consumed_size;
}

void gdb_session_execute(char** pp_output, size_t* p_output_size) {
    ASSERT_PTR_NOT_NULL(pp_output);
    ASSERT_PTR_NOT_NULL(p_output_size);

    if (g_gdb_session.state == GDB_SESSION_STATE_IDLE) {
        *pp_output     = NULL;
        *p_output_size = 0u;

        return;
    }

    switch (g_gdb_session.state) {
        case GDB_SESSION_STATE_SEND_ACK:
            gdb_packet_write_single(&g_gdb_session.output_packet, GDB_PACKET_CHAR_ACK);
            g_gdb_session.state = GDB_SESSION_STATE_HANDLE_COMMAND;
            break;

        case GDB_SESSION_STATE_SEND_NACK:
            gdb_packet_write_single(&g_gdb_session.output_packet, GDB_PACKET_CHAR_NACK);
            g_gdb_session.state = GDB_SESSION_STATE_IDLE;
            break;

        case GDB_SESSION_STATE_HANDLE_COMMAND:
            gdb_command_execute(&g_gdb_session.input_packet, &g_gdb_session.output_packet);
            g_gdb_session.state = GDB_SESSION_STATE_IDLE;
            break;

        default:
            ASSERT_NOT_REACHED();
            break;
    }

    *pp_output     = gdb_packet_get_buffer(&g_gdb_session.output_packet);
    *p_output_size = gdb_packet_get_size(&g_gdb_session.output_packet);

    ASSERT_PTR_NOT_NULL(*pp_output);
    ASSERT_VERBOSE(*p_output_size != 0u, "Invalid output size");
}

/**
 * @brief Lock the GDB session.
 *
 * @param transport The transport that attempts to lock the session.
 * @return bool True, if the session could be locked, false if it was already.
 */
bool gdb_session_lock(enum gdb_session_transport transport) {
    bool b_locked = (MSG_OK == chBSemWaitTimeout(&g_gdb_session.lock, TIME_IMMEDIATE));

    if (b_locked) {
        gdb_packet_init(&g_gdb_session.input_packet, GDB_PACKET_TYPE_INBOUND);
        gdb_packet_init(&g_gdb_session.output_packet, GDB_PACKET_TYPE_OUTBOUND);

        g_gdb_session.transport = transport;
        palSetLine(LINE_LED_BLUE);
    }

    return b_locked;
}

/**
 * @brief Release the GDB session.
 */
void gdb_session_release(void) {
    g_gdb_session.state     = GDB_SESSION_STATE_IDLE;
    g_gdb_session.transport = GDB_SESSION_TRANSPORT_NONE;

    chBSemSignal(&g_gdb_session.lock);
    palClearLine(LINE_LED_BLUE);
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
