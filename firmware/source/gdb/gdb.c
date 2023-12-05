// Copyright 2023 elagil

/**
 * @file
 * @brief   The general GDB module.
 * @details This is the entry point to handling commands from the GDB client. From here, the commands are parsed and
 * execution branches off to subommand levels.
 *
 * @addtogroup gdb
 * @{
 */

#include "gdb.h"

#include <string.h>

#include "query/gdb_query.h"

/**
 * @brief Extract arguments from the input packet of a GDB session.
 *
 * @param p_string A pointer to the GDB command string.
 * @param p_argc A pointer to the argument count value.
 * @param pp_argv A pointer to the array of argument string pointers.
 * @return enum gdb_result The GDB command result code.
 */
enum gdb_result gdb_get_args(char* p_string, size_t* p_argc, const char** pp_argv) {
    ASSERT_PTR_NOT_NULL(p_string);

    const char DELIMITERS[] = " \t";

    *p_argc = 0u;

    char*       p_tokenizer_state = NULL;
    const char* p_token           = strtok_r(p_string, DELIMITERS, &p_tokenizer_state);

    while (p_token != NULL) {
        if ((*p_argc) >= GDB_MAX_ARG_COUNT) {
            // Abort, as there are too many arguments.
            return GDB_RESULT_EXCESS_ARGUMENTS;
        }

        pp_argv[*p_argc] = p_token;
        (*p_argc)++;

        p_token = strtok_r(NULL, DELIMITERS, &p_tokenizer_state);
    }

    return GDB_RESULT_OK;
}

/**
 * @brief
 * @note The function does not take arguments from the input packet.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 */
static void gdb_extended_remote(struct gdb_session* p_gdb_session) {
    p_gdb_session->properties.b_is_extended_remote = true;

    gdb_reply(p_gdb_session, GDB_REPLY_OK);
    gdb_session_write();
}

/**
 * @brief
 * @note The function does not take arguments from the input packet.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 */
static void gdb_stop_reason_query(struct gdb_session* p_gdb_session) {
    if (p_gdb_session->properties.b_non_stop && p_gdb_session->properties.b_target_is_running) {
        gdb_reply(p_gdb_session, GDB_REPLY_OK);
        gdb_session_write();
        return;
    }

    gdb_packet_write(&p_gdb_session->output_packet, "W00");
    gdb_session_write();
}

