#include <math.h>
#include <stdio.h>

#include "ik.h"
#include "test_util.h"

static void test_straight_down(void)
{
    leg_angles_t a;

    CHECK(ik_leg(0.0f, 100.0f, &a) == 0);
    /* symmetric bend: hip forward by half the knee */
    CHECK_NEAR(a.hip, a.knee / 2.0f, 0.01);
    CHECK_NEAR(a.ankle, a.knee / 2.0f, 0.01);
}

static void test_known_pose(void)
{
    leg_angles_t a;

    /* l1 = l2 = 55, z = 55*sqrt(2) -> knee 90 deg, hip 45 deg */
    CHECK(ik_leg(0.0f, 77.782f, &a) == 0);
    CHECK_NEAR(a.knee, 90.0f, 0.05);
    CHECK_NEAR(a.hip, 45.0f, 0.05);
}

static void test_round_trip_grid(void)
{
    float x, z, fx, fz;
    leg_angles_t a;
    int bad = 0, n = 0;

    for (x = -40.0f; x <= 40.0f; x += 5.0f) {
        for (z = 60.0f; z <= 105.0f; z += 5.0f) {
            if (ik_leg(x, z, &a) != 0)
                continue;
            n++;
            fk_leg(&a, &fx, &fz);
            if (fabsf(fx - x) > 0.01f || fabsf(fz - z) > 0.01f)
                bad++;
        }
    }
    CHECK(n > 100);
    CHECK(bad == 0);
}

static void test_out_of_reach(void)
{
    leg_angles_t a;

    CHECK(ik_leg(0.0f, 120.0f, &a) == -1);
    CHECK(ik_leg(80.0f, 80.0f, &a) == -1);
    CHECK(ik_leg(0.0f, 0.5f, &a) == -1);
    CHECK(ik_leg(10.0f, -50.0f, &a) == -1);   /* above the hip */
}

void run_ik_tests(void)
{
    test_straight_down();
    test_known_pose();
    test_round_trip_grid();
    test_out_of_reach();
}
