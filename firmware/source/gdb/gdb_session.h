// Copyright 2023 elagil

/**
 * @file
 * @brief   The GDB session module headers.
 *
 * @addtogroup gdb
 * @{
 */

#ifndef SOURCE_GDB_GDB_SESSION_H_
#define SOURCE_GDB_GDB_SESSION_H_

#include "ch.h"
#include "gdb_packet.h"

typedef bool (*p_gdb_write_cb_t)(char*, size_t);

enum gdb_session_state {
    GDB_SESSION_STATE_IDLE,
    GDB_SESSION_STATE_ACTIVE,
    GDB_SESSION_STATE_ABORTED,
};

/**
 * @brief The selected session transport.
 */
enum gdb_session_transport {
    GDB_SESSION_TRANSPORT_NONE,     ///< No transport (when session is inactive).
    GDB_SESSION_TRANSPORT_USB_CDC,  ///< The USB CDC transport.
    GDB_SESSION_TRANSPORT_TCP_IP,   ///< The TCP/IP transport.
};

/**
 * @brief The GDB session structure.
 */
struct gdb_session {
    struct gdb_packet input_packet;
    struct gdb_packet output_packet;

    p_gdb_write_cb_t           p_write_cb;
    enum gdb_session_transport transport;  ///< The type of transport to use for the session.

    struct {
        bool b_target_is_running;
        bool b_is_extended_remote;
        bool b_non_stop;
    } properties;

    enum gdb_session_state state;
    binary_semaphore_t     lock;  ///< The session locking semaphore. FIXME: Make generic.
};

enum gdb_session_state gdb_session_get_state(void);

size_t gdb_session_handle(const char* p_input, const size_t input_size);
void   gdb_session_write(void);

bool gdb_session_lock(enum gdb_session_transport transport, p_gdb_write_cb_t p_write_cb);
void gdb_session_release(void);

void gdb_session_init(void);

#endif  // SOURCE_GDB_GDB_SESSION_H_

/**
 * @}
 */
