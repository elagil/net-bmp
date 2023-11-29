#include "gdb_command.h"

#include <string.h>

struct gdb_command {
    char key;
    void (*p_callback)(struct gdb_packet*, struct gdb_packet*);
};

/**
 * @brief Write a GDB 'OK' response packet.
 *
 * @param p_output_packet A pointer to the output packet to write.
 */
static void gdb_command_write_response_ok(struct gdb_packet* p_output_packet) {
    gdb_packet_write(p_output_packet, GDB_COMMAND_RESPONSE_OK);
}

/**
 * @brief Write an empty GDB response packet.
 *
 * @param p_output_packet A pointer to the output packet to write.
 */
static void gdb_command_write_response_empty(struct gdb_packet* p_output_packet) {
    gdb_packet_write(p_output_packet, GDB_COMMAND_RESPONSE_EMPTY);
}

/**
 * @brief
 * @note The function does not take arguments from the input packet.
 *
 * @param p_input_packet A pointer to the input packet.
 * @param p_output_packet A pointer to the output packet.
 * @return enum gdb_command_result The GDB command result code.
 */
static void gdb_command_extended_remote(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_ok(p_output_packet);
}

/**
 * @brief
 * @note The function does not take arguments from the input packet.
 *
 * @param p_input_packet A pointer to the input packet.
 * @param p_output_packet A pointer to the output packet.
 * @return enum gdb_command_result The GDB command result code.
 */
static void gdb_command_stop_reason_query(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    // FIXME: If no target connected.
    gdb_packet_write(p_output_packet, "W00");

    // gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_resume_addr(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_resume_signal(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_detach(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

/**
 * @brief
 * @note The function does not take arguments from the input packet.
 *
 * @param p_input_packet A pointer to the input packet.
 * @param p_output_packet A pointer to the output packet.
 */
static void gdb_command_get_registers(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_ok(p_output_packet);
}

static void gdb_command_set_registers(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_set_thread(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    // FIXME: Extract thread number, if applicable.

    gdb_command_write_response_ok(p_output_packet);
}

/**
 * @brief
 * @note The function does not take arguments from the input packet.
 *
 * @param p_input_packet A pointer to the input packet.
 * @param p_output_packet A pointer to the output packet.
 */
static void gdb_command_kill(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_ok(p_output_packet);
}

static void gdb_command_get_memory(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_write_memory_hex(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_read_register(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_write_register(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_handle_query(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_handle_general_set(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_restart(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_step(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_step_signal(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_is_thread_alive(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_v(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_write_memory(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_insert_breakpoint(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

static void gdb_command_remove_breakpoint(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    (void)p_input_packet;

    gdb_command_write_response_empty(p_output_packet);
}

/**
 * @brief A mapping of GDB commands to handler functions.
 */
const struct gdb_command G_COMMANDS[] = {
    {'!', gdb_command_extended_remote},     // Enable extended remote mode.
    {'?', gdb_command_stop_reason_query},   // Stop reason query.
    {'c', gdb_command_resume_addr},         // Continue (at addr)
    {'C', gdb_command_resume_signal},       // Continue with signal.
    {'D', gdb_command_detach},              // Detach.
    {'g', gdb_command_get_registers},       // Read general registers.
    {'G', gdb_command_set_registers},       // Write general registers.
    {'H', gdb_command_set_thread},          // Set thread for subsequent operations.
    {'k', gdb_command_kill},                // Kill.
    {'m', gdb_command_get_memory},          // Read memory.
    {'M', gdb_command_write_memory_hex},    // Write memory (hex).
    {'p', gdb_command_read_register},       // Read register.
    {'P', gdb_command_write_register},      // Write register.
    {'q', gdb_command_handle_query},        // General query
    {'Q', gdb_command_handle_general_set},  // General set.
    {'R', gdb_command_restart},             // Extended remote restart command.
    {'s', gdb_command_step},                // Single step.
    {'S', gdb_command_step_signal},         // Step with signal.
    {'T', gdb_command_is_thread_alive},     // Thread liveliness query.
    {'v', gdb_command_v},                   // Group of 'v' commands.
    {'X', gdb_command_write_memory},        // Write memory (binary).
    {'z', gdb_command_insert_breakpoint},   // Insert breakpoint/watchpoint.
    {'Z', gdb_command_remove_breakpoint},   // Remove breakpoint/watchpoint.
};

/**
 * @brief Execute a command, determined from the content of an input packet.
 *
 * @param p_input_packet A pointer to the input packet.
 * @param p_output_packet A pointer to the output (response) packet.
 */
void gdb_command_execute(struct gdb_packet* p_input_packet, struct gdb_packet* p_output_packet) {
    ASSERT_PTR_NOT_NULL(p_input_packet);
    ASSERT_PTR_NOT_NULL(p_output_packet);

    ASSERT_VERBOSE(p_input_packet != p_output_packet, "Input and output packet pointers must not match.");
    ASSERT_VERBOSE(p_input_packet->state == GDB_PACKET_STATE_COMPLETE, "Cannot handle incomplete input packet.");
    ASSERT_VERBOSE(p_input_packet->type == GDB_PACKET_TYPE_INBOUND, "Input packet not marked as inbound.");

    char command = gdb_packet_get_command(p_input_packet);

    const struct gdb_command* p_command = NULL;
    for (size_t command_index = 0; command_index < ARRAY_LENGTH(G_COMMANDS); command_index++) {
        if (command == G_COMMANDS[command_index].key) {
            p_command = &G_COMMANDS[command_index];
            break;
        }
    }

    if (p_command == NULL) {
        gdb_command_write_response_empty(p_output_packet);
    }

    ASSERT_PTR_NOT_NULL(p_command->p_callback);
    return p_command->p_callback(p_input_packet, p_output_packet);
}
