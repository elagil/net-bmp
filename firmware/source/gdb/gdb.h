// Copyright 2023 elagil

/**
 * @file
 * @brief   The main GDB module headers.
 *
 * @addtogroup gdb
 * @{
 */

#ifndef SOURCE_GDB_GDB_H_
#define SOURCE_GDB_GDB_H_

#include <stdint.h>

#include "common/common.h"
#include "gdb_packet.h"
#include "gdb_session.h"

/**
 * @brief The maximum number of commands that the GDB server accepts on queries.
 */
#define GDB_MAX_ARG_COUNT 32u

#define GDB_REPLY_EMPTY          ""    // An empty response, usually for unsupported requests.
#define GDB_REPLY_OK             "OK"  // A reply for successful execution of a command.
#define GDB_REPLY_CONSOLE_OUTPUT "O"   // Leads a reply that results in GDB console output.

// The indices of error replies are not used by the GDB client - they can be chosen arbitrarily.
#define GDB_REPLY_ERROR_01 "E01"  // Error reply with index 1
#define GDB_REPLY_ERROR_02 "E02"  // Error reply with index 2
#define GDB_REPLY_ERROR_03 "E03"  // Error reply with index 3

/**
 * @brief GDB result codes for handling commands.
 */
enum gdb_result {
    GDB_RESULT_OK,                ///< The command was handled correctly.
    GDB_RESULT_EXCESS_ARGUMENTS,  ///< The command had too many arguments.
};

/**
 * @brief The command callback function pointer type.
 */
typedef void (*command_cb_t)(struct gdb_session*);

/**
 * @brief The subcommand callback function pointer type. Takes arguments.
 */
typedef void (*subcommand_cb_t)(struct gdb_session*, const size_t, const char*);

/**
 * @brief A GDB command connects a command (packet) character with a callback function.
 */
struct gdb_command {
    const char   command;     ///< The command character.
    command_cb_t p_callback;  ///< The callback to execute for the packet.
};

/**
 * @brief A GDB subcommand connects a command string with a callback function.
 */
struct gdb_subcommand {
    const char*     p_subcommand;  ///< The subcommand string.
    const char*     p_help;
    subcommand_cb_t p_callback;  ///< The callback to execute for the packet.
};

enum gdb_result gdb_get_args(char* p_string, size_t* p_argc, const char** pp_argv);

void gdb_execute(struct gdb_session* p_gdb_session);
void gdb_execute_sub(const char* p_command_string, struct gdb_session* p_gdb_session,
                     const struct gdb_subcommand* p_gdb_subcommands, const size_t gdb_subcommands_length,
                     const size_t argc, const char* p_argv);

/**
 * @brief Write a GDB reply packet to the session, and flush it to the transport.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param reply The reply to send.
 */
static inline void gdb_reply(struct gdb_session* p_gdb_session, char* reply) {
    gdb_packet_write(&p_gdb_session->output_packet, reply);
    gdb_session_flush(p_gdb_session);
}

#endif  // SOURCE_GDB_GDB_H_

/**
 * @}
 */
