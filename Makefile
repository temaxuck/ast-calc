BUILD_DIR	?= build
CC			?= gcc
DEBUG       ?=
CC_FLAGS	?= -Wall $(if $(DEBUG),-ggdb,) -o $(BUILD_DIR)/main 
TARGET_FILE ?= repl.c

.PHONY: build run

build:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CC_FLAGS) $(TARGET_FILE)

run: build
	$(BUILD_DIR)/main
