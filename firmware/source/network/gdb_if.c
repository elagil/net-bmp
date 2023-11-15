// Copyright 2023 elagil

/**
 * @file
 * @brief   The gdb interface module.
 * @details This file implements a transparent channel over which the GDB Remote Serial Debugging protocol is
 * implemented. This implementation uses a network connection to implement the channel.
 *
 * @addtogroup gdb
 * @{
 */

#include "network.h"

/**
 * @brief An empty packet.
 */
#define GDB_IF_EMPTY "\0"

void gdb_if_putchar(const char c, const int flush) {
    (void)flush;
    network_putchar(c);

#if 0
    buffer_in[count_in++] = c;
    if (flush || count_in == CDCACM_PACKET_SIZE) {
        /* Refuse to send if USB isn't configured, and
         * don't bother if nobody's listening */
        if (usb_get_config() != 1 || !gdb_serial_get_dtr()) {
            count_in = 0;
            return;
        }
        while (usbd_ep_write_packet(usbdev, CDCACM_GDB_ENDPOINT, buffer_in, count_in) <= 0) continue;

        if (flush && count_in == CDCACM_PACKET_SIZE) {
            /* We need to send an empty packet for some hosts
             * to accept this as a complete transfer. */
            /* libopencm3 needs a change for us to confirm when
             * that transfer is complete, so we just send a packet
             * containing a null byte for now.
             */
            while (usbd_ep_write_packet(usbdev, CDCACM_GDB_ENDPOINT, "\0", 1) <= 0) continue;
        }

        count_in = 0;
    }
#endif
}

char gdb_if_getchar(void) {
    return network_getchar_timeout(NETWORK_TIMEOUT_INFINITE);

#if 0
    while (out_ptr >= count_out) {
        /*
         * Detach if port closed
         *
         * The WFI here is safe because any interrupt, including the regular SysTick
         * will cause the processor to resume from the WFI instruction.
         */
        if (!gdb_serial_get_dtr()) {
            __WFI();
            return '\x04';
        }

        gdb_if_update_buf();
    }

    return buffer_out[out_ptr++];
#endif
}

char gdb_if_getchar_to(const uint32_t timeout) {
    if (timeout == 0u) {
        return network_getchar_immediate();
    }

    return network_getchar_timeout(timeout);

    // platform_timeout_s receive_timeout;
    // platform_timeout_set(&receive_timeout, timeout);

    // /* Wait while we need more data or until the timeout expires */
    // while (out_ptr >= count_out && !platform_timeout_is_expired(&receive_timeout)) {
    //     /*
    //      * Detach if port closed
    //      *
    //      * The WFI here is safe because any interrupt, including the regular SysTick
    //      * will cause the processor to resume from the WFI instruction.
    //      */
    //     if (!gdb_serial_get_dtr()) {
    //         __WFI();
    //         return '\x04';
    //     }
    //     gdb_if_update_buf();
    // }

    // if (out_ptr < count_out) return buffer_out[out_ptr++];
    // /* XXX: Need to find a better way to error return than this. This provides '\xff' characters. */
    // return -1;
}

/**
 * @}
 *
 */
