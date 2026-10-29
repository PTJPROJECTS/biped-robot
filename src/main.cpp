// Biped walker - Arduino Nano + PCA9685, 8 x MG90S
// Servo channels on the PCA9685 match the enum in servo.h (0..7).

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#include "cmd.h"
#include "gait.h"
#include "servo.h"

#define PCA_ADDR      0x40
#define PCA_OSC_HZ    26100000UL   // measured with a scope, see calibration.md
#define UPDATE_MS     20           // one servo frame

Adafruit_PWMServoDriver pca(PCA_ADDR);
gait_player_t player;
joint_pose_t joints;

static bool manual;           // true after a "j" command, gait output paused
static char line[40];
static uint8_t lineLen;

static void writeServo(int idx, float deg)
{
    uint16_t us = servo_angle_to_us(&g_servo_cal[idx], deg);
    pca.setPWM(idx, 0, servo_us_to_ticks(us, PCA_OSC_HZ));
}

static void writeAll()
{
    for (int i = 0; i < NUM_SERVOS; i++)
        writeServo(i, joints.deg[i]);
}

static void printCal()
{
    for (int i = 0; i < NUM_SERVOS; i++) {
        Serial.print(i);
        Serial.print(' ');
        Serial.print(servo_name(i));
        Serial.print(F(" center="));
        Serial.print(g_servo_cal[i].center_us);
        Serial.print(F(" now="));
        Serial.println(joints.deg[i], 1);
    }
}

static void handleCommand(const char *s)
{
    cmd_t c;
    cmd_parse(s, &c);

    switch (c.type) {
    case CMD_NONE:
        return;
    case CMD_WALK:
        manual = false;
        gait_start(&player, GAIT_WALK, GAIT_WALK_LEN, GAIT_WALK_LOOP, millis());
        break;
    case CMD_STOP:
    case CMD_HOME:
        manual = false;
        gait_start(&player, GAIT_HOME, 1, -1, millis());
        break;
    case CMD_CAL:
        printCal();
        break;
    case CMD_JOINT:
        if (!player.done) {
            Serial.println(F("err stop first"));
            return;
        }
        manual = true;
        joints.deg[c.servo] = c.value;
        writeServo(c.servo, c.value);
        break;
    case CMD_TRIM:
        g_servo_cal[c.servo].center_us = c.value;
        writeServo(c.servo, joints.deg[c.servo]);
        break;
    case CMD_SPEED:
        player.speed_pct = c.value;
        break;
    case CMD_ERROR:
        Serial.print(F("err "));
        Serial.println(c.err);
        return;
    }
    Serial.println(F("ok"));
}

static void readSerial()
{
    while (Serial.available()) {
        char ch = Serial.read();
        if (ch == '\n' || ch == '\r') {
            line[lineLen] = '\0';
            if (lineLen)
                handleCommand(line);
            lineLen = 0;
        } else if (lineLen < sizeof(line) - 1) {
            line[lineLen++] = ch;
        }
    }
}

void setup()
{
    Serial.begin(115200);
    pca.begin();
    pca.setOscillatorFrequency(PCA_OSC_HZ);
    pca.setPWMFreq(50);
    delay(10);

    gait_init(&player, millis());
    foot_to_joints(gait_update(&player, millis()), &joints);
    // stagger the first write, 8 servos jumping at once browns out the 5V rail
    for (int i = 0; i < NUM_SERVOS; i++) {
        writeServo(i, joints.deg[i]);
        delay(150);
    }
    Serial.println(F("biped ready. walk | stop | home | cal | j | trim | speed"));
}

void loop()
{
    static unsigned long last;
    readSerial();

    unsigned long now = millis();
    if (now - last < UPDATE_MS)
        return;
    last = now;

    if (manual)
        return;

    joint_pose_t next;
    if (foot_to_joints(gait_update(&player, now), &next) != 0) {
        // should not happen with the tables in gait.c, but don't move if it does
        Serial.println(F("err ik"));
        gait_start(&player, GAIT_HOME, 1, -1, now);
        return;
    }
    joints = next;
    writeAll();
}
