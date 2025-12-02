CC = g++

# Compilation flags based on mode
MODE ?= fast

DEBUG_FLAGS = -g -Wall -Wextra -Werror -fsanitize=address -fsanitize=undefined -MMD -MP -std=c++23 -I.
OPTIMIZED_FLAGS = -O3 -march=native -flto -funroll-loops -fstrict-aliasing -ffast-math -fno-math-errno -fomit-frame-pointer -MMD -MP -std=c++23 -I.

CFLAGS = $(if $(filter debug,$(MODE)),$(DEBUG_FLAGS),$(OPTIMIZED_FLAGS))

LDFLAGS = $(if $(filter debug,$(MODE)),-fsanitize=address -fsanitize=undefined,)

BUILD_DIR = build

# Recursively find all cpp files
SRCS := $(shell find . -type f -name '*.cpp')

# Convert sources to build/*.o with mirrored directory structure
OBJS := $(patsubst ./%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

MAIN_OBJ := $(BUILD_DIR)/main.o
TEST_OBJ := $(BUILD_DIR)/test.o
OTHER_OBJS := $(filter-out $(MAIN_OBJ) $(TEST_OBJ), $(OBJS))

OBJS_MAIN := $(MAIN_OBJ) $(OTHER_OBJS)
OBJS_TEST := $(TEST_OBJ) $(OTHER_OBJS)

DEPS := $(OBJS:.o=.d)

all: $(BUILD_DIR)/main

$(BUILD_DIR)/main: $(OBJS_MAIN)
	$(CC) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/test: $(OBJS_TEST)
	$(CC) $^ $(LDFLAGS) -o $@

$(BUILD_DIR)/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

main: $(BUILD_DIR)/main
	./$(BUILD_DIR)/main

test: $(BUILD_DIR)/test
	./$(BUILD_DIR)/test

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.ppm

.PHONY: all main test clean