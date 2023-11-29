// Copyright 2023 elagil

/**
 * @file
 * @brief   The GDB session module headers.
 *
 * @addtogroup gdb
 * @{
 */

#ifndef GDB_SESSION_H_
#define GDB_SESSION_H_

#include "ch.h"
#include "gdb_packet.h"

/**
 * @brief The total GDB receive buffer size.
 */
#define GDB_SESSION_RECEIVE_BUFFER_SIZE 2048u

#define GDB_SESSION_MESSAGE_BUFFER_SIZE 128u

/**
 * @brief The selected session transport.
 */
enum gdb_session_transport {
    GDB_SESSION_TRANSPORT_NONE,     ///< No transport (when session is inactive).
    GDB_SESSION_TRANSPORT_USB_CDC,  ///< The USB CDC transport.
    GDB_SESSION_TRANSPORT_TCP_IP,   ///< The TCP/IP transport.
};

enum gdb_session_state {
    GDB_SESSION_STATE_IDLE,
    GDB_SESSION_STATE_WAIT_ACK,
    GDB_SESSION_STATE_SEND_ACK,
    GDB_SESSION_STATE_SEND_NACK,
    GDB_SESSION_STATE_HANDLE_COMMAND,
};

/**
 * @brief The GDB session structure.
 */
struct gdb_session {
    struct gdb_packet input_packet;
    struct gdb_packet output_packet;

    enum gdb_session_transport transport;  ///< The type of transport to use for the session.
    enum gdb_session_state     state;
    binary_semaphore_t         lock;  ///< The session locking semaphore. FIXME: Make generic.
};

size_t gdb_session_receive(const char* p_input, const size_t input_size);
void   gdb_session_execute(char** pp_output, size_t* p_output_size);

bool gdb_session_lock(enum gdb_session_transport transport);
void gdb_session_release(void);

void gdb_session_init(void);

#endif  // GDB_SESSION_H_

/**
 * @}
 */
