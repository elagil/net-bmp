// Copyright 2023 elagil

/**
 * @file
 * @brief   The network module.
 * @details Provides a TCP server for GDB to connect to.
 *
 * @addtogroup network
 * @{
 */

#include "network.h"

#include <string.h>

#include "gdb/gdb_session.h"
#include "lwip/api.h"
#include "lwip/netif.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwipthread.h"

#define NETWORK_TCP_SERVER_STACK_SIZE 2048u

static struct network_gdb_session {
    struct netconn *p_conn;  ///< A pointer to the netconn structure, on which the GDB client is served.
    err_t           err;
} g_network_gdb_session;

static bool network_gdb_write_cb(char *p_data, size_t size) {
    g_network_gdb_session.err = netconn_write(g_network_gdb_session.p_conn, p_data, size, NETCONN_COPY);
    return (g_network_gdb_session.err == ERR_OK) ? true : false;
}

/**
 * @brief Serve GDB on a connection.
 *
 * @param p_netbuf A pointer to the received netbuf.
 * @returns err_t An error code.
 */
static err_t network_serve_gdb(struct netbuf *p_netbuf) {
    char    *p_data    = NULL;
    uint16_t data_size = 0u;

    RETURN_IF_NOT(netbuf_data(p_netbuf, (void **)&p_data, &data_size), ERR_OK);
    ASSERT_PTR_NOT_NULL(p_data);

    size_t total_consumed_size = 0u;
    while (total_consumed_size != data_size) {
        ASSERT_VERBOSE(data_size >= total_consumed_size, "Data size inconsistent.");

        size_t consumed_size = gdb_session_handle(p_data, data_size - total_consumed_size);
        RETURN_IF_NOT(g_network_gdb_session.err, ERR_OK);

        if (gdb_session_get_state() == GDB_SESSION_STATE_ABORTED) {
            return ERR_ABRT;
        }

        ASSERT_VERBOSE(consumed_size != 0, "No data consumed.");

        total_consumed_size += consumed_size;
        p_data = &p_data[consumed_size];
    }

    return ERR_OK;
}

/**
 * @brief Serve the connection in an endless loop.
 * @details Break the loop, if there is a connection error.
 */
static void network_serve(void) {
    err_t          err = ERR_OK;
    struct netbuf *p_netbuf;

    while (err == ERR_OK) {
        err = netconn_recv(g_network_gdb_session.p_conn, &p_netbuf);

        if (err == ERR_OK) {
            err = network_serve_gdb(p_netbuf);
        }

        netbuf_delete(p_netbuf);
    }
}

THD_WORKING_AREA(wa_network_tcp_server, NETWORK_TCP_SERVER_STACK_SIZE);

/**
 * @brief The TCP server thread.
 *
 * @param p_arg A pointer to arguments to the TCP server, unused.
 */
THD_FUNCTION(network_tcp_server, p_arg) {
    (void)p_arg;
    struct netconn *p_tcp_conn = NULL;
    err_t           err;

    chRegSetThreadName("network_tcp_server");

    p_tcp_conn = netconn_new(NETCONN_TCP);
    LWIP_ERROR("tcp: invalid conn", (p_tcp_conn != NULL), chThdExit(MSG_RESET););

    err = netconn_bind(p_tcp_conn, IP4_ADDR_ANY, NETWORK_FIRST_TCP_PORT);
    LWIP_ERROR("tcp: could not bind", (err == ERR_OK), chThdExit(MSG_RESET););

    netconn_listen(p_tcp_conn);

    // Set final thread priority.
    chThdSetPriority(LOWPRIO + 2);

    while (true) {
        g_network_gdb_session.p_conn = NULL;

        err = netconn_accept(p_tcp_conn, &g_network_gdb_session.p_conn);
        if (err != ERR_OK) {
            continue;
        }

        if (!gdb_session_lock(GDB_SESSION_TRANSPORT_TCP_IP, network_gdb_write_cb)) {
            // Session is already locked.
            netconn_close(g_network_gdb_session.p_conn);
            netconn_delete(g_network_gdb_session.p_conn);
            continue;
        }

        network_serve();
        netconn_close(g_network_gdb_session.p_conn);
        netconn_delete(g_network_gdb_session.p_conn);

        gdb_session_release();
    }
}

/**
 * @brief Initialize the networking thread.
 */
void network_init(void) {
    lwipInit(NULL);

    chThdCreateStatic(wa_network_tcp_server, sizeof(wa_network_tcp_server), NORMALPRIO + 1, network_tcp_server, NULL);
}

/**
 * @}
 */
