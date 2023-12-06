// Copyright 2023 elagil

/**
 * @file
 * @brief   The main module.
 * @details Contains the main application thread, and from there sets up other worker threads.
 *
 * @addtogroup main
 * @{
 */

#include <stdio.h>

#include "ch.h"
#include "gdb/gdb_session.h"
#include "hal.h"
#include "network/network.h"
#include "usb_cdc/shell_interface.h"

/**
 * @brief Application entry point.
 *
 * @return int The return code.
 */
int main(void) {
    halInit();
    chSysInit();

    palClearLine(LINE_LED_BLUE);
    palClearLine(LINE_LED_AMBER);
    palClearLine(LINE_LED_RED);
    palClearLine(LINE_LED_GREEN);

    gdb_session_init();
    network_init();
    shell_interface_start_thread();

    while (true) {
        palToggleLine(LINE_LED_RED);
        chThdSleepMilliseconds(1000);
    }
}

/**
 * @}
 */
