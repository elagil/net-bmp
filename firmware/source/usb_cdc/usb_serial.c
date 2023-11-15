/* This file implements a the USB Communications Device Class - Abstract
 * Control Model (CDC-ACM) as defined in CDC PSTN subclass 1.2.
 * A Device Firmware Upgrade (DFU 1.1) class interface is provided for
 * field firmware upgrade.
 *
 * The device's unique id is used as the USB serial number string.
 *
 * Endpoint Usage
 *
 *     0 Control Endpoint
 * IN  1 GDB CDC DATA
 * OUT 1 GDB CDC DATA
 * IN  2 GDB CDC CTR
 * IN  3 UART CDC DATA
 * OUT 3 UART CDC DATA
 * OUT 4 UART CDC CTRL
 * In  5 Trace Capture
 *
 */

#ifndef ENABLE_DEBUG
#include <string.h>
#include <sys/stat.h>

typedef struct stat stat_s;
#endif

#include "gdb_if.h"
#include "general.h"
#include "platform.h"
// #include "usb_serial.h"
#ifdef PLATFORM_HAS_TRACESWO
#include "traceswo.h"
#endif

#include "rtt.h"
#include "rtt_if.h"

static bool gdb_serial_dtr = true;

// static void usb_serial_set_state(usbd_device *dev, uint16_t iface, uint8_t ep);

// static void debug_serial_send_callback(usbd_device *dev, uint8_t ep);
// static void debug_serial_receive_callback(usbd_device *dev, uint8_t ep);

#if defined(ENABLE_DEBUG) && defined(PLATFORM_HAS_DEBUG)
/*
 * This call initialises "SemiHosting", only we then do our own SVC interrupt things to
 * route all output through to the debug USB serial interface if debug_bmp is true.
 *
 * https://github.com/mirror/newlib-cygwin/blob/master/newlib/libc/sys/arm/syscalls.c#L115
 */
void initialise_monitor_handles(void);

static char    debug_serial_debug_buffer[AUX_UART_BUFFER_SIZE];
static uint8_t debug_serial_debug_write_index;
static uint8_t debug_serial_debug_read_index;
#endif

// static usbd_request_return_codes_e gdb_serial_control_request(usbd_device *dev, usb_setup_data_s *req, uint8_t **buf,
//                                                               uint16_t *const len,
//                                                               void            (**complete)(usbd_device      *dev,
//                                                                                 usb_setup_data_s *req)) {
//     (void)buf;
//     (void)complete;
//     /* Is the request for the GDB UART interface? */
//     if (req->wIndex != GDB_IF_NO) return USBD_REQ_NEXT_CALLBACK;

//     switch (req->bRequest) {
//         case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
//             /* Send a notification back on the notification endpoint */
//             usb_serial_set_state(dev, req->wIndex, CDCACM_GDB_ENDPOINT + 1U);
//             gdb_serial_dtr = req->wValue & 1U;
//             return USBD_REQ_HANDLED;
//         case USB_CDC_REQ_SET_LINE_CODING:
//             if (*len < sizeof(usb_cdc_line_coding_s)) return USBD_REQ_NOTSUPP;
//             return USBD_REQ_HANDLED; /* Ignore on GDB Port */
//         case USB_CDC_REQ_GET_LINE_CODING: {
//             if (*len < sizeof(usb_cdc_line_coding_s)) return USBD_REQ_NOTSUPP;
//             usb_cdc_line_coding_s *line_coding = (usb_cdc_line_coding_s *)*buf;
//             /* This tells the host that we talk 1MBaud, 8-bit no parity w/ 1 stop bit */
//             line_coding->dwDTERate   = 1 * 1000 * 1000;
//             line_coding->bCharFormat = USB_CDC_1_STOP_BITS;
//             line_coding->bParityType = USB_CDC_NO_PARITY;
//             line_coding->bDataBits   = 8;
//             return USBD_REQ_HANDLED;
//         }
//     }
//     return USBD_REQ_NOTSUPP;
// }

