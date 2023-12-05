// Copyright 2023 elagil

/**
 * @file
 * @brief   The headers for the GDB query handler module for the 'qRcmd' (remote) subcommand.
 *
 * @addtogroup gdb
 * @{
 */

#ifndef GDB_COMMANDS_GDB_QUERY_REMOTE_H_
#define GDB_COMMANDS_GDB_QUERY_REMOTE_H_

#include "gdb/gdb.h"
#include "gdb/gdb_session.h"

#define GDB_QUERY_REMOTE "qRcmd,"
void gdb_query_remote(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv);

#endif  // GDB_COMMANDS_GDB_QUERY_REMOTE_H_

/**
 * @}
 */
