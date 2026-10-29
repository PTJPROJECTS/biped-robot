#include "servo.h"

/*
 * Default calibration table. The center_us numbers are example trims,
 * every build needs its own - follow docs/calibration.md and update these.
 * Ankle max is 60 because the "lift back foot" key needs about 51 deg.
 */
servo_cal_t g_servo_cal[NUM_SERVOS] = {
    /* center  dir  min  max */
    { 1500,  1, -45,  60 },   /* L_HIP   */
    { 1470,  1,   0, 100 },   /* L_KNEE  */
    { 1530,  1, -50,  60 },   /* L_ANKLE */
    { 1500,  1, -20,  20 },   /* L_ROLL  */
    { 1520, -1, -45,  60 },   /* R_HIP   */
    { 1500, -1,   0, 100 },   /* R_KNEE  */
    { 1480, -1, -50,  60 },   /* R_ANKLE */
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
    /* Same prescale math as Adafruit_PWMServoDriver::setPWMFreq(50), so
     * this has to be given the same osc_hz as setOscillatorFrequency().
     * The prescale is an integer, so the real frame is not exactly 20 ms
     * and the tick length has to come from the prescale, not from 20ms/4096. */
    uint32_t prescale = (uint32_t)((float)osc_hz / (4096.0f * 50.0f) - 0.5f);
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
