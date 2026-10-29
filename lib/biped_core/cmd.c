#include "cmd.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "servo.h"

/* avr-libc has no strcasecmp in some setups, so do it by hand */
static int word_eq(const char *a, size_t alen, const char *b)
{
    size_t i;

    if (alen != strlen(b))
        return 0;
    for (i = 0; i < alen; i++)
        if (tolower((unsigned char)a[i]) != b[i])
            return 0;
    return 1;
}

static const char *skip_space(const char *s)
{
    while (*s == ' ' || *s == '\t')
        s++;
    return s;
}

static int parse_int(const char **s, int *out)
{
    char *end;
    long v;

    *s = skip_space(*s);
    v = strtol(*s, &end, 10);
    if (end == *s)
        return -1;
    *s = end;
    *out = (int)v;
    return 0;
}

static int at_end(const char *s)
{
    s = skip_space(s);
    return *s == '\0' || *s == '\r' || *s == '\n';
}

void cmd_parse(const char *line, cmd_t *out)
{
    const char *p = skip_space(line);
    const char *w = p;
    size_t wlen;

    memset(out, 0, sizeof(*out));

    while (*p && !isspace((unsigned char)*p))
        p++;
    wlen = (size_t)(p - w);

    if (wlen == 0) {
        out->type = CMD_NONE;
        return;
    }

    if (word_eq(w, wlen, "walk"))
        out->type = CMD_WALK;
    else if (word_eq(w, wlen, "stop"))
        out->type = CMD_STOP;
    else if (word_eq(w, wlen, "home"))
        out->type = CMD_HOME;
    else if (word_eq(w, wlen, "cal"))
        out->type = CMD_CAL;
    else if (word_eq(w, wlen, "j"))
        out->type = CMD_JOINT;
    else if (word_eq(w, wlen, "trim"))
        out->type = CMD_TRIM;
    else if (word_eq(w, wlen, "speed"))
        out->type = CMD_SPEED;
    else {
        out->type = CMD_ERROR;
        out->err = "unknown command";
        return;
    }

    switch (out->type) {
    case CMD_JOINT:
    case CMD_TRIM:
        if (parse_int(&p, &out->servo) || parse_int(&p, &out->value)) {
            out->type = CMD_ERROR;
            out->err = "usage: j|trim <servo 0-7> <value>";
            return;
        }
        if (out->servo < 0 || out->servo >= NUM_SERVOS) {
            out->type = CMD_ERROR;
            out->err = "servo out of range";
            return;
        }
        if (out->type == CMD_TRIM &&
            (out->value < SERVO_MIN_US || out->value > SERVO_MAX_US)) {
            out->type = CMD_ERROR;
            out->err = "trim must be 500-2500 us";
            return;
        }
        break;
    case CMD_SPEED:
        if (parse_int(&p, &out->value) || out->value < 25 || out->value > 200) {
            out->type = CMD_ERROR;
            out->err = "speed must be 25-200";
            return;
        }
        break;
    default:
        break;
    }

    if (!at_end(p)) {
        out->type = CMD_ERROR;
        out->err = "extra arguments";
    }
}
