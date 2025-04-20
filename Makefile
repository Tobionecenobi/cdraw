# Makefile for p5c drawing library

# Detect OS
ifeq ($(OS),Windows_NT)
    PLATFORM = WINDOWS
    CC = gcc
    CFLAGS = -Wall -Wextra -O2 -I./include
    LDFLAGS = -lgdi32 -luser32 -lm
    EXE_EXT = .exe
else
    PLATFORM = LINUX
    CC = gcc
    CFLAGS = -Wall -Wextra -O2 -I./include
    LDFLAGS = -lX11 -lm
    EXE_EXT =
endif

# Directories
SRC_DIR = src
INCLUDE_DIR = include
EXAMPLES_DIR = examples
BUILD_DIR = build

# Source files
LIB_SRCS = $(SRC_DIR)/p5c.c
LIB_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(LIB_SRCS))

# Example files
EXAMPLE_SRCS = $(filter-out $(EXAMPLES_DIR)/header_only_example.c, $(wildcard $(EXAMPLES_DIR)/*.c))
EXAMPLE_BINS = $(patsubst $(EXAMPLES_DIR)/%.c,$(BUILD_DIR)/%$(EXE_EXT),$(EXAMPLE_SRCS))

# Header-only example
HEADER_ONLY_EXAMPLE = $(BUILD_DIR)/header_only_example$(EXE_EXT)

# Default target
all: $(BUILD_DIR) $(LIB_OBJS) $(EXAMPLE_BINS) $(HEADER_ONLY_EXAMPLE)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Compile library source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -D_$(PLATFORM) -c $< -o $@

# Link example executables
$(BUILD_DIR)/%$(EXE_EXT): $(EXAMPLES_DIR)/%.c $(LIB_OBJS)
	$(CC) $(CFLAGS) -D_$(PLATFORM) $< $(LIB_OBJS) -o $@ $(LDFLAGS)

# Build header-only example
$(HEADER_ONLY_EXAMPLE): $(EXAMPLES_DIR)/header_only_example.c
	$(CC) $(CFLAGS) -D_$(PLATFORM) $< -o $@ $(LDFLAGS)

# Clean build files
clean:
	rm -rf $(BUILD_DIR)

# Run the main example
run: $(BUILD_DIR)/main$(EXE_EXT)
	$(BUILD_DIR)/main$(EXE_EXT)

.PHONY: all clean run