bool gdb_serial_get_dtr(void) { return gdb_serial_dtr; }

// static usbd_request_return_codes_e debug_serial_control_request(usbd_device *dev, usb_setup_data_s *req, uint8_t
// **buf,
//                                                                 uint16_t *const len,
//                                                                 void            (**complete)(usbd_device      *dev,
//                                                                                   usb_setup_data_s *req)) {
//     (void)complete;
//     /* Is the request for the physical/debug UART interface? */
//     if (req->wIndex != UART_IF_NO) return USBD_REQ_NEXT_CALLBACK;

//     switch (req->bRequest) {
//         case USB_CDC_REQ_SET_CONTROL_LINE_STATE:
//             /* Send a notification back on the notification endpoint */
//             usb_serial_set_state(dev, req->wIndex, CDCACM_UART_ENDPOINT + 1U);
// #ifdef USBUSART_DTR_PIN
//             gpio_set_val(USBUSART_PORT, USBUSART_DTR_PIN, !(req->wValue & 1U));
// #endif
// #ifdef USBUSART_RTS_PIN
//             gpio_set_val(USBUSART_PORT, USBUSART_RTS_PIN, !((req->wValue >> 1U) & 1U));
// #endif
//             return USBD_REQ_HANDLED;
//         case USB_CDC_REQ_SET_LINE_CODING:
//             if (*len < sizeof(usb_cdc_line_coding_s)) return USBD_REQ_NOTSUPP;
//             aux_serial_set_encoding((usb_cdc_line_coding_s *)*buf);
//             return USBD_REQ_HANDLED;
//         case USB_CDC_REQ_GET_LINE_CODING:
//             if (*len < sizeof(usb_cdc_line_coding_s)) return USBD_REQ_NOTSUPP;
//             aux_serial_get_encoding((usb_cdc_line_coding_s *)*buf);
//             return USBD_REQ_HANDLED;
//     }
//     return USBD_REQ_NOTSUPP;
// }

// void usb_serial_set_state(usbd_device *const dev, const uint16_t iface, const uint8_t ep) {
//     uint8_t                 buf[10];
//     usb_cdc_notification_s *notif = (void *)buf;
//     /* We echo signals back to host as notification */
//     notif->bmRequestType = 0xa1;
//     notif->bNotification = USB_CDC_NOTIFY_SERIAL_STATE;
//     notif->wValue        = 0;
//     notif->wIndex        = iface;
//     notif->wLength       = 2;
//     buf[8]               = 3U;
//     buf[9]               = 0U;
//     // usbd_ep_write_packet(dev, ep, buf, sizeof(buf));
// }

// void usb_serial_set_config(usbd_device *dev, uint16_t value) {
//     usb_config = value;

//     /* GDB interface */
// #if defined(STM32F4) || defined(LM4F) || defined(STM32F7)
//     usbd_ep_setup(dev, CDCACM_GDB_ENDPOINT, USB_ENDPOINT_ATTR_BULK, CDCACM_PACKET_SIZE, gdb_usb_out_cb);
// #else
//     usbd_ep_setup(dev, CDCACM_GDB_ENDPOINT, USB_ENDPOINT_ATTR_BULK, CDCACM_PACKET_SIZE, NULL);
// #endif
//     usbd_ep_setup(dev, CDCACM_GDB_ENDPOINT | USB_REQ_TYPE_IN, USB_ENDPOINT_ATTR_BULK, CDCACM_PACKET_SIZE, NULL);
//     usbd_ep_setup(dev, (CDCACM_GDB_ENDPOINT + 1U) | USB_REQ_TYPE_IN, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

