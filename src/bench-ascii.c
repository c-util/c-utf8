/*
 * ASCII validation benchmark
 */

#undef NDEBUG
#include <assert.h>
#include <c-stdaux.h>
#include <inttypes.h>
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
        c_assert(r >= 0);

        return ts.tv_sec * UINT64_C(1000000000) + ts.tv_nsec;
}

static void test_trivial_utf8(const char *string, size_t n_string) {
        uint64_t ts;
        const char *res;

        ts = test_get_time();
        res = trivial_utf8_is_valid(string);
        fprintf(stderr, "UTF-8 verify ASCII string (trivial): %"PRIu64" MB/s\n", n_string * 1000 * 1000 * 1000 / 1024 / 1024 / (test_get_time() - ts));
        c_assert(res == string);
}

static void test_utf8(const char *string, size_t n_string) {
        uint64_t ts;
        size_t len = n_string;

        ts = test_get_time();
        c_utf8_verify(&string, &len);
        fprintf(stderr, "UTF-8 verify ASCII string: %"PRIu64" MB/s\n", n_string * 1000 * 1000 * 1000 / 1024 / 1024 / (test_get_time() - ts));
        c_assert(len == 1);
}

static void test_strlen(const char *string, size_t n_string) {
        uint64_t ts;
        size_t len;

        ts = test_get_time();
        len = strnlen(string, n_string);
        fprintf(stderr, "strlen() of ASCII string: %"PRIu64" MB/s\n", n_string * 1000 * 1000 * 1000 / 1024 / 1024 / (test_get_time() - ts));
        c_assert(len == n_string - 1);
}

#define TEST_STRING_SIZE (500ULL * 1024ULL * 1024ULL)

int main(int argc, char **argv) {
        char *string;

        string = malloc(TEST_STRING_SIZE);
        c_assert(string);

        for (size_t i = 0; i < TEST_STRING_SIZE - 1; ++i)
                string[i] = (i % 127) + 1;

        string[TEST_STRING_SIZE - 1] = '\0';

        test_trivial_utf8(string, TEST_STRING_SIZE);
        test_utf8(string, TEST_STRING_SIZE);
        test_strlen(string, TEST_STRING_SIZE);

        return 0;
}
