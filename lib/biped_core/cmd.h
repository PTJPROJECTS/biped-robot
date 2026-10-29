/*
 * Serial command parser. One command per line:
 *   walk | stop | home | cal
 *   j <servo> <deg>     move one joint (robot must be stopped)
 *   trim <servo> <us>   change center pulse, for calibration
 *   speed <pct>         gait speed, 25..200
 */
#ifndef CMD_H
#define CMD_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    CMD_NONE = 0,   /* empty line */
    CMD_WALK,
    CMD_STOP,
    CMD_HOME,
    CMD_CAL,
    CMD_JOINT,
    CMD_TRIM,
    CMD_SPEED,
    CMD_ERROR,
} cmd_type_t;

typedef struct {
    cmd_type_t type;
    int servo;
    int value;
    const char *err;
} cmd_t;

void cmd_parse(const char *line, cmd_t *out);

#ifdef __cplusplus
}
#endif

#endif
