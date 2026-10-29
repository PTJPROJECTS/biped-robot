/*
 * Prints the walk gait as CSV using the same code as the firmware,
 * so it can be plotted on the PC (tools/plot_gait.py).
 *
 *   ./build/gait_dump [ms] [step_ms]
 */
#include <stdio.h>
#include <stdlib.h>

#include "gait.h"

int main(int argc, char **argv)
{
    unsigned long total = argc > 1 ? strtoul(argv[1], NULL, 10) : 8000;
    unsigned long step = argc > 2 ? strtoul(argv[2], NULL, 10) : 20;
    gait_player_t p;
    joint_pose_t j;
    unsigned long t;
    int i;

    gait_init(&p, 0);
    gait_start(&p, GAIT_WALK, GAIT_WALK_LEN, GAIT_WALK_LOOP, 0);

    printf("t_ms,lx,lz,rx,rz,roll");
    for (i = 0; i < NUM_SERVOS; i++)
        printf(",%s", servo_name(i));
    printf("\n");

    for (t = 0; t <= total; t += step) {
        const foot_pose_t *f = gait_update(&p, (uint32_t)t);
        if (foot_to_joints(f, &j) != 0) {
            fprintf(stderr, "ik failed at t=%lu\n", t);
            return 1;
        }
        printf("%lu,%.2f,%.2f,%.2f,%.2f,%.2f", t, f->lx, f->lz, f->rx,
               f->rz, f->roll);
        for (i = 0; i < NUM_SERVOS; i++)
            printf(",%.2f", j.deg[i]);
        printf("\n");
    }
    return 0;
}
