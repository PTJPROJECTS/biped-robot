#include "cmd.h"
#include "test_util.h"

static cmd_t c;

static void test_simple_words(void)
{
    cmd_parse("walk", &c);
    CHECK(c.type == CMD_WALK);
    cmd_parse("  STOP\r\n", &c);
    CHECK(c.type == CMD_STOP);
    cmd_parse("home\n", &c);
    CHECK(c.type == CMD_HOME);
    cmd_parse("", &c);
    CHECK(c.type == CMD_NONE);
    cmd_parse("   \r\n", &c);
    CHECK(c.type == CMD_NONE);
}

static void test_joint(void)
{
    cmd_parse("j 2 -15", &c);
    CHECK(c.type == CMD_JOINT);
    CHECK(c.servo == 2);
    CHECK(c.value == -15);

    cmd_parse("j 8 10", &c);
    CHECK(c.type == CMD_ERROR);
    cmd_parse("j 1", &c);
    CHECK(c.type == CMD_ERROR);
    cmd_parse("j 1 10 5", &c);
    CHECK(c.type == CMD_ERROR);
}

static void test_trim_and_speed(void)
{
    cmd_parse("trim 4 1520", &c);
    CHECK(c.type == CMD_TRIM && c.servo == 4 && c.value == 1520);
    cmd_parse("trim 4 3000", &c);
    CHECK(c.type == CMD_ERROR);

    cmd_parse("speed 150", &c);
    CHECK(c.type == CMD_SPEED && c.value == 150);
    cmd_parse("speed 0", &c);
    CHECK(c.type == CMD_ERROR);
    cmd_parse("speed fast", &c);
    CHECK(c.type == CMD_ERROR);
}

static void test_unknown(void)
{
    cmd_parse("jump", &c);
    CHECK(c.type == CMD_ERROR);
    CHECK(c.err != NULL);
    cmd_parse("walking", &c);
    CHECK(c.type == CMD_ERROR);
}

void run_cmd_tests(void)
{
    test_simple_words();
    test_joint();
    test_trim_and_speed();
    test_unknown();
}
