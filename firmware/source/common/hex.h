#ifndef COMMON_HEX_
#define COMMON_HEX_

#include <inttypes.h>
#include <stddef.h>

char hex_nibble_from_uint(uint8_t value);
void hex_from_uint(const uint8_t *p_values, const size_t values_length, char *p_hex, const size_t hex_length);

uint8_t uint_from_hex_nibble(const char hex);
void    uint_from_hex(const char hex[], size_t hex_length, uint8_t *p_values, size_t values_length);

#endif  // COMMON_HEX_
