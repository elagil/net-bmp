// Copyright 2023 Giovanni Di Sirio

/**
 * @file
 * @brief   The USB configuration module headers.
 *
 * @addtogroup usb_cfg
 * @{
 */

#ifndef SOURCE_USB_CDC_USBCFG_H_
#define SOURCE_USB_CDC_USBCFG_H_

#include "ch.h"
#include "hal.h"

void                  usbcfg_init(void);
BaseSequentialStream *get_serial_usb_stream(void);

#endif  // SOURCE_USB_CDC_USBCFG_H_

/**
 * @}
 */
