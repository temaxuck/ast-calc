BUILD_DIR=build
CC=gcc
CC_ARGS=-Wall -o $(BUILD_DIR)/main -pg

.PHONY: build run

build:
	mkdir -p $(BUILD_DIR)
	$(CC) $(CC_ARGS) main.c

run: build
	$(BUILD_DIR)/main
