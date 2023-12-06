// Copyright 2023 elagil

/**
 * @file
 * @brief   The GDB query handler module for the 'qRcmd' (remote) subcommand.
 *
 * @addtogroup gdb
 * @{
 */

#include "gdb_query_remote.h"

#include <string.h>

#include "common/hex.h"
#include "gdb/gdb_packet.h"
#include "gdb_query.h"

static void gdb_query_remote_help(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv);
static void gdb_query_remote_version(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv);

/**
 * @brief The supported monitor subcommands.
 */
const struct gdb_subcommand G_QUERY_REMOTE_SUBCOMMANDS[] = {
    {"help", "Show the supported monitor commands.", gdb_query_remote_help},
    {"version", "Show firmware version information.", gdb_query_remote_version}};

/**
 * @brief Respond to the help query, and provide a list of all supported monitor subcommands.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param argc The argument count (unused).
 * @param p_argv The list of argument string pointers.
 */
void gdb_query_remote_help(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv) {
    (void)argc;
    (void)p_argv;

    struct gdb_packet* p_output_packet = &p_gdb_session->output_packet;

    gdb_packet_write_start(p_output_packet);
    gdb_packet_write_payload_as_hex(p_output_packet, "Supported monitor commands:\n\n");

    for (size_t command_index = 0; command_index < ARRAY_LENGTH(G_QUERY_REMOTE_SUBCOMMANDS); command_index++) {
        gdb_packet_write_payload_as_hex(p_output_packet, G_QUERY_REMOTE_SUBCOMMANDS[command_index].p_subcommand);
        gdb_packet_write_payload_as_hex(p_output_packet, ": ");
        gdb_packet_write_payload_as_hex(p_output_packet, G_QUERY_REMOTE_SUBCOMMANDS[command_index].p_help);
        gdb_packet_write_payload_as_hex(p_output_packet, "\n");
    }

    gdb_packet_write_stop(p_output_packet);

    gdb_session_flush(p_gdb_session);
}

/**
 * @brief Respond to a version query.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param argc The argument count (unused).
 * @param p_argv The list of argument string pointers.
 */
static void gdb_query_remote_version(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv) {
    (void)argc;
    (void)p_argv;

    struct gdb_packet* p_output_packet = &p_gdb_session->output_packet;

    gdb_packet_write_start(p_output_packet);
    gdb_packet_write_payload_as_hex(p_output_packet, "Version: ");
    gdb_packet_write_payload_as_hex(p_output_packet, VERSION);
    gdb_packet_write_payload_as_hex(p_output_packet, "-");
    gdb_packet_write_payload_as_hex(p_output_packet, COMMIT_HASH);
    gdb_packet_write_payload_as_hex(p_output_packet, "\n");
    gdb_packet_write_stop(p_output_packet);

    gdb_session_flush(p_gdb_session);
}

/**
 * @brief Execute a remote command on the server.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param argc The argument count (unused).
 * @param p_argv The list of argument string pointers (unused).
 */
void gdb_query_remote(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv) {
    (void)argc;
    (void)p_argv;

    const size_t       PREFIX_LENGTH  = strlen(GDB_QUERY_REMOTE);
    struct gdb_packet* p_input_packet = &p_gdb_session->input_packet;

    char message[GDB_QUERY_MESSAGE_MAX_LENGTH];
    str_from_hex((uint8_t*)gdb_packet_get_buffer_payload_offset(p_input_packet, PREFIX_LENGTH),
                 gdb_packet_get_payload_length(p_input_packet) - PREFIX_LENGTH, message, ARRAY_LENGTH(message));

    volatile size_t      extracted_argc = 0u;
    volatile const char* p_extracted_argv[GDB_MAX_ARG_COUNT];
    gdb_get_args(message, (size_t*)&extracted_argc, (const char**)&p_extracted_argv);

    // The 0th argument is the name of the subcommand to call.
    if (extracted_argc > 0u) {
        gdb_execute_sub((const char*)p_extracted_argv[0u], p_gdb_session, G_QUERY_REMOTE_SUBCOMMANDS,
                        ARRAY_LENGTH(G_QUERY_REMOTE_SUBCOMMANDS), extracted_argc, (const char*)p_extracted_argv);
    }
}

/**
 * @}
 */