//     /* Serial interface */
//     usbd_ep_setup(dev, CDCACM_UART_ENDPOINT, USB_ENDPOINT_ATTR_BULK, CDCACM_PACKET_SIZE / 2U,
//                   debug_serial_receive_callback);
//     usbd_ep_setup(dev, CDCACM_UART_ENDPOINT | USB_REQ_TYPE_IN, USB_ENDPOINT_ATTR_BULK, CDCACM_PACKET_SIZE,
//                   debug_serial_send_callback);
//     usbd_ep_setup(dev, (CDCACM_UART_ENDPOINT + 1U) | USB_REQ_TYPE_IN, USB_ENDPOINT_ATTR_INTERRUPT, 16, NULL);

// #ifdef PLATFORM_HAS_TRACESWO
//     /* Trace interface */
//     usbd_ep_setup(dev, TRACE_ENDPOINT | USB_REQ_TYPE_IN, USB_ENDPOINT_ATTR_BULK, TRACE_ENDPOINT_SIZE,
//     trace_buf_drain);
// #endif

//     usbd_register_control_callback(dev, USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
//                                    USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT, debug_serial_control_request);
//     usbd_register_control_callback(dev, USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
//                                    USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT, gdb_serial_control_request);

//     /* Notify the host that DCD is asserted.
//      * Allows the use of /dev/tty* devices on *BSD/MacOS
//      */
//     usb_serial_set_state(dev, GDB_IF_NO, CDCACM_GDB_ENDPOINT);
//     usb_serial_set_state(dev, UART_IF_NO, CDCACM_UART_ENDPOINT);

// #if defined(ENABLE_DEBUG) && defined(PLATFORM_HAS_DEBUG)
//     initialise_monitor_handles();
// #endif
// }

void debug_serial_send_stdout(const uint8_t *const data, const size_t len) {
    // for (size_t offset = 0; offset < len; offset += CDCACM_PACKET_SIZE) {
    //     const size_t count = MIN(len - offset, CDCACM_PACKET_SIZE);
    //     nvic_disable_irq(USB_IRQ);
    //     /* XXX: Do we actually care if this fails? Possibly not.. */
    //     usbd_ep_write_packet(usbdev, CDCACM_UART_ENDPOINT, data + offset, count);
    //     nvic_enable_irq(USB_IRQ);
    // }
}

uint32_t debug_serial_fifo_send(const char *const fifo, const uint32_t fifo_begin, const uint32_t fifo_end) {
    /*
     * To avoid the need of sending ZLP don't transmit full packet.
     * Also reserve space for copy function overrun.
     */
    // char     packet[CDCACM_PACKET_SIZE - 1U];
    // uint32_t packet_len = 0;
    // for (uint32_t fifo_index = fifo_begin; fifo_index != fifo_end && packet_len < CDCACM_PACKET_SIZE - 1U;
    //      fifo_index %= AUX_UART_BUFFER_SIZE)
    //     packet[packet_len++] = fifo[fifo_index++];

    // if (packet_len) {
    //     const uint16_t written = usbd_ep_write_packet(usbdev, CDCACM_UART_ENDPOINT, packet, packet_len);
    //     return (fifo_begin + written) % AUX_UART_BUFFER_SIZE;
    // }
    // return fifo_begin;
    return 0u;
}

#if defined(ENABLE_DEBUG) && defined(PLATFORM_HAS_DEBUG)
static bool debug_serial_fifo_buffer_empty(void) {
    return debug_serial_debug_write_index == debug_serial_debug_read_index;
}
#endif

#if defined(STM32F0) || defined(STM32F1) || defined(STM32F3) || defined(STM32F4) || defined(STM32F7)
/*
 * Runs deferred processing for AUX serial RX, draining RX FIFO by sending
 * characters to host PC via the debug serial interface.
 */
// static void debug_serial_send_data(void) {
//     debug_serial_send_complete = false;
//     aux_serial_update_receive_buffer_fullness();

