// Copyright 2023 elagil

/**
 * @file
 * @brief   Conversion functions from string to hex, and vice-versa.
 *
 * @addtogroup common
 * @{
 */

#include "hex.h"

#include "common.h"

/**
 * @brief Convert a character to a hex nibble.
 *
 * @param character The unsigned integer character.
 * @return The hex nibble.
 */
char hex_nibble_from_char(const char character) {
    ASSERT_VERBOSE(character <= 15u, "Invalid character.");

    char nibble = character;

    if (character > 9u) {
        nibble += 'A' - '0' - 10u;
    }

    return nibble + '0';
}

/**
 * @brief Convert characters to a hex string. They do not have to be NUL-terminated.
 *
 * @param p_string A pointer to the characters.
 * @param string_length The number of characters in the string.
 * @param p_hex A pointer to the hex characters.
 * @param hex_length The length of the character array.
 */
void hex_from_str(const char *p_string, const size_t string_length, uint8_t *p_hex, const size_t hex_length) {
    ASSERT_PTR_NOT_NULL(p_hex);
    ASSERT_PTR_NOT_NULL(p_string);

    ASSERT_VERBOSE((string_length * 2u) == hex_length, "Buffer length mismatch.");
    ASSERT_VERBOSE(hex_length > 0u, "Invalid input length.");

    for (size_t value_index = 0; value_index < string_length; value_index++) {
        p_hex[value_index * 2u]      = hex_nibble_from_char(GET_UPPER_NIBBLE(p_string[value_index]));
        p_hex[value_index * 2u + 1u] = hex_nibble_from_char(GET_LOWER_NIBBLE(p_string[value_index]));
    }
}

/**
 * @brief Convert a single hex nibble to a character.
 *
 * @param hex The hex nibble.
 * @return char The character.
 */
char char_from_hex_nibble(const uint8_t hex) {
    char nibble = hex - '0';

    if (nibble > 9u) {
        nibble -= 'A' - '0' - 10u;
    }

    if (nibble > 16u) {
        nibble -= 'a' - 'A';
    }

    ASSERT_VERBOSE(nibble <= 15u, "Invalid hex nibble.");

    return nibble;
}

/**
 * @brief Convert a hex string to integer values. Write a terminating NUL-character to the output string.
 *
 * @param p_hex A pointer to the hex characters.
 * @param hex_length The length of the hex string.
 * @param p_string A pointer to the values to write.
 * @param string_length The length of the values array.
 */
void str_from_hex(const uint8_t *p_hex, const size_t hex_length, char *p_string, const size_t string_length) {
    ASSERT_PTR_NOT_NULL(p_hex);
    ASSERT_PTR_NOT_NULL(p_string);

    ASSERT_VERBOSE(string_length >= (hex_length / 2u + 1u), "Target buffer length exceeded.");
    ASSERT_VERBOSE(hex_length > 0u, "Invalid input length.");

    for (size_t value_index = 0; value_index < (hex_length / 2u); value_index++) {
        p_string[value_index] = (char_from_hex_nibble(p_hex[2u * value_index]) << 4u) |
                                (char_from_hex_nibble(p_hex[2u * value_index + 1u]));
    }

    p_string[hex_length / 2u] = '\0';
}

/**
 * @}
 */
