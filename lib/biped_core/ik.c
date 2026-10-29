#include "ik.h"

#include <math.h>

#define RAD2DEG(r) ((r) * 57.29578f)
#define DEG2RAD(d) ((d) * 0.017453293f)

int ik_leg(float x, float z, leg_angles_t *out)
{
    const float l1 = THIGH_MM, l2 = SHANK_MM;
    float d2 = x * x + z * z;
    float c, knee, hip;

    /* keep a little margin, a fully straight knee makes the next move jerky */
    if (d2 > (l1 + l2 - 0.5f) * (l1 + l2 - 0.5f))
        return -1;
    if (d2 < (l1 - l2) * (l1 - l2) + 1.0f || z <= 0.0f)
        return -1;

    c = (d2 - l1 * l1 - l2 * l2) / (2.0f * l1 * l2);
    if (c > 1.0f)
        c = 1.0f;
    if (c < -1.0f)
        c = -1.0f;
    knee = acosf(c);
    hip = atan2f(x, z) + atan2f(l2 * sinf(knee), l1 + l2 * cosf(knee));

    out->hip = RAD2DEG(hip);
    out->knee = RAD2DEG(knee);
    out->ankle = out->knee - out->hip;
    return 0;
}

void fk_leg(const leg_angles_t *a, float *x, float *z)
{
    float t1 = DEG2RAD(a->hip);
    float t2 = DEG2RAD(a->hip - a->knee);

    *x = THIGH_MM * sinf(t1) + SHANK_MM * sinf(t2);
    *z = THIGH_MM * cosf(t1) + SHANK_MM * cosf(t2);
}
