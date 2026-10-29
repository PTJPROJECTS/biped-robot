#include "servo.h"
#include "test_util.h"

static void test_center_and_direction(void)
{
    servo_cal_t c = { 1500, 1, -90, 90 };
    servo_cal_t m = { 1500, -1, -90, 90 };

    CHECK(servo_angle_to_us(&c, 0.0f) == 1500);
    CHECK(servo_angle_to_us(&c, 45.0f) == 2000);
    CHECK(servo_angle_to_us(&m, 45.0f) == 1000);
}

static void test_limits(void)
{
    servo_cal_t c = { 1500, 1, 0, 100 };

    /* knee can't bend backwards */
    CHECK(servo_angle_to_us(&c, -30.0f) == 1500);
    CHECK(servo_angle_to_us(&c, 150.0f) == servo_angle_to_us(&c, 100.0f));

    c.min_deg = -180;
    c.max_deg = 180;
    CHECK(servo_angle_to_us(&c, 170.0f) == SERVO_MAX_US);
    CHECK(servo_angle_to_us(&c, -170.0f) == SERVO_MIN_US);
}

static void test_ticks(void)
{
    /* nominal clock: 122 * 40 ns = 4.88 us per tick */
    CHECK(servo_us_to_ticks(1500, 25000000u) == 307);
    /* fast oscillator -> shorter tick -> more ticks for the same pulse */
    CHECK(servo_us_to_ticks(1500, 26100000u) == 321);
}

static void test_default_table(void)
{
    int i;

    for (i = 0; i < NUM_SERVOS; i++) {
        CHECK(g_servo_cal[i].dir == 1 || g_servo_cal[i].dir == -1);
        CHECK(g_servo_cal[i].min_deg < g_servo_cal[i].max_deg);
        CHECK(g_servo_cal[i].center_us > 1300 && g_servo_cal[i].center_us < 1700);
    }
}

void run_servo_tests(void)
{
    test_center_and_direction();
    test_limits();
    test_ticks();
    test_default_table();
}
