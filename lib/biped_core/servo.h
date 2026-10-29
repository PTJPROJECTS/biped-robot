/*
 * Joint angle -> servo pulse conversion with per-servo calibration.
 * Servos are driven by a PCA9685 at 50 Hz.
 */
#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
    L_HIP = 0, L_KNEE, L_ANKLE, L_ROLL,
    R_HIP, R_KNEE, R_ANKLE, R_ROLL,
    NUM_SERVOS
};

typedef struct {
    int16_t center_us;   /* pulse where the joint is at 0 deg */
    int8_t dir;          /* +1 or -1, right side servos are mirrored */
    int16_t min_deg;
    int16_t max_deg;
} servo_cal_t;

/* MG90S: roughly 500-2500 us over 180 deg */
#define SERVO_US_PER_DEG   11.11f
#define SERVO_MIN_US       500
#define SERVO_MAX_US       2500

extern servo_cal_t g_servo_cal[NUM_SERVOS];

uint16_t servo_angle_to_us(const servo_cal_t *c, float deg);
/* osc_hz is the real PCA9685 oscillator frequency. It is usually off
 * from the nominal 25 MHz by a few percent, see docs/calibration.md */
uint16_t servo_us_to_ticks(uint16_t us, uint32_t osc_hz);
const char *servo_name(int idx);

#ifdef __cplusplus
}
#endif

#endif
