/*
 * 2-link leg kinematics in the sagittal plane.
 *
 * Coordinates are relative to the hip joint: x forward, z down (mm).
 * Angles in degrees:
 *   hip   - thigh angle from vertical, + is forward
 *   knee  - bend, 0 = straight, + bends the shin backward
 *   ankle - foot pitch needed to keep the sole parallel to the ground
 */
#ifndef IK_H
#define IK_H

#ifdef __cplusplus
extern "C" {
#endif

#define THIGH_MM  55.0f
#define SHANK_MM  55.0f

typedef struct {
    float hip;
    float knee;
    float ankle;
} leg_angles_t;

/* returns 0 on success, -1 if the point is out of reach */
int ik_leg(float x, float z, leg_angles_t *out);
void fk_leg(const leg_angles_t *a, float *x, float *z);

#ifdef __cplusplus
}
#endif

#endif
