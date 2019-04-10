/*
 * Tests for Public API
 * This test, unlikely the others, is linked against the real, distributed,
 * shared library. Its sole purpose is to test for symbol availability.
 */

#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c-utf8.h"

static void test_api(void) {
        const char *string1 = "foo";
        size_t n_string1 = strlen(string1);
        const char *string2 = "bar";
        size_t n_string2 = strlen(string2);

        c_utf8_verify_ascii(&string1, &n_string1);
        c_utf8_verify(&string2, &n_string2);
}

int main(int argc, char **argv) {
        test_api();
        return 0;
}
