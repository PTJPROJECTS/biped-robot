/*
 * Gait playback. A gait is a list of key poses in foot space
 * (foot x/z for each leg plus body roll). The player eases between
 * keys and the result goes through IK to get joint angles.
 */
#ifndef GAIT_H
#define GAIT_H

#include <stdint.h>

#include "servo.h"

#ifdef __cplusplus
extern "C" {
#endif

#define STAND_Z   95.0f   /* hip height when standing, mm */

typedef struct {
    float lx, lz;    /* left foot, relative to left hip */
    float rx, rz;
    float roll;      /* ankle roll, + leans the body over the right foot */
} foot_pose_t;

typedef struct {
    foot_pose_t pose;
    uint16_t ms;     /* time to move from the previous key to this one */
} gait_key_t;

typedef struct {
    const gait_key_t *keys;
    int n;
    int loop_from;   /* index to jump back to after the last key, -1 = hold */
    int idx;         /* key we are moving towards */
    uint32_t seg_start;
    foot_pose_t from;
    foot_pose_t cur;
    uint8_t speed_pct;
    uint8_t done;
} gait_player_t;

typedef struct {
    float deg[NUM_SERVOS];
} joint_pose_t;

extern const gait_key_t GAIT_WALK[];
extern const int GAIT_WALK_LEN;
extern const int GAIT_WALK_LOOP;
extern const gait_key_t GAIT_HOME[];

void gait_init(gait_player_t *p, uint32_t now);
void gait_start(gait_player_t *p, const gait_key_t *keys, int n,
                int loop_from, uint32_t now);
const foot_pose_t *gait_update(gait_player_t *p, uint32_t now);

/* 0..1 -> 0..1, slow at both ends so the servos don't jerk */
float ease_cos(float t);
void foot_pose_lerp(const foot_pose_t *a, const foot_pose_t *b, float t,
                    foot_pose_t *out);

/* runs IK for both legs. returns -1 if either foot is out of reach */
int foot_to_joints(const foot_pose_t *f, joint_pose_t *j);

#ifdef __cplusplus
}
#endif

#endif