static void gdb_resume_addr(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_resume_signal(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_detach(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

/**
 * @brief
 * @note The function does not take arguments from the input packet.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 */
static void gdb_get_registers(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_OK);
    gdb_session_write();
}

static void gdb_set_registers(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_set_thread(struct gdb_session* p_gdb_session) {
    // FIXME: Extract thread number, if applicable.

    gdb_reply(p_gdb_session, GDB_REPLY_OK);
    gdb_session_write();
}

/**
 * @brief
 * @note The function does not take arguments from the input packet.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 */
static void gdb_kill(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_OK);
    gdb_session_write();
}

static void gdb_get_memory(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_write_memory_hex(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_read_register(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_write_register(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_set(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_restart(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_step(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_step_signal(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_is_thread_alive(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_v(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

static void gdb_write_memory(struct gdb_session* p_gdb_session) {
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
    gdb_session_write();
}

/**
 * @brief Insert or remove a target breakpoint.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 */
static void gdb_breakpoint(struct gdb_session* p_gdb_session) {
    char*  p_buffer = gdb_packet_get_buffer(&p_gdb_session->output_packet);
    size_t size     = gdb_packet_get_length(&p_gdb_session->output_packet);

    uint32_t kind    = 0u;
    uint32_t address = 0u;
    uint32_t length  = 0u;
    SNSCANF(p_buffer, size, "%*[zZ]%" PRIu32 ",%08" PRIx32 ",%" PRIu32, &kind, &address, &length);

    // FIXME: set or clear breakpoint on the target.
    (void)kind;
    (void)address;
    (void)length;

    // Error when inserting/removing breakpoint.
    gdb_reply(p_gdb_session, GDB_REPLY_ERROR_01);

    // Breakpoint insertion/removal was successful.
    gdb_reply(p_gdb_session, GDB_REPLY_OK);

    // Unsupported breakpoint kind.
    gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
}

/**
 * @brief A mapping of GDB commands to handler functions.
 */
const struct gdb_command G_COMMANDS[] = {
    {'!', gdb_extended_remote},    // Enable extended remote mode.
    {'?', gdb_stop_reason_query},  // Stop reason query.
    {'c', gdb_resume_addr},        // Continue (at addr)
    {'C', gdb_resume_signal},      // Continue with signal.
    {'D', gdb_detach},             // Detach.
    {'g', gdb_get_registers},      // Read general registers.
    {'G', gdb_set_registers},      // Write general registers.
    {'H', gdb_set_thread},         // Set thread for subsequent operations.
    {'k', gdb_kill},               // Kill.
    {'m', gdb_get_memory},         // Read memory.
    {'M', gdb_write_memory_hex},   // Write memory (hex).
    {'p', gdb_read_register},      // Read register.
    {'P', gdb_write_register},     // Write register.
    {'q', gdb_query},              // General query
    {'Q', gdb_set},                // General set.
    {'R', gdb_restart},            // Extended remote restart command.
    {'s', gdb_step},               // Single step.
    {'S', gdb_step_signal},        // Step with signal.
    {'T', gdb_is_thread_alive},    // Thread liveliness query.
    {'v', gdb_v},                  // Group of 'v' commands.
    {'X', gdb_write_memory},       // Write memory (binary).
    {'z', gdb_breakpoint},         // Insert breakpoint/watchpoint.
    {'Z', gdb_breakpoint},         // Remove breakpoint/watchpoint.
};

/**
 * @brief Execute a command that is pending in a GDB session.
 *
 * @param p_gdb_session A pointer to the GDB session structure.
 */
void gdb_execute(struct gdb_session* p_gdb_session) {
    ASSERT_PTR_NOT_NULL(p_gdb_session);

    ASSERT_VERBOSE(p_gdb_session->input_packet.state == GDB_PACKET_STATE_COMPLETE,
                   "Cannot handle incomplete input packet.");
    ASSERT_VERBOSE(p_gdb_session->input_packet.type == GDB_PACKET_TYPE_INBOUND, "Input packet not marked as inbound.");

    char command = gdb_packet_get_command(&p_gdb_session->input_packet);

    const struct gdb_command* p_gdb_command = NULL;
    for (size_t command_index = 0; command_index < ARRAY_LENGTH(G_COMMANDS); command_index++) {
        if (command == G_COMMANDS[command_index].command) {
            p_gdb_command = &G_COMMANDS[command_index];
            break;
        }
    }

    if (p_gdb_command == NULL) {
        gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
        gdb_session_write();
        return;
    }

    ASSERT_PTR_NOT_NULL(p_gdb_command->p_callback);
    p_gdb_command->p_callback(p_gdb_session);
}

/**
 * @brief Execute a subcommand that is given by a string.
 *
 * @param p_command_string A pointer to the command string to parse.
 * @param p_gdb_session A pointer to the GDB session structure.
 * @param p_gdb_subcommands A pointer to the table of subcommands to use for execution.
 * @param p_gdb_subcommands_length The number of available subcommands to match against.
 */
void gdb_execute_sub(const char* p_command_string, struct gdb_session* p_gdb_session,
                     const struct gdb_subcommand* p_gdb_subcommands, const size_t gdb_subcommands_length,
                     const size_t argc, const char* p_argv) {
    ASSERT_PTR_NOT_NULL(p_command_string);
    ASSERT_PTR_NOT_NULL(p_gdb_session);
    ASSERT_PTR_NOT_NULL(p_gdb_subcommands);

    ASSERT_VERBOSE(*p_command_string != GDB_PACKET_CHAR_START,
                   "GDB packet start delimiter is invalid in commands names.");

    const struct gdb_subcommand* p_matched_gdb_subcommand = NULL;

    for (size_t command_index = 0; command_index < gdb_subcommands_length; command_index++) {
        const struct gdb_subcommand* p_gdb_subcommand = &p_gdb_subcommands[command_index];
        const char*                  p_subcommand     = p_gdb_subcommand->p_subcommand;
        const size_t                 subcommand_size  = strlen(p_subcommand);

        if (0 == strncmp(p_command_string, p_subcommand, subcommand_size)) {
            p_matched_gdb_subcommand = p_gdb_subcommand;
            break;
        }
    }

    if (p_matched_gdb_subcommand == NULL) {
        // An empty reply indicates that the subcommand is not recognized.
        gdb_reply(p_gdb_session, GDB_REPLY_EMPTY);
        gdb_session_write();
        return;
    }

    ASSERT_PTR_NOT_NULL(p_matched_gdb_subcommand);
    ASSERT_PTR_NOT_NULL(p_matched_gdb_subcommand->p_callback);
    p_matched_gdb_subcommand->p_callback(p_gdb_session, argc, p_argv);
}

/**
 * @}
 *
 */
