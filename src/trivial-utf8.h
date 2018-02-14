#pragma once

/*
 * Non-optimized utf-8 validation implementation, lifted from systemd, which
 * in turn came from glib (and hence used all over the place).
 *
 * This is only used for the benchmarks, none of this code is shipped.
 */

#include <stddef.h>
#include <stdint.h>

const char *trivial_utf8_is_valid(const char *str);
const char *trivial_ascii_is_valid(const char *str);
