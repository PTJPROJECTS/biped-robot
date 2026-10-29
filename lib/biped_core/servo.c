#include "servo.h"

/*
 * Default calibration for my build. center_us values came from the
 * horn alignment step in docs/calibration.md, re-check after any rebuild.
 */
servo_cal_t g_servo_cal[NUM_SERVOS] = {
    /* center  dir  min  max */
    { 1500,  1, -45,  60 },   /* L_HIP   */
    { 1470,  1,   0, 100 },   /* L_KNEE  */
    { 1530,  1, -50,  50 },   /* L_ANKLE */
    { 1500,  1, -20,  20 },   /* L_ROLL  */
    { 1520, -1, -45,  60 },   /* R_HIP   */
    { 1500, -1,   0, 100 },   /* R_KNEE  */
    { 1480, -1, -50,  50 },   /* R_ANKLE */
    { 1510,  1, -20,  20 },   /* R_ROLL  */
};

uint16_t servo_angle_to_us(const servo_cal_t *c, float deg)
{
    float us;

    if (deg < c->min_deg)
        deg = c->min_deg;
    if (deg > c->max_deg)
        deg = c->max_deg;

    us = c->center_us + c->dir * deg * SERVO_US_PER_DEG;
    if (us < SERVO_MIN_US)
        us = SERVO_MIN_US;
    if (us > SERVO_MAX_US)
        us = SERVO_MAX_US;
    return (uint16_t)(us + 0.5f);
}

uint16_t servo_us_to_ticks(uint16_t us, uint32_t osc_hz)
{
    /* prescale is computed for 50 Hz from the nominal 25 MHz clock, so the
     * real period is a bit shorter when the oscillator runs fast */
    const uint32_t prescale = 121;   /* round(25e6 / (4096 * 50)) - 1 */
    uint32_t tick_ns = (uint32_t)(1000000000ull * (prescale + 1) / osc_hz);
    return (uint16_t)(((uint32_t)us * 1000u + tick_ns / 2) / tick_ns);
}

const char *servo_name(int idx)
{
    static const char *names[NUM_SERVOS] = {
        "L_HIP", "L_KNEE", "L_ANKLE", "L_ROLL",
        "R_HIP", "R_KNEE", "R_ANKLE", "R_ROLL",
    };
    if (idx < 0 || idx >= NUM_SERVOS)
        return "?";
    return names[idx];
}
