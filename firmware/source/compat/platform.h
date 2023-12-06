// Copyright 2023 elagil

/**
 * @file
 * @brief   Compatibility layer headers for platform commands, required by the blackmagic firmware project.
 *
 * @addtogroup compat
 * @{
 */

#ifndef SOURCE_COMPAT_PLATFORM_H_
#define SOURCE_COMPAT_PLATFORM_H_

void gdb_out(const char *buf);
void gdb_voutf(const char *fmt, va_list);
void gdb_outf(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif  // SOURCE_COMPAT_PLATFORM_H_
