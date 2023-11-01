// Copyright 2023 elagil

/**
 * @file
 * @brief   The main module.
 * @details Contains the main application thread, and from there sets up other worker threads.
 *
 * @addtogroup main
 * @{
 */

#include "ch.h"
#include "hal.h"

/**
 * @brief Application entry point.
 *
 * @return int The return code.
 */
int main(void) {
    halInit();
    chSysInit();

    while (true) {
        chThdSleepMilliseconds(1000);
    }
}

/**
 * @}
 */
