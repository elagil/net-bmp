// Copyright 2023 elagil

/**
 * @file
 * @brief   The headers fot the GDB query handler module for the 'qRcmd' subcommand.
 *
 * @addtogroup gdb
 * @{
 */

#ifndef GDB_COMMANDS_GDB_COMMAND_QUERY_RCMD_H_
#define GDB_COMMANDS_GDB_COMMAND_QUERY_RCMD_H_

#include "gdb/gdb_session.h"
#include "gdb_command.h"

#define GDB_COMMAND_QUERY_RCMD "qRcmd,"
void gdb_command_query_rcmd(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv);

#endif  // GDB_COMMANDS_GDB_COMMAND_QUERY_RCMD_H_

/**
 * @}
 */
