/*
 * Tests for UTF-8 validation helper
 */

#undef NDEBUG
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c-utf8.h"

static void test_ascii(void) {
        char str[0x100];
        const char *p = str;
        size_t len = sizeof(str);

        for (size_t i = 0; i < sizeof(str); ++i)
                str[i] = i;

        c_utf8_verify_ascii(&p, &len);
        assert(*p == 0x00);
        assert(p == str);
        assert(len == sizeof(str));

        ++p;
        --len;

        c_utf8_verify_ascii(&p, &len);
        assert((unsigned char)*p == 0x80);
        assert(p == str + 0x7F + 1);
        assert(len == sizeof(str) - 0x7F - 1);

}

static void test_utf8(void) {
        /* verify a mix of greek, czech and chinese */
        {
                char str[] = "Η Ελλάδα ή Ελλάς, επίσημα γνωστή ως Ελληνική "
                             "Δημοκρατία, είναι χώρα της νοτιοανατολικής "
                             "Ευρώπης στο νοτιότερο άκρο της Βαλκανικής "
                             "χερσονήσου. Συνορεύει στα βορειοδυτικά με την "
                             "Αλβανία, στα βόρεια με την πρώην Γιουγκοσλαβική "
                             "Δημοκρατία της Μακεδονίας και τη Βουλγαρία και "
                             "στα βορειοανατολικά με την Τουρκία. Česko, "
                             "úředním názvem Česká republika, je stát ve "
                             "střední Evropě. Jako formálně svrchovaný národní "
                             "stát vznikla tehdejší Česká socialistická "
                             "republika 1. ledna 1969 v rámci federalizace "
                             "Československa. Od 6. března 1990 nese tento "
                             "stát název Česká republika. 中華民國十年，"
                             "中國共產黨立於上海。初附於中國國民黨，"
                             "黨人得以兼國民黨，共理中華民國廣州軍政府，"
                             "同謀北伐。其後國民黨人以共產黨人以公務營黨務，"
                             "既下南京，蔣中正令捕殺共產黨人。遂奔江西。"
                             "十六年起義於南昌，中國工農紅軍是立";
                const char *p = str;
                size_t len = sizeof(str);

                c_utf8_verify(&p, &len);
                assert(len == 1);
                assert(p == str + strlen(str));
                assert(*p == 0x00);
        }

        /* verify every 1-byte character */
        for (uint32_t i = 0; i <= 0xFF; i++) {
                char str[] = { i };
                size_t len = sizeof(str);
                const char *p = str;

                c_utf8_verify(&p, &len);
                if (str[0] == 0 || (str[0] & 0b10000000) != 0)
                        assert(p == str && len == sizeof(str));
                else
                        assert(p == str + sizeof(str) && len == 0);
        }

        /* verify every 2-byte character */
        for (uint32_t i = 0; i <= 0xFFFF; i++) {
                char str[] = { (i >> 8), i & 0xFF };
                size_t len = sizeof(str);
                const char *p = str;
                uint32_t n;

                if ((str[0] & 0x80) == 0)
                        /* ignore leading 1-byte characters */
                        continue;

                n = (str[0] & 0b00011111) << 6 | (str[1] & 0b00111111);

                c_utf8_verify(&p, &len);

                if (((str[0] & 0b11100000) != 0b11000000) ||
                    ((str[1] & 0b11000000) != 0b10000000) ||
                    (n < 0x80)) /* overlong */
                        assert(p == str && len == sizeof(str));
                else
                        assert(p == str + sizeof(str) && len == 0);
        }

        /* verify every 0xF'th 3-byte character */
        for (uint32_t i = 0; i <= 0xFFFFFF; i += 0xF) {
                char str[] = { (i >> 16), (i >> 8) & 0xFF, i & 0xFF };
                size_t len = sizeof(str);
                const char *p = str;
                uint32_t n;

                if (((str[0] & 0b10000000) == 0) ||
                    ((str[0] & 0b11100000) == 0b11000000))
                        /* ignore leading 1,2-byte characters */
                        continue;

                n = (str[0] & 0b00001111) << 12 | (str[1] & 0b00111111) << 6 | (str[2] & 0b00111111);

                c_utf8_verify(&p, &len);

                if (((str[0] & 0b11110000) != 0b11100000) ||
                    ((str[1] & 0b11000000) != 0b10000000) ||
                    ((str[2] & 0b11000000) != 0b10000000) ||
                    (n < 0x800) || /* overlong */
                    (n >= 0xD800 && n <= 0xDFFF)) /* surrogates */
                        assert(p == str && len == sizeof(str));
                else
                        assert(p == str + sizeof(str) && len == 0);
        }

        /* verify every 0xFFF'th 4-byte character */
        for (uint64_t i = 0; i <= 0xFFFFFFFF; i += 0xFFF) {
                char str[] = { (i >> 24), (i >> 16) & 0xFF, (i >> 8) & 0xFF, i & 0xFF };
                size_t len = sizeof(str);
                const char *p = str;
                uint32_t n;

                if (((str[0] & 0b10000000) == 0) ||
                    ((str[0] & 0b11100000) == 0b11000000) ||
                    ((str[0] & 0b11110000) == 0b11100000))
                        /* ignore leading 1,2,3-byte characters */
                        continue;

                n = (str[0] & 0b00000111) << 18 | (str[1] & 0b00111111) << 12 | (str[2] & 0b00111111) << 6 | (str[3] & 0b00111111);

                c_utf8_verify(&p, &len);

                if (((str[0] & 0b11111000) != 0b11110000) ||
                    ((str[1] & 0b11000000) != 0b10000000) ||
                    ((str[2] & 0b11000000) != 0b10000000) ||
                    ((str[3] & 0b11000000) != 0b10000000) ||
                    (n < 0x10000) || /* overlong */
                    (n > 0x10FFFF)) /* out of unicode range */
                        assert(p == str && len == sizeof(str));
                else
                        assert(p == str + sizeof(str) && len == 0);
        }
}

int main(int argc, char **argv) {
        test_ascii();
        test_utf8();
        return 0;
}
