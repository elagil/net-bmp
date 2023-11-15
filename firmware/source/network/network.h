// Copyright 2023 elagil

/**
 * @file
 * @brief   The network module headers.
 *
 * @addtogroup network
 * @{
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "ch.h"

#define NETWORK_TIMEOUT_INFINITE 0u
#define NETWORK_FIRST_TCP_PORT   2000

void network_putchar(const char c);
char network_getchar_timeout(const uint32_t timeout_ms);
char network_getchar_immediate(void);

void network_init(void);

#endif  // NETWORK_H_

/**
 * @}
 */
