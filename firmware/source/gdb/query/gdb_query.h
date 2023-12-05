// Copyright 2023 elagil

/**
 * @file
 * @brief   The GDB query ('q') (sub)command handling module headers.
 *
 * @addtogroup gdb
 * @{
 */

#ifndef SOURCE_GDB_QUERY_GDB_QUERY_H_
#define SOURCE_GDB_QUERY_GDB_QUERY_H_

#include "gdb/gdb.h"
#include "gdb/gdb_session.h"

/**
 * @brief The maximum length of a query message.
 */
#define GDB_QUERY_MESSAGE_MAX_LENGTH 256u

void gdb_query(struct gdb_session* p_gdb_session);

#endif  // SOURCE_GDB_QUERY_GDB_QUERY_H_

/**
 * @}
 */
