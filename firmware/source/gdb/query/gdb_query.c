// Copyright 2023 elagil

/**
 * @file
 * @brief   The GDB query ('q') (sub)command handling module.
 *
 * @addtogroup gdb
 * @{
 */

#include "gdb_query.h"

#include <string.h>

#include "common/hex.h"
#include "gdb_query_remote.h"

#define GDB_QUERY_SUPPORTED_FEATURES "qSupported"
#define GDB_QUERY_CURRENT_THREAD_ID  "qC"
#define GDB_QUERY_CRC                "qCRC"

/**
 * @brief Respond to the query of supported features.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param argc The argument count (unused).
 * @param p_argv The list of argument string pointers (unused).
 */
void gdb_query_supported(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv) {
    (void)argc;
    (void)p_argv;

    struct gdb_packet* p_output_packet = &p_gdb_session->output_packet;

    char message[GDB_QUERY_MESSAGE_MAX_LENGTH];

    // FIXME: add ";qXfer:memory-map:read+;qXfer:features:read+;vContSupported+;QStartNoAckMode+" features
    SNPRINTF(message, ARRAY_LENGTH(message), "PacketSize=%X", GDB_PACKET_MAX_USABLE_BUFFER_LENGTH);

    gdb_packet_write(p_output_packet, message);

    gdb_session_write();
}

/**
 * @brief Respond to the query of the current thread ID.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param argc The argument count (unused).
 * @param p_argv The list of argument string pointers (unused).
 */
void gdb_query_current_thread_id(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv) {
    (void)argc;
    (void)p_argv;

    struct gdb_packet* p_output_packet = &p_gdb_session->output_packet;

    // At the moment, only support a single thread (thread 1).
    gdb_packet_write(p_output_packet, "QC1");

    gdb_session_write();
}

/**
 * @brief Supported query commands.
 */
const struct gdb_subcommand G_QUERY_COMMANDS[] = {{GDB_QUERY_REMOTE, NULL, gdb_query_remote},
                                                  {GDB_QUERY_SUPPORTED_FEATURES, NULL, gdb_query_supported},
                                                  {GDB_QUERY_CURRENT_THREAD_ID, NULL, gdb_query_current_thread_id}};

/**
 * @brief Entry point to handling query commands.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 */
void gdb_query(struct gdb_session* p_gdb_session) {
    gdb_execute_sub(gdb_packet_get_buffer_payload(&p_gdb_session->input_packet), p_gdb_session, G_QUERY_COMMANDS,
                    ARRAY_LENGTH(G_QUERY_COMMANDS), 0u, NULL);
}

/**
 * @}
 */
