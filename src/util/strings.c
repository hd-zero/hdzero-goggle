#include "strings.h"

#include <stdlib.h>
#include <string.h>

int str_compare_versions(const char *a, const char *b) {
    int result = 0;
    while (result == 0) {
        char *ta;
        char *tb;
        unsigned long va = strtoul(a, &ta, 10);
        unsigned long vb = strtoul(b, &tb, 10);
        if (va < vb) {
            result = -1;
        } else if (va > vb) {
            result = +1;
        } else {
            a = ta;
            b = tb;
            if (*a == '\0' && *b == '\0') {
                break;
            } else if (*a == '\0') {
                result = -1;
            } else if (*b == '\0') {
                result = +1;
            } else {
                a++;
                b++;
            }
        }
    }
    return result;
}

static int str_compare_ptr(const void *p1, const void *p2) {
    return strcmp(*(char *const *)p1, *(char *const *)p2);
}

void str_qsort(char **list, int count) {
    qsort(list, count, sizeof(char *), str_compare_ptr);
}
