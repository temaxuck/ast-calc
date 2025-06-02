BUILD_DIR=build
CC=gcc
CC_ARGS=-Wall -o $(BUILD_DIR)/main -pg
TARGET_FILE=example.c

.PHONY: build run

build:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CC_ARGS) $(TARGET_FILE)

run: build
	$(BUILD_DIR)/main
