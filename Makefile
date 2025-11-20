CC = g++
CFLAGS = -g -Wall -Wextra -Werror -MMD -MP -std=c++23 -I.
BUILD_DIR = build

# Recursively find all cpp files
SRCS := $(shell find . -type f -name '*.cpp')

# Convert sources to build/*.o with mirrored directory structure
OBJS := $(patsubst ./%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Object files for main and test targets
MAIN_OBJ := $(BUILD_DIR)/main.o
TEST_OBJ := $(BUILD_DIR)/test.o
OTHER_OBJS := $(filter-out $(MAIN_OBJ) $(TEST_OBJ), $(OBJS))

OBJS_MAIN := $(MAIN_OBJ) $(OTHER_OBJS)
OBJS_TEST := $(TEST_OBJ) $(OTHER_OBJS)

DEPS := $(OBJS:.o=.d)

all: $(BUILD_DIR)/main

$(BUILD_DIR)/main: $(OBJS_MAIN)
	$(CC) $(CFLAGS) $^ -o $@

$(BUILD_DIR)/test: $(OBJS_TEST)
	$(CC) $(CFLAGS) $^ -o $@

# Pattern rule supporting nested directories
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
