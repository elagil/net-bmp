#ifndef COMMON_H_
#define COMMON_H_

#include "ch.h"
#include "hal.h"

/**
 * @brief Return the \a _val if \a _op matches it.
 * @param _op The operation to execute, or the left hand value.
 * @param _val The comparison, or right hand value.
 * @returns \a _val or nothing, in case of mismatch.
 */
#define RETURN_IF(_op, _val)                                                                                           \
    do {                                                                                                               \
        if ((_op) == (_val)) {                                                                                         \
            return (_val);                                                                                             \
        }                                                                                                              \
    } while (false)

/**
 * @brief Return the \a _val if \a _op does not match it.
 * @param _op The operation to execute, or the left hand value.
 * @param _val The comparison, or right hand value.
 * @returns \a _val or nothing, in case of match.
 */
#define RETURN_IF_NOT(_op, _val)                                                                                       \
    do {                                                                                                               \
        if ((_op) != (_val)) {                                                                                         \
            return (_val);                                                                                             \
        }                                                                                                              \
    } while (false)

#define GET_LOWER_NIBBLE(_val) ((_val) & 0xF)
#define GET_UPPER_NIBBLE(_val) (((_val) >> 4u) & 0xF)

#define ASSERT_VERBOSE(_cond, _msg) chDbgAssert((_cond), (_msg))
#define ASSERT_PTR_NOT_NULL(_ptr)   ASSERT_VERBOSE((_ptr) != NULL, "Null pointer.")
#define ASSERT_NOT_REACHED()        ASSERT_VERBOSE(false, "Should not be reached.")

#define ARRAY_LENGTH(_a) (sizeof((_a)) / sizeof((_a)[0]))

// Requires "hal.h".
#include "chprintf.h"

#define SNPRINTF chsnprintf

#endif  // COMMON_H_
