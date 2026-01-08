CC = clang
CF = -Wall -Wextra -march=native -g

SRC_DIR = compiler
BUILD_DIR = build
BIN_DIR = $(BUILD_DIR)/bin

COMPILER = $(BIN_DIR)/albedo

SRCS 	= $(shell find $(SRC_DIR) -name "*.c")
OBJECTS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

.PHONY: clean

$(COMPILER): $(OBJECTS) | $(BIN_DIR)
	$(CC) $(CF) -o $@ $(OBJECTS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	mkdir -p $(dir $@)
	$(CC) $(CF) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR): | $(BUILD_DIR)
	mkdir -p $@

clean:
	rm -rvf $(BIN_DIR)
	rm -rvf $(BUILD_DIR)
