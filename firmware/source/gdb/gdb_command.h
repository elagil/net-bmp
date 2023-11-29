#ifndef GDB_GDB_COMMAND_H_
#define GDB_GDB_COMMAND_H_

#include <stdint.h>

#include "common/common.h"
#include "gdb_packet.h"

#define GDB_COMMAND_RESPONSE_EMPTY ""
#define GDB_COMMAND_RESPONSE_OK    "OK"

void gdb_command_execute(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet);

#endif  // GDB_GDB_COMMAND_H_
