// Copyright 2023 elagil

/**
 * @file
 * @brief   The shell interface module.
 * @details Allows communicating with the grinder via USB-CDC.
 *
 * @addtogroup shell_interface
 * @{
 */

#include "shell_interface.h"

#include <string.h>

#include "chprintf.h"
#include "shell.h"
#include "usbcfg.h"

/**
 * @brief Commands that can be called over the shell.
 */
static const ShellCommand COMMANDS[] = {{NULL, NULL}};

static THD_WORKING_AREA(wa_shell_interface_thread, 1024);
static THD_WORKING_AREA(wa_shell_thread, 4096);

/**
 * @brief The shell interface thread.
 */
static THD_FUNCTION(shell_interface_thread, arg) {
    (void)arg;
    chRegSetThreadName("shell interface");
    shellInit();
    usbcfg_init();

    ShellConfig shell_cfg = {.sc_commands = COMMANDS};
    shell_cfg.sc_channel  = get_serial_usb_stream();

    while (true) {
        // Spawn a new shell when required.
        thread_t *p_shell_thread = chThdCreateStatic(wa_shell_thread, sizeof(wa_shell_thread), NORMALPRIO + 1,
                                                     shellThread, (void *)&shell_cfg);

        chThdWait(p_shell_thread);
        chThdSleepMilliseconds(1000);
    }
}

/**
 * @brief Starts the shell interface thread.
 */
void shell_interface_start_thread(void) {
    chThdCreateStatic(&wa_shell_interface_thread, sizeof(wa_shell_interface_thread), NORMALPRIO, shell_interface_thread,
                      NULL);
}
