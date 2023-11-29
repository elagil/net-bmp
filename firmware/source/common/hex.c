#include "hex.h"

#include "common.h"

/**
 * @brief Convert an unsigned integer value to a hex nibble.
 *
 * @param value The unsigned integer value.
 * @return The hex nibble.
 */
char hex_nibble_from_uint(uint8_t value) {
    ASSERT_VERBOSE(value <= 15u, "Invalid value.");

    char nibble = (char)value;

    if (value > 9u) {
        nibble += 'A' - '0' - 10u;
    }

    return nibble + '0';
}

/**
 * @brief Convert unsigned integer values to a hex string.
 *
 * @param p_values A pointer to the integers.
 * @param values_length The number of integer values.
 * @param p_hex A pointer to the hex characters.
 * @param hex_length The length of the character array.
 */
void hex_from_uint(const uint8_t *p_values, const size_t values_length, char *p_hex, const size_t hex_length) {
    ASSERT_PTR_NOT_NULL(p_hex);
    ASSERT_PTR_NOT_NULL(p_values);

    ASSERT_VERBOSE((values_length * 2u) == hex_length, "Buffer length mismatch.");
    ASSERT_VERBOSE(hex_length > 0u, "Invalid input length.");

    for (size_t value_index = 0; value_index < values_length; value_index++) {
        p_hex[value_index * 2u]      = hex_nibble_from_uint(GET_UPPER_NIBBLE(p_values[value_index]));
        p_hex[value_index * 2u + 1u] = hex_nibble_from_uint(GET_LOWER_NIBBLE(p_values[value_index]));
    }
}

/**
 * @brief Convert a single hex nibble to an unsigned integer value.
 *
 * @param hex The hex nibble.
 * @return uint8_t The unsigned integer value.
 */
uint8_t uint_from_hex_nibble(const char hex) {
    uint8_t nibble = hex - '0';

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
 * @brief Convert a hex string to integer values.
 *
 * @param p_hex A pointer to the hex characters.
 * @param hex_length The length of the hex string.
 * @param p_values A pointer to the values to write.
 * @param values_length The length of the values array.
 */
void uint_from_hex(const char *p_hex, const size_t hex_length, uint8_t *p_values, const size_t values_length) {
    ASSERT_PTR_NOT_NULL(p_hex);
    ASSERT_PTR_NOT_NULL(p_values);

    ASSERT_VERBOSE((values_length * 2u) == hex_length, "Buffer length mismatch.");
    ASSERT_VERBOSE(hex_length > 0u, "Invalid input length.");

    for (size_t value_index = 0; value_index < values_length; value_index++) {
        p_values[value_index] = (uint_from_hex_nibble(p_hex[2u * value_index]) << 4u) |
                                (uint_from_hex_nibble(p_hex[2u * value_index + 1u]));
    }
}
