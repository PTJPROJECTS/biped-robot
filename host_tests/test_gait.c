#include <math.h>

#include "gait.h"
#include "ik.h"
#include "test_util.h"

static void test_ease(void)
{
    CHECK_NEAR(ease_cos(0.0f), 0.0, 1e-6);
    CHECK_NEAR(ease_cos(0.5f), 0.5, 1e-6);
    CHECK_NEAR(ease_cos(1.0f), 1.0, 1e-6);
    CHECK_NEAR(ease_cos(2.0f), 1.0, 1e-6);
    /* slow start */
    CHECK(ease_cos(0.1f) < 0.1f);
}

static void test_every_key_reachable(void)
{
    joint_pose_t j;
    int i, k;

    for (i = 0; i < GAIT_WALK_LEN; i++) {
        CHECK(foot_to_joints(&GAIT_WALK[i].pose, &j) == 0);
        for (k = 0; k < NUM_SERVOS; k++) {
            CHECK(j.deg[k] >= g_servo_cal[k].min_deg);
            CHECK(j.deg[k] <= g_servo_cal[k].max_deg);
        }
    }
}

static void test_never_both_feet_lifted(void)
{
    gait_player_t p;
    uint32_t t;
    const foot_pose_t *f;
    int bad = 0;

    gait_init(&p, 0);
    gait_start(&p, GAIT_WALK, GAIT_WALK_LEN, GAIT_WALK_LOOP, 0);
    for (t = 0; t < 20000; t += 20) {
        f = gait_update(&p, t);
        if (f->lz < STAND_Z - 0.5f && f->rz < STAND_Z - 0.5f)
            bad++;
    }
    CHECK(bad == 0);
}

static void test_lean_before_lift(void)
{
    /* foot must only come up while the body leans over the other foot */
    gait_player_t p;
    uint32_t t;
    const foot_pose_t *f;
    int bad = 0;

    gait_init(&p, 0);
    gait_start(&p, GAIT_WALK, GAIT_WALK_LEN, GAIT_WALK_LOOP, 0);
    for (t = 0; t < 20000; t += 20) {
        f = gait_update(&p, t);
        if (f->lz < STAND_Z - 1.0f && f->roll < 11.0f)
            bad++;
        if (f->rz < STAND_Z - 1.0f && f->roll > -11.0f)
            bad++;
    }
    CHECK(bad == 0);
}

static void test_smooth_motion(void)
{
    /* at 20 ms steps no joint should jump more than a few degrees */
    gait_player_t p;
    joint_pose_t prev, cur;
    uint32_t t;
    int k;
    float worst = 0.0f;

    gait_init(&p, 0);
    gait_start(&p, GAIT_WALK, GAIT_WALK_LEN, GAIT_WALK_LOOP, 0);
    CHECK(foot_to_joints(gait_update(&p, 0), &prev) == 0);
    for (t = 20; t < 15000; t += 20) {
        CHECK(foot_to_joints(gait_update(&p, t), &cur) == 0);
        for (k = 0; k < NUM_SERVOS; k++) {
            float d = fabsf(cur.deg[k] - prev.deg[k]);
            if (d > worst)
                worst = d;
        }
        prev = cur;
    }
    CHECK(worst < 4.0f);
}

static void test_stop_returns_home(void)
{
    gait_player_t p;
    const foot_pose_t *f;

    gait_init(&p, 0);
    gait_start(&p, GAIT_WALK, GAIT_WALK_LEN, GAIT_WALK_LOOP, 0);
    gait_update(&p, 2345);
    gait_start(&p, GAIT_HOME, 1, -1, 2345);
    f = gait_update(&p, 2345 + 700);
    CHECK(p.done);
    CHECK_NEAR(f->lx, 0.0, 1e-4);
    CHECK_NEAR(f->rz, STAND_Z, 1e-4);
    CHECK_NEAR(f->roll, 0.0, 1e-4);
}

static void test_speed_scaling(void)
{
    gait_player_t p;

    gait_init(&p, 0);
    p.speed_pct = 200;
    gait_start(&p, GAIT_HOME, 1, -1, 0);
    p.cur.lx = 10.0f;
    p.from.lx = 10.0f;
    gait_update(&p, 310);   /* 600 ms key at 2x speed = 300 ms */
    CHECK(p.done);
}

void run_gait_tests(void)
{
    test_ease();
    test_every_key_reachable();
    test_never_both_feet_lifted();
    test_lean_before_lift();
    test_smooth_motion();
    test_stop_returns_home();
    test_speed_scaling();
}
