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

#define GDB_QUERY_SUPPORTED_FEATURES     "qSupported"
#define GDB_QUERY_CURRENT_THREAD_ID      "qC"
#define GDB_QUERY_CRC                    "qCRC"
#define GDB_QUERY_MEMORY_MAP_READ        "qXfer:memory-map:read::"
#define GDB_QUERY_FEATURES_READ          "qXfer:features:read:target.xml:"
#define GDB_QUERY_FIRST_THREAD_INFO      "qfThreadInfo"
#define GDB_QUERY_SUBSEQUENT_THREAD_INFO "qsThreadInfo"
#define GDB_QUERY_START_NO_ACK_MODE      "QStartNoAckMode"
#define GDB_QUERY_ATTACHED_TO_EXISTING   "qAttached"

/**
 * @brief Respond to the query of supported features.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param argc The argument count (unused).
 * @param p_argv The list of argument string pointers (unused).
 */
static void gdb_query_supported(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv) {
    (void)argc;
    (void)p_argv;

    char message[GDB_QUERY_MESSAGE_MAX_LENGTH];

    // FIXME: add ";qXfer:memory-map:read+;qXfer:features:read+;vContSupported+;QStartNoAckMode+" features
    SNPRINTF(message, ARRAY_LENGTH(message), "PacketSize=%X", GDB_PACKET_MAX_USABLE_BUFFER_LENGTH);

    gdb_reply(p_gdb_session, message);
}

/**
 * @brief Respond to the query of the current thread ID.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param argc The argument count (unused).
 * @param p_argv The list of argument string pointers (unused).
 */
static void gdb_query_current_thread_id(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv) {
    (void)argc;
    (void)p_argv;

    // At the moment, only support a single thread (thread 1).
    gdb_reply(p_gdb_session, "QC1");
}

/**
 * @brief Respond to the first query of thread information.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param argc The argument count (unused).
 * @param p_argv The list of argument string pointers (unused).
 */
static void gdb_query_first_thread_info(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv) {
    (void)argc;
    (void)p_argv;

    if (p_gdb_session->p_target != NULL) {
        // At the moment, only support a single thread (thread 1).
        gdb_reply(p_gdb_session, "m1");
    } else {
        // End of the list.
        gdb_reply(p_gdb_session, "l");
    }
}

/**
 * @brief Respond to subsequent query of thread information.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param argc The argument count (unused).
 * @param p_argv The list of argument string pointers (unused).
 */
static void gdb_query_subsequent_thread_info(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv) {
    (void)argc;
    (void)p_argv;

    // End of the list - only a single thread is currently supported.
    gdb_reply(p_gdb_session, "l");
}

/**
 * @brief Start the no-ACK mode.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param argc The argument count (unused).
 * @param p_argv The list of argument string pointers (unused).
 */
static void gdb_query_start_no_ack_mode(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv) {
    (void)argc;
    (void)p_argv;

    p_gdb_session->properties.b_no_ack_mode = true;

    gdb_reply(p_gdb_session, GDB_REPLY_OK);
}

/**
 * @brief Supported query commands.
 */
const struct gdb_subcommand G_QUERY_COMMANDS[] = {
    {GDB_QUERY_REMOTE, NULL, gdb_query_remote},
    {GDB_QUERY_SUPPORTED_FEATURES, NULL, gdb_query_supported},
    {GDB_QUERY_CURRENT_THREAD_ID, NULL, gdb_query_current_thread_id},
    {GDB_QUERY_CRC, NULL, NULL},
    {GDB_QUERY_MEMORY_MAP_READ, NULL, NULL},
    {GDB_QUERY_FEATURES_READ, NULL, NULL},
    {GDB_QUERY_FIRST_THREAD_INFO, NULL, gdb_query_first_thread_info},
    {GDB_QUERY_SUBSEQUENT_THREAD_INFO, NULL, gdb_query_subsequent_thread_info},
    {GDB_QUERY_START_NO_ACK_MODE, NULL, gdb_query_start_no_ack_mode},
    {GDB_QUERY_ATTACHED_TO_EXISTING, NULL, NULL},
};

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
