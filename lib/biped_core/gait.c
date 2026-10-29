#include "gait.h"

#include <math.h>

#include "ik.h"

#define LIFT_Z   (STAND_Z - 15.0f)
#define STEP     12.0f
#define LEAN     12.0f

/* x values are half a step, the foot goes from -STEP to +STEP */
const gait_key_t GAIT_WALK[] = {
    /*  lx      lz       rx      rz       roll     ms */
    { {  0,     STAND_Z,  0,     STAND_Z,  0     },  300 },  /* 0 stand */
    { {  0,     STAND_Z,  0,     STAND_Z,  LEAN  },  400 },  /* 1 lean right */
    { {  0,     LIFT_Z,   0,     STAND_Z,  LEAN  },  250 },  /* 2 lift left */
    /* loop starts here */
    { {  STEP,  LIFT_Z,  -STEP,  STAND_Z,  LEAN  },  350 },  /* 3 swing left */
    { {  STEP,  STAND_Z, -STEP,  STAND_Z,  LEAN  },  250 },  /* 4 drop left */
    { {  STEP,  STAND_Z, -STEP,  STAND_Z, -LEAN  },  500 },  /* 5 lean left */
    { {  STEP,  STAND_Z, -STEP,  LIFT_Z,  -LEAN  },  250 },  /* 6 lift right */
    { { -STEP,  STAND_Z,  STEP,  LIFT_Z,  -LEAN  },  350 },  /* 7 swing right */
    { { -STEP,  STAND_Z,  STEP,  STAND_Z, -LEAN  },  250 },  /* 8 drop right */
    { { -STEP,  STAND_Z,  STEP,  STAND_Z,  LEAN  },  500 },  /* 9 lean right */
    { { -STEP,  LIFT_Z,   STEP,  STAND_Z,  LEAN  },  250 },  /* 10 lift left */
};
const int GAIT_WALK_LEN = sizeof(GAIT_WALK) / sizeof(GAIT_WALK[0]);
const int GAIT_WALK_LOOP = 3;

const gait_key_t GAIT_HOME[] = {
    { { 0, STAND_Z, 0, STAND_Z, 0 }, 600 },
};

float ease_cos(float t)
{
    if (t <= 0.0f)
        return 0.0f;
    if (t >= 1.0f)
        return 1.0f;
    return 0.5f - 0.5f * cosf(t * 3.14159265f);
}

void foot_pose_lerp(const foot_pose_t *a, const foot_pose_t *b, float t,
                    foot_pose_t *out)
{
    out->lx = a->lx + (b->lx - a->lx) * t;
    out->lz = a->lz + (b->lz - a->lz) * t;
    out->rx = a->rx + (b->rx - a->rx) * t;
    out->rz = a->rz + (b->rz - a->rz) * t;
    out->roll = a->roll + (b->roll - a->roll) * t;
}

void gait_init(gait_player_t *p, uint32_t now)
{
    p->keys = GAIT_HOME;
    p->n = 1;
    p->loop_from = -1;
    p->idx = 0;
    p->seg_start = now;
    p->cur = GAIT_HOME[0].pose;
    p->from = p->cur;
    p->speed_pct = 100;
    p->done = 1;
}

void gait_start(gait_player_t *p, const gait_key_t *keys, int n,
                int loop_from, uint32_t now)
{
    /* always start from wherever the feet are right now */
    p->from = p->cur;
    p->keys = keys;
    p->n = n;
    p->loop_from = loop_from;
    p->idx = 0;
    p->seg_start = now;
    p->done = 0;
}

const foot_pose_t *gait_update(gait_player_t *p, uint32_t now)
{
    uint32_t dur, el;

    while (!p->done) {
        dur = (uint32_t)p->keys[p->idx].ms * 100u / p->speed_pct;
        el = now - p->seg_start;
        if (el < dur) {
            foot_pose_lerp(&p->from, &p->keys[p->idx].pose,
                           ease_cos((float)el / (float)dur), &p->cur);
            break;
        }

        /* segment finished, move on to the next key */
        p->cur = p->keys[p->idx].pose;
        p->from = p->cur;
        p->seg_start += dur;
        if (p->idx + 1 < p->n) {
            p->idx++;
        } else if (p->loop_from >= 0) {
            p->idx = p->loop_from;
        } else {
            p->done = 1;
        }
    }
    return &p->cur;
}

int foot_to_joints(const foot_pose_t *f, joint_pose_t *j)
{
    leg_angles_t l, r;

    if (ik_leg(f->lx, f->lz, &l) != 0 || ik_leg(f->rx, f->rz, &r) != 0)
        return -1;

    j->deg[L_HIP] = l.hip;
    j->deg[L_KNEE] = l.knee;
    j->deg[L_ANKLE] = l.ankle;
    j->deg[R_HIP] = r.hip;
    j->deg[R_KNEE] = r.knee;
    j->deg[R_ANKLE] = r.ankle;
    /* both ankles roll the same way to tilt the body sideways */
    j->deg[L_ROLL] = f->roll;
    j->deg[R_ROLL] = f->roll;
    return 0;
}
