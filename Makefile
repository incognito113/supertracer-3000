CC = g++
CFLAGS = -g -Wall -Wextra -Werror -MMD -MP
BUILD_DIR = build

# Automatically find all .cpp files
SRCS := $(wildcard *.cpp)

# Separate main.cpp and test.cpp
MAIN_SRC := main.cpp
TEST_SRC := test.cpp
OTHER_SRCS := $(filter-out $(MAIN_SRC) $(TEST_SRC), $(SRCS))

OBJS_MAIN := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(MAIN_SRC) $(OTHER_SRCS))
OBJS_TEST := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(TEST_SRC) $(OTHER_SRCS))

DEPS := $(OBJS_MAIN:.o=.d) $(OBJS_TEST:.o=.d)

# Default target
all: $(BUILD_DIR)/main

# Build executables
$(BUILD_DIR)/main: $(OBJS_MAIN) | build_dir
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/test: $(OBJS_TEST) | build_dir
	$(CC) $(CFLAGS) $^ -o $@

# Pattern rule for object files
$(BUILD_DIR)/%.o: %.cpp | build_dir
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

# Run targets
main: $(BUILD_DIR)/main
	./$(BUILD_DIR)/main

test: $(BUILD_DIR)/test
	./$(BUILD_DIR)/test

# Build all without running
build: $(BUILD_DIR)/main $(BUILD_DIR)/test

# Clean
clean:
	rm -rf $(BUILD_DIR)

.PHONY: all main test build clean build_dir

build_dir:
	mkdir -p $(BUILD_DIR)
