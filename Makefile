BUILD_DIR		?= build
CC				?= gcc
DEBUG			?=
CC_FLAGS		?= -Wall $(if $(DEBUG),-ggdb,)
LINKER_FLAGS	?= -lm
TARGET_FILE		?= example.c
OUT_FILE		?= main

.PHONY: build run repl

repl: override TARGET_FILE = repl.c
repl: override OUT_FILE = repl
repl: run

run: build
	$(BUILD_DIR)/$(OUT_FILE)

build:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CC_FLAGS) -o $(BUILD_DIR)/$(OUT_FILE) $(TARGET_FILE) $(LINKER_FLAGS)
