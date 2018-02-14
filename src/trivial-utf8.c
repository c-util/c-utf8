/* SPDX-License-Identifier: LGPL-2.1+ */
/***
  This file is part of systemd.

  Copyright 2008-2011 Kay Sievers
  Copyright 2012 Lennart Poettering

  systemd is free software; you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation; either version 2.1 of the License, or
  (at your option) any later version.

  systemd is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with systemd; If not, see <http://www.gnu.org/licenses/>.
***/

/* Parts of this file are based on the GLIB utf8 validation functions. The
 * original license text follows. */

/* gutf8.c - Operations on UTF-8 strings.
 *
 * Copyright (C) 1999 Tom Tromey
 * Copyright (C) 2000 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <uchar.h>

#include <stdio.h>

#include "trivial-utf8.h"

/* count of characters used to encode one unicode char */
static int utf8_encoded_expected_len(const char *str) {
        unsigned char c;

        c = (unsigned char) str[0];
        if (c < 0x80)
                return 1;
        if ((c & 0xe0) == 0xc0)
                return 2;
        if ((c & 0xf0) == 0xe0)
                return 3;
        if ((c & 0xf8) == 0xf0)
                return 4;
        if ((c & 0xfc) == 0xf8)
                return 5;
        if ((c & 0xfe) == 0xfc)
                return 6;

        return 0;
}

/* decode one unicode char */
static int utf8_encoded_to_unichar(const char *str, char32_t *ret_unichar) {
        char32_t unichar;
        int len, i;

        len = utf8_encoded_expected_len(str);

        switch (len) {
        case 1:
                *ret_unichar = (char32_t)str[0];
                return 0;
        case 2:
                unichar = str[0] & 0x1f;
                break;
        case 3:
                unichar = (char32_t)str[0] & 0x0f;
                break;
        case 4:
                unichar = (char32_t)str[0] & 0x07;
                break;
        case 5:
                unichar = (char32_t)str[0] & 0x03;
                break;
        case 6:
                unichar = (char32_t)str[0] & 0x01;
                break;
        default:
                return -EINVAL;
        }

        for (i = 1; i < len; i++) {
                if (((char32_t)str[i] & 0xc0) != 0x80)
                        return -EINVAL;
                unichar <<= 6;
                unichar |= (char32_t)str[i] & 0x3f;
        }

        *ret_unichar = unichar;

        return 0;
}

/* expected size used to encode one unicode char */
static int utf8_unichar_to_encoded_len(char32_t unichar) {

        if (unichar < 0x80)
                return 1;
        if (unichar < 0x800)
                return 2;
        if (unichar < 0x10000)
                return 3;
        if (unichar < 0x200000)
                return 4;
        if (unichar < 0x4000000)
                return 5;

        return 6;
}

static bool unichar_is_valid(char32_t ch) {

        if (ch >= 0x110000) /* End of unicode space */
                return false;
        if ((ch & 0xFFFFF800) == 0xD800) /* Reserved area for UTF-16 */
                return false;

        return true;
}

/* validate one encoded unicode char and return its length */
static int utf8_encoded_valid_unichar(const char *str) {
        int len, i, r;
        char32_t unichar;

        len = utf8_encoded_expected_len(str);
        if (len == 0)
                return -EINVAL;

        /* ascii is valid */
        if (len == 1)
                return 1;

        /* check if expected encoded chars are available */
        for (i = 0; i < len; i++)
                if ((str[i] & 0x80) != 0x80)
                        return -EINVAL;

        r = utf8_encoded_to_unichar(str, &unichar);
        if (r < 0)
                return r;

        /* check if encoded length matches encoded value */
        if (utf8_unichar_to_encoded_len(unichar) != len)
                return -EINVAL;

        /* check if value has valid range */
        if (!unichar_is_valid(unichar))
                return -EINVAL;

        return len;
}

const char *trivial_utf8_is_valid(const char *str) {
        const uint8_t *p;

        for (p = (const uint8_t*) str; *p; ) {
                int len;

                len = utf8_encoded_valid_unichar((const char *)p);
                if (len < 0) {
                        fprintf(stderr, "failed after %zd characters\n", (void*)p - (void*)str);
                        return NULL;
                }

                p += len;
        }

        return str;
}

const char *trivial_ascii_is_valid(const char *str) {
        const char *p;

        for (p = str; *p; p++)
                if ((unsigned char) *p >= 128)
                        return NULL;

        return (char*) str;
}
