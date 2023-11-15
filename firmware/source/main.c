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
#include "network/network.h"
#include "usb_cdc/shell_interface.h"

#define GDB_PACKET_BUFFER_SIZE 1024U

/* This has to be aligned so the remote protocol can re-use it without causing Problems */
static char pbuf[GDB_PACKET_BUFFER_SIZE + 1U] __attribute__((aligned(8)));

char *gdb_packet_buffer(void) { return pbuf; }

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

    network_init();
    shell_interface_start_thread();

    size_t lamp_index = 0u;

    while (true) {
        chThdSleepMilliseconds(500);

        palClearLine(LINE_LED_AMBER);
        palClearLine(LINE_LED_RED);

        lamp_index++;
        if (lamp_index >= 2u) {
            lamp_index = 0u;
        }

        switch (lamp_index) {
            case 0u:
                palSetLine(LINE_LED_AMBER);
                break;

            case 1u:
                palSetLine(LINE_LED_RED);
                break;

            default:
                break;
        }
    }
}

/**
 * @}
 */
