# Host build for the shared biped code. Firmware is built with PlatformIO.

CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -Werror -O1 -g
CFLAGS  += -Ilib/biped_core -Ihost_tests
LDLIBS  += -lm

BUILD   := build
CORE    := $(wildcard lib/biped_core/*.c)
TESTS   := $(wildcard host_tests/*.c)

.PHONY: all test clean

all: $(BUILD)/run_tests

$(BUILD):
	mkdir -p $@

$(BUILD)/run_tests: $(TESTS) $(CORE) | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

test: all
	./$(BUILD)/run_tests

clean:
	rm -rf $(BUILD)
