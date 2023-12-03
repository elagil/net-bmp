// Copyright 2023 elagil

/**
 * @file
 * @brief   The GDB query ('q') (sub)command handling module headers.
 *
 * @addtogroup gdb
 * @{
 */

#ifndef GDB_COMMANDS_GDB_COMMAND_QUERY_H_
#define GDB_COMMANDS_GDB_COMMAND_QUERY_H_

#include "gdb/gdb_session.h"
#include "gdb_command.h"

void gdb_command_query(struct gdb_session* p_gdb_session);

#endif  // GDB_COMMANDS_GDB_COMMAND_QUERY_H_

/**
 * @}
 */
