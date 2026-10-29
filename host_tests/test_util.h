#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <math.h>
#include <stdio.h>

extern int g_checks, g_failures;

#define CHECK(cond)                                                   \
    do {                                                              \
        g_checks++;                                                   \
        if (!(cond)) {                                                \
            g_failures++;                                             \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);    \
        }                                                             \
    } while (0)

#define CHECK_NEAR(a, b, tol) CHECK(fabs((double)(a) - (double)(b)) <= (tol))

#endif
