# Host build for the shared biped code. Firmware is built with PlatformIO.

CC      ?= gcc
CFLAGS  ?= -std=c99 -Wall -Wextra -Werror -O1 -g
CFLAGS  += -Ilib/biped_core -Ihost_tests
LDLIBS  += -lm

BUILD   := build
CORE    := $(wildcard lib/biped_core/*.c)
TESTS   := $(wildcard host_tests/*.c)

.PHONY: all test pytest plot clean

all: $(BUILD)/run_tests $(BUILD)/gait_dump

$(BUILD):
	mkdir -p $@

$(BUILD)/run_tests: $(TESTS) $(CORE) | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

$(BUILD)/gait_dump: host/gait_dump.c $(CORE) | $(BUILD)
	$(CC) $(CFLAGS) $^ -o $@ $(LDLIBS)

test: all
	./$(BUILD)/run_tests

pytest: all
	python3 -m pytest -q tests

plot: all
	./$(BUILD)/gait_dump 8000 20 > $(BUILD)/gait.csv
	python3 tools/plot_gait.py $(BUILD)/gait.csv

clean:
	rm -rf $(BUILD)