//     /* Forcibly empty fifo if no USB endpoint.
//      * If fifo empty, nothing further to do. */
//     if (usb_get_config() != 1 || (aux_serial_receive_buffer_empty()
// #if defined(ENABLE_DEBUG) && defined(PLATFORM_HAS_DEBUG)
//                                   && debug_serial_fifo_buffer_empty()
// #endif
//                                       )) {
// #if defined(ENABLE_DEBUG) && defined(PLATFORM_HAS_DEBUG)
//         debug_serial_debug_read_index = debug_serial_debug_write_index;
// #endif
//         aux_serial_drain_receive_buffer();
//         debug_serial_send_complete = true;
//     } else {
// #if defined(ENABLE_DEBUG) && defined(PLATFORM_HAS_DEBUG)
//         debug_serial_debug_read_index = debug_serial_fifo_send(debug_serial_debug_buffer,
//         debug_serial_debug_read_index,
//                                                                debug_serial_debug_write_index);
// #endif
//         aux_serial_stage_receive_buffer();
//     }
// }

// void debug_serial_run(void) {
//     nvic_disable_irq(USB_IRQ);
//     aux_serial_set_led(AUX_SERIAL_LED_RX);

//     /* Try to send a packet if usb is idle */
//     if (debug_serial_send_complete) debug_serial_send_data();

//     nvic_enable_irq(USB_IRQ);
// }
#endif

// static void debug_serial_send_callback(usbd_device *dev, uint8_t ep) {
//     (void)ep;
//     (void)dev;
// #if defined(STM32F0) || defined(STM32F1) || defined(STM32F3) || defined(STM32F4) || defined(STM32F7)
//     debug_serial_send_data();
// #endif
// }

// static void debug_serial_receive_callback(usbd_device *dev, uint8_t ep) {
// #ifdef ENABLE_RTT
//     if (rtt_enabled) {
//         rtt_serial_receive_callback(dev, ep);
//         return;
//     }
// #endif

//     char *const    transmit_buffer = aux_serial_current_transmit_buffer() + aux_serial_transmit_buffer_fullness();
//     const uint16_t len             = usbd_ep_read_packet(dev, ep, transmit_buffer, CDCACM_PACKET_SIZE);

// #if defined(BLACKMAGIC)
//     /* Don't bother if uart is disabled.
//      * This will be the case on mini while we're being debugged.
//      */
//     if (!(RCC_APB2ENR & RCC_APB2ENR_USART1EN) && !(RCC_APB1ENR & RCC_APB1ENR_USART2EN)) return;
// #endif

//     aux_serial_send(len);

// #if defined(STM32F0) || defined(STM32F1) || defined(STM32F3) || defined(STM32F4) || defined(STM32F7)
//     /* Disable USBUART TX packet reception if buffer does not have enough space */
//     if (AUX_UART_BUFFER_SIZE - aux_serial_transmit_buffer_fullness() < CDCACM_PACKET_SIZE) usbd_ep_nak_set(dev, ep,
//     1);
// #endif
// }

// #ifdef ENABLE_DEBUG &&PLATFORM_HAS_DEBUG
// static void           debug_serial_append_char(const char c) {
//     debug_serial_debug_buffer[debug_serial_debug_write_index] = c;
//     ++debug_serial_debug_write_index;
//     debug_serial_debug_write_index %= AUX_UART_BUFFER_SIZE;
// }

// static size_t debug_serial_debug_write(const char *buf, const size_t len) {
//     if (nvic_get_active_irq(USB_IRQ) || nvic_get_active_irq(USBUSART_IRQ) ||
//     nvic_get_active_irq(USBUSART_DMA_RX_IRQ))
//         return 0;

//     CM_ATOMIC_CONTEXT();
//     size_t offset = 0;

//     for (;
//          offset < len && (debug_serial_debug_write_index + 1U) % AUX_UART_BUFFER_SIZE !=
//          debug_serial_debug_read_index;
//          ++offset) {
//         if (buf[offset] == '\n') {
//             debug_serial_append_char('\r');

//             if ((debug_serial_debug_write_index + 1U) % AUX_UART_BUFFER_SIZE == debug_serial_debug_read_index) break;
//         }
//         debug_serial_append_char(buf[offset]);
//     }

//     debug_serial_run();
//     return offset;
// }
// #endif
