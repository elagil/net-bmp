// Copyright 2023 elagil

/**
 * @file
 * @brief   Conversion functions from string to hex, and vice-versa.
 *
 * @addtogroup common
 * @{
 */

#ifndef SOURCE_COMMON_HEX_H_
#define SOURCE_COMMON_HEX_H_

#include <inttypes.h>
#include <stddef.h>

char hex_nibble_from_char(char character);
void hex_from_str(const char *p_string, const size_t string_length, uint8_t *p_hex, const size_t hex_length);

char char_from_hex_nibble(const uint8_t hex);
void str_from_hex(const uint8_t *p_hex, size_t hex_length, char *p_string, size_t string_length);

#endif  // SOURCE_COMMON_HEX_H_

/**
 * @}
 */
