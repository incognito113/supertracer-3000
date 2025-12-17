# Compilation flags based on mode
MODE ?= fast
BUILD_DIR = build

# Detect macOS
UNAME_S := $(shell uname -s)
IS_MAC := $(filter Darwin,$(UNAME_S))

# Select compiler based on OS
ifeq ($(IS_MAC),Darwin)
CC := clang++
else
CC := g++
endif

# Compiler and linker flags
SDL_FLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs)

DEBUG_FLAGS = -g -Wall -Werror -Wextra -Wpedantic -fsanitize=address -fsanitize=undefined -MMD -MP -std=c++23 -I. $(SDL_FLAGS)
OPTIMIZED_FLAGS = -O3 -march=native -funroll-loops -fstrict-aliasing -flto -ffast-math -fno-math-errno -fomit-frame-pointer -MMD -MP -std=c++17 -I. $(SDL_FLAGS)

# Choose flags based on mode
CFLAGS := $(if $(filter debug,$(MODE)),$(DEBUG_FLAGS),$(OPTIMIZED_FLAGS))

# Linker flags
LDFLAGS := $(if $(filter debug,$(MODE)),-fsanitize=address -fsanitize=undefined,)
LDFLAGS += $(SDL_LDFLAGS)

# Source files: find all .cpp and .mm files
SRCS_CPP := $(shell find . -type f -name '*.cpp' -print)

# Convert sources to build/*.o with mirrored directory structure
OBJS := $(patsubst ./%.cpp,$(BUILD_DIR)/%.o,$(SRCS_CPP)) \
				$(patsubst ./%.mm,$(BUILD_DIR)/%.o,$(SRCS_MM))

# Object files for main and test targets
MAIN_OBJ := $(BUILD_DIR)/main.o
TEST_OBJ := $(BUILD_DIR)/test.o
OTHER_OBJS := $(filter-out $(MAIN_OBJ) $(TEST_OBJ), $(OBJS))

OBJS_MAIN := $(MAIN_OBJ) $(OTHER_OBJS)
OBJS_TEST := $(TEST_OBJ) $(OTHER_OBJS)

DEPS := $(OBJS:.o=.d)

.PHONY: all clean
all: $(BUILD_DIR)/main $(BUILD_DIR)/test

$(BUILD_DIR)/main: $(OBJS_MAIN)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS_MAIN) -o $@ $(LDFLAGS)

$(BUILD_DIR)/test: $(OBJS_TEST)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS_TEST) -o $@ $(LDFLAGS)

# Generic compile rule for .cpp -> build/%.o
$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

main: $(BUILD_DIR)/main
	./$(BUILD_DIR)/main

test: $(BUILD_DIR)/test
	./$(BUILD_DIR)/test

leaks-main: $(BUILD_DIR)/main
	leaks --atExit -- ./$(BUILD_DIR)/main

leaks-test: $(BUILD_DIR)/test
	leaks --atExit -- ./$(BUILD_DIR)/test

clean:
	rm -rf $(BUILD_DIR)
	rm -f *.ppm
