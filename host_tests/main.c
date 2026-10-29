#include <stdio.h>

int g_checks, g_failures;

void run_ik_tests(void);

int main(void)
{
    run_ik_tests();
    printf("%d checks, %d failed\n", g_checks, g_failures);
    return g_failures ? 1 : 0;
}
