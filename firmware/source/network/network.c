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

#include "gdb_main.h"
#include "gdb_packet.h"
#include "lwip/api.h"
#include "lwip/netif.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwipthread.h"

/**
 * @brief End of transmission.
 */
#define NETWORK_EOT '\x04'

#define NETWORK_STACK_SIZE       1024u
#define NETWORK_CHAR_BUFFER_SIZE 1024u

static char pbuf[GDB_PACKET_BUFFER_SIZE + 1U] __attribute__((aligned(8)));

struct network_context {
    char            receive_buffer[NETWORK_CHAR_BUFFER_SIZE];
    size_t          received_char_count;
    struct netconn *p_conn;
} g_context;

static void network_receive(const uint32_t timeout_ms) {
    struct netbuf *p_recv_buf;

    netconn_set_recvtimeout(g_context.p_conn, timeout_ms);

    // Allocates a netbuf.
    err_t err = netconn_recv(g_context.p_conn, &p_recv_buf);

    if (err == ERR_OK) {
        void    *p_data      = NULL;
        uint16_t buffer_size = 0u;
        netbuf_data(p_recv_buf, &p_data, &buffer_size);

        if ((g_context.received_char_count + buffer_size) <= NETWORK_CHAR_BUFFER_SIZE) {
            memcpy(&g_context.receive_buffer[g_context.received_char_count], p_data, buffer_size);
            g_context.received_char_count += buffer_size;
        }
    }

    netbuf_delete(p_recv_buf);
}

void network_putchar(const char c) { netconn_write(g_context.p_conn, &c, 1, NETCONN_COPY); }

char network_getchar_immediate(void) {
    if (g_context.received_char_count == 0u) {
        return NETWORK_EOT;
    }

    g_context.received_char_count--;
    return g_context.receive_buffer[g_context.received_char_count];
}

char network_getchar_timeout(const uint32_t timeout_ms) {
    while (g_context.received_char_count == 0) {
        network_receive(timeout_ms);
    }

    return network_getchar_immediate();
}

/**
 * @brief Serve a connection.
 * @details Receive data from the stream.
 *
 * @param p_conn A pointer to the connection to serve.
 */
static void network_serve(struct netconn *p_conn) {
    g_context.received_char_count = 0u;
    g_context.p_conn              = p_conn;

    size_t size = gdb_getpacket(pbuf, GDB_PACKET_BUFFER_SIZE);
    // If port closed and target detached, stay idle
    // if (pbuf[0] != '\x04' || cur_target) {
    //     SET_IDLE_STATE(false);
    // }
    gdb_main(pbuf, GDB_PACKET_BUFFER_SIZE, size);
}

THD_WORKING_AREA(wa_tcp_server, NETWORK_STACK_SIZE);

/**
 * @brief The TCP server thread.
 *
 * @param p_arg A pointer to arguments to the TCP server, unused.
 */
THD_FUNCTION(tcp_server, p_arg) {
    (void)p_arg;
    struct netconn *p_tcp_conn = NULL;
    err_t           err;

    chRegSetThreadName("tcp_server");

    p_tcp_conn = netconn_new(NETCONN_TCP);
    LWIP_ERROR("tcp: invalid conn", (p_tcp_conn != NULL), chThdExit(MSG_RESET););

    err = netconn_bind(p_tcp_conn, IP4_ADDR_ANY, NETWORK_FIRST_TCP_PORT);
    LWIP_ERROR("tcp: could not bind", (err == ERR_OK), chThdExit(MSG_RESET););

    netconn_listen(p_tcp_conn);

    // Set final thread priority.
    chThdSetPriority(LOWPRIO + 2);

    while (true) {
        struct netconn *p_new_conn = NULL;

        err = netconn_accept(p_tcp_conn, &p_new_conn);
        if (err != ERR_OK) {
            continue;
        }

        network_serve(p_new_conn);
        netconn_delete(p_new_conn);
    }
}

/**
 * @brief Initialize the networking thread.
 */
void network_init(void) {
    lwipInit(NULL);

    chThdCreateStatic(wa_tcp_server, sizeof(wa_tcp_server), NORMALPRIO + 1, tcp_server, NULL);
}

/**
 * @}
 */
