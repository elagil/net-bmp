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

#include "gdb/gdb.h"
#include "gdb/gdb_session.h"

/**
 * @brief The maximum length of a query message.
 */
#define GDB_QUERY_MESSAGE_MAX_LENGTH 256u

void gdb_query(struct gdb_session* p_gdb_session);

#endif  // GDB_COMMANDS_GDB_COMMAND_QUERY_H_

/**
 * @}
 */
