// Copyright 2023 elagil

/**
 * @file
 * @brief   The headers for the GDB query handler module for the 'qRcmd' (remote) subcommand.
 *
 * @addtogroup gdb
 * @{
 */

#ifndef SOURCE_GDB_QUERY_GDB_QUERY_REMOTE_H_
#define SOURCE_GDB_QUERY_GDB_QUERY_REMOTE_H_

#include "gdb/gdb.h"
#include "gdb/gdb_session.h"

#define GDB_QUERY_REMOTE "qRcmd,"
void gdb_query_remote(struct gdb_session* p_gdb_session, const size_t argc, const char* p_argv);

#endif  // SOURCE_GDB_QUERY_GDB_QUERY_REMOTE_H_

/**
 * @}
 */
