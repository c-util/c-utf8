/*
 * UTF-8 multibyte validation benchmark
 */

#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "c-utf8.h"
#include "trivial-utf8.h"

uint64_t test_get_time(void) {
        struct timespec ts;
        int r;

        r = clock_gettime(CLOCK_THREAD_CPUTIME_ID, &ts);
        assert(r >= 0);

        return ts.tv_sec * UINT64_C(1000000000) + ts.tv_nsec;
}

static void test_trivial_utf8(const char *string, size_t n_string, size_t n_bytes) {
        uint64_t ts;
        const char *res;

        ts = test_get_time();
        res = trivial_utf8_is_valid(string);
        fprintf(stderr, "UTF-8 verify string of %zu-byte characters (trivial): %lu MB/s\n", n_bytes, n_string * 1000 * 1000 * 1000 / 1024 / 1024 / (test_get_time() - ts));
        assert(res == string);
}

static void test_utf8(const char *string, size_t n_string, size_t n_bytes) {
        uint64_t ts;
        size_t len = n_string;

        ts = test_get_time();
        c_utf8_verify(&string, &len);
        fprintf(stderr, "UTF-8 verify string of %zu-byte characters: %lu MB/s\n", n_bytes, n_string * 1000 * 1000 * 1000 / 1024 / 1024 / (test_get_time() - ts));
        if (len >= n_bytes) {
                if (n_bytes == 4)
                        fprintf(stderr, "consumed: %zu bytes, next: %#x %#x %#x %#x\n", n_string - len, (uint8_t)string[0], (uint8_t)string[1], (uint8_t)string[2], (uint8_t)string[3]);
        }
        assert(len == 1);
}

static void test_strlen(const char *string, size_t n_string, size_t n_bytes) {
        uint64_t ts;
        size_t len;

        ts = test_get_time();
        len = strnlen(string, n_string);
        fprintf(stderr, "strlen() of string of %zu-byte characters: %lu MB/s\n", n_bytes, n_string * 1000 * 1000 * 1000 / 1024 / 1024 / (test_get_time() - ts));
        assert(len == n_string - 1);
}

#define TEST_STRING_SIZE (500ULL * 1024ULL * 1024ULL)

static void test_multibytes(size_t n_bytes) {
        char *string;

        string = calloc(1, TEST_STRING_SIZE);
        assert(string);

        for (size_t i = 0; i < (TEST_STRING_SIZE - 1) / n_bytes; ++i) {
                switch (n_bytes) {
                case 2: {
                        uint32_t c = (i % (0x800 - 0x80)) + 0x80;

                        string[i * n_bytes] = 0b11000000 | (c >> 6);
                        string[i * n_bytes + 1] = 0b10000000 | (c & 0b111111);

                        break;
                }
                case 3: {
                        uint32_t c = (i % (0x10000 - 0x800)) + 0x800;

                        if (c >= 0xD800 && c <= 0xDFFF)
                                c = 0x800;

                        string[i * n_bytes] = 0b11100000 | (c >> 12);
                        string[i * n_bytes + 1] = 0b10000000 | ((c >> 6) & 0b111111);
                        string[i * n_bytes + 2] = 0b10000000 | (c & 0b111111);

                        break;
                }
                case 4: {
                        uint32_t c = (i % (0x110000 - 0x10000)) + 0x10000;

                        string[i * n_bytes] = 0b11110000 | (c >> 18);
                        string[i * n_bytes + 1] = 0b10000000 | ((c >> 12) & 0b111111);
                        string[i * n_bytes + 2] = 0b10000000 | ((c >> 6) & 0b111111);
                        string[i * n_bytes + 3] = 0b10000000 | (c & 0b111111);

                        break;
                }
                default:
                        assert(0);
                }
        }

        test_trivial_utf8(string, ((TEST_STRING_SIZE - 1) / n_bytes) * n_bytes + 1, n_bytes);
        test_utf8(string, ((TEST_STRING_SIZE - 1) / n_bytes) * n_bytes + 1, n_bytes);
        test_strlen(string, ((TEST_STRING_SIZE - 1) / n_bytes) * n_bytes + 1, n_bytes);

        free(string);
}

int main(int argc, char **argv) {
        test_multibytes(2);
        test_multibytes(3);
        test_multibytes(4);

        return 0;
}
