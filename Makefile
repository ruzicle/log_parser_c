# ==============================================================================
# Makefile — log_parser
# ==============================================================================

# Compiler & core flags
CC      := gcc
CFLAGS  := -std=c11 -Wall -Wextra -Wpedantic -Wshadow \
           -Wstrict-prototypes -fstack-protector-strong

# Directories
SRC_DIR     := src
INC_DIR     := include
OBJ_DIR     := build/obj
BIN_DIR     := build/bin
TEST_DIR    := tests

# Target binary
TARGET      := $(BIN_DIR)/log_parser

# Source discovery and object mapping
SRCS        := $(wildcard $(SRC_DIR)/*.c)
OBJS        := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Valgrind configuration
VALGRIND        := valgrind
VALGRIND_FLAGS  := --leak-check=full --show-leak-kinds=all --track-origins=yes --error-exitcode=1

# Sample log for testing
SAMPLE_LOG      := $(TEST_DIR)/sample.log

# ==============================================================================
# BUILD TARGETS
# ==============================================================================

# Default target
.PHONY: all
all: release

# Release build
.PHONY: release
release: CFLAGS += -O2
release: $(TARGET)

# Debug build 
.PHONY: debug
debug: CFLAGS += -g3 -O0 -fsanitize=address,undefined
debug: $(TARGET)

# Link step
$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "  LINK  $@"

# Compile step
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@
	@echo "  CC    $<"

# Directory creation
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# ==============================================================================
# VERIFICATION & RUN TARGETS
# ==============================================================================

# Run with Valgrind
.PHONY: valgrind
valgrind: debug $(SAMPLE_LOG)
	$(VALGRIND) $(VALGRIND_FLAGS) $(TARGET) $(SAMPLE_LOG)

# Run standard test
.PHONY: run
run: debug $(SAMPLE_LOG)
	$(TARGET) $(SAMPLE_LOG)

# Static analysis
.PHONY: check
check:
	cppcheck --enable=all --inconclusive --std=c11 -I$(INC_DIR) $(SRC_DIR)/

# ==============================================================================
# UTILITY
# ==============================================================================

# Print variables for debugging
.PHONY: vars
vars:
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"
	@echo "TARGET = $(TARGET)"

# Clean build files
.PHONY: clean
clean:
	rm -rf build/
	@echo "  CLEAN build/"
