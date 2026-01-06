# Immutable Makefile for MCC
# Targets x86-64 Linux
# Compiler: System GCC (used to bootstrap MCC)

TARGET := mcc
CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -g -Iinclude
LDFLAGS := 

# Directories
SRC_DIR := src
OBJ_DIR := build
INC_DIR := include

# Recursively find all .c files in src/
SRCS := $(shell find $(SRC_DIR) -name '*.c')

# Generate object file paths in build/ mirroring src/ structure
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Generate dependency files
DEPS := $(OBJS:.o=.d)

# Main Target
$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build Complete. Run ./$(TARGET)"

# Compile Source to Objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Formatting
format:
	@echo "Formatting code..."
	@find $(SRC_DIR) $(INC_DIR) -name '*.[ch]' | xargs clang-format -i
	@echo "Done."

# Clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: clean format

# Include dependencies
-include $(DEPS)