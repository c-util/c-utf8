/*
 * UTF-8 Implementation
 *
 * For highlevel documentation of the API see the header file and the docbook
 * comments. This implementation is inspired in part by Rust's std::str.
 *
 * So far only validation helpers are implemented, as those seem the most
 * critical.
 */

#include <stddef.h>
#include <stdint.h>
#include "c-utf8.h"

#define _public_ __attribute__((__visibility__("default")))
#define _unlikely_(_x) (__builtin_expect(!!(_x), 0))
#define ALIGN_TO(_val, _alignment) ((_val + (_alignment) - 1) & ~((_alignment) - 1))

/* The following constants are truncated on 32-bit machines */
#define C_UTF8_ASCII_MASK ((size_t)UINT64_C(0x8080808080808080))
#define C_UTF8_ASCII_SUB ((size_t)UINT64_C(0x0101010101010101))

/**
 * c_utf8_verify_ascii() - verify that a string is ASCII encoded
 * @strp:               pointer to string to verify
 * @lenp:               pointer to length of string
 *
 * Up to the first @lenp bytes of the string pointed to by @strp is
 * verified to be ASCII encoded, and @strp and @lenp are updated to
 * point to the first non-ASCII character or the first NULL of the
 * string, and the remaining number of bytes of the string,
 * respectively.
 */
_public_ void c_utf8_verify_ascii(const char **strp, size_t *lenp) {
        unsigned char *str = (unsigned char *)*strp;
        size_t len = *lenp;

        while (len > 0 && *str < 128) {
                if ((void*)ALIGN_TO((unsigned long)str, sizeof(size_t)) == str) {
                        /*
                         * If the string is aligned to a word boundary, scan one word
                         * at a time for any NULL or non-ASCII characters.
                         */
                        while (len >= sizeof(size_t)) {
                                /* break if any byte is NULL or has the MSB set */
                                if ((((*(size_t*)str - C_UTF8_ASCII_SUB) | *(size_t*)str) & C_UTF8_ASCII_MASK) != 0)
                                        break;

                                str += sizeof(size_t);
                                len -= sizeof(size_t);
                        }


                        /*
                         * Find the actual end of the ASCII-portion of the string.
                         */
                        while (len > 0 && *str < 128) {
                                if (_unlikely_(*str == 0x00))
                                        goto out;
                                ++str;
                                --len;
                        }
                } else {
                        /*
                         * The string was not aligned, scan one character at a time until
                         * it is.
                         */
                        if (_unlikely_(*str == 0x00))
                                goto out;
                        ++str;
                        --len;
                }
        }

out:
        *strp = (char *)str;
        *lenp = len;
}

#define C_UTF8_CHAR_IS_TAIL(_x)         (((_x) & 0xC0) == 0x80)

/**
 * c_utf8_verify() - verify that a string is UTF-8 encoded
 * @strp:               pointer to string to verify
 * @lenp:               pointer to length of string
 *
 * Up to the first @lenp bytes of the string pointed to by @strp is
 * verified to be UTF-8 encoded, and @strp and @lenp are updated to
 * point to the first non-UTF-8 character or the first NULL of the
 * string, and the remaining number of bytes of the string,
 * respectively.
 */
_public_ void c_utf8_verify(const char **strp, size_t *lenp) {
        unsigned char *str = (unsigned char *)*strp;
        size_t len = *lenp;

        /* See Unicode 10.0.0, Chapter 3, Section D92 */

        while (len > 0) {
                switch (*str) {
                case 0x00:
                        goto out;
                case 0x01 ... 0x7F:
                        /*
                         * Special-case and optimize the ASCII case.
                         */
                        c_utf8_verify_ascii((const char **)&str, &len);

                        break;
                case 0xC2 ... 0xDF:
                        if (_unlikely_(len < 2))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 1))))
                                goto out;

                        str += 2;
                        len -= 2;

                        break;
                case 0xE0:
                        if (_unlikely_(len < 3))
                                goto out;
                        if (_unlikely_(*(str + 1) < 0xA0 || *(str + 1) > 0xBF))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 2))))
                                goto out;

                        str += 3;
                        len -= 3;

                        break;
                case 0xE1 ... 0xEC:
                        if (_unlikely_(len < 3))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 1))))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 2))))
                                goto out;

                        str += 3;
                        len -= 3;

                        break;
                case 0xED:
                        if (_unlikely_(len < 3))
                                goto out;
                        if (_unlikely_(*(str + 1) < 0x80 || *(str + 1) > 0x9F))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 2))))
                                goto out;

                        str += 3;
                        len -= 3;

                        break;
                case 0xEE ... 0xEF:
                        if (_unlikely_(len < 3))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 1))))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 2))))
                                goto out;

                        str += 3;
                        len -= 3;

                        break;
                case 0xF0:
                        if (_unlikely_(len < 4))
                                goto out;
                        if (_unlikely_(*(str + 1) < 0x90 || *(str + 1) > 0xBF))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 2))))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 3))))
                                goto out;

                        str += 4;
                        len -= 4;

                        break;
                case 0xF1 ... 0xF3:
                        if (_unlikely_(len < 4))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 1))))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 2))))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 3))))
                                goto out;

                        str += 4;
                        len -= 4;

                        break;
                case 0xF4:
                        if (_unlikely_(len < 4))
                                goto out;
                        if (_unlikely_(*(str + 1) < 0x80 || *(str + 1) > 0x8F))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 2))))
                                goto out;
                        if (_unlikely_(!C_UTF8_CHAR_IS_TAIL(*(str + 3))))
                                goto out;

                        str += 4;
                        len -= 4;

                        break;
                default:
                        goto out;
                }
        }

out:
        *strp = (char *)str;
        *lenp = len;
}
