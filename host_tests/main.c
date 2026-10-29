#include <stdio.h>

int g_checks, g_failures;

void run_ik_tests(void);
void run_servo_tests(void);
void run_gait_tests(void);
void run_cmd_tests(void);

int main(void)
{
    run_ik_tests();
    run_servo_tests();
    run_gait_tests();
    run_cmd_tests();
    printf("%d checks, %d failed\n", g_checks, g_failures);
    return g_failures ? 1 : 0;
}
