// Copyright 2023 elagil

/**
 * @file
 * @brief   The GDB query ('q') (sub)command handling module.
 *
 * @addtogroup gdb
 * @{
 */

#include "gdb_command_query.h"

#include <string.h>

#include "common/hex.h"
#include "gdb_command_query_rcmd.h"

#define GDB_COMMAND_QUERY_BUFFER_LENGTH 256u

/**
 * @brief Supported query commands.
 */
const struct gdb_subcommand G_QUERY_COMMANDS[] = {
    {GDB_COMMAND_QUERY_RCMD, NULL, gdb_command_query_rcmd},
};

/**
 * @brief Entry point to handling query commands.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 */
void gdb_command_query(struct gdb_session* p_gdb_session) {
    gdb_command_execute_sub(gdb_packet_get_buffer_payload(&p_gdb_session->input_packet), p_gdb_session,
                            G_QUERY_COMMANDS, ARRAY_LENGTH(G_QUERY_COMMANDS), 0u, NULL);
}

/**
 * @}
 */
