# Requires metal-cpp headers installed at /Library/Developer/metal-cpp

# Compilation flags based on mode
MODE ?= fast
BUILD_DIR = build

# Detect macOS
UNAME_S := $(shell uname -s)
IS_MAC := $(filter Darwin,$(UNAME_S))

# Try to find metal and metallib tools
METAL := $(shell xcrun --find metal 2>/dev/null || echo)
METALLIB := $(shell xcrun --find metallib 2>/dev/null || echo)

# Header path expected by this project (user must install)
METAL_CPP_DIR := /Library/Developer/metal-cpp

# Require macOS + metal + metallib + metal-cpp headers
USE_METAL := 0
ifeq ($(IS_MAC),Darwin)
ifneq ($(METAL),)
ifneq ($(METALLIB),)
ifneq ($(wildcard $(METAL_CPP_DIR)),)
USE_METAL := 1
endif
endif
endif
endif

# Select compiler based on OS
ifeq ($(IS_MAC),Darwin)
CC := clang++
else
CC := g++
endif

# Compiler and linker flags
SDL_FLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs)

DEBUG_FLAGS = -g -Wall -Wextra -Werror -fsanitize=address -fsanitize=undefined -MMD -MP -std=c++23 -I. $(SDL_FLAGS) $(METAL_FLAGS)
OPTIMIZED_FLAGS = -O3 -march=native -funroll-loops -fstrict-aliasing -flto -ffast-math -fno-math-errno -fomit-frame-pointer -MMD -MP -std=c++23 -I. $(SDL_FLAGS) $(METAL_FLAGS)

# Add metal-cpp include when Metal enabled
ifneq ($(USE_METAL),0)
CFLAGS := $(if $(filter debug,$(MODE)),$(DEBUG_FLAGS),$(OPTIMIZED_FLAGS)) -I$(METAL_CPP_DIR)
else
CFLAGS := $(if $(filter debug,$(MODE)),$(DEBUG_FLAGS),$(OPTIMIZED_FLAGS))
endif

# Linker flags
LDFLAGS := $(if $(filter debug,$(MODE)),-fsanitize=address -fsanitize=undefined,)
LDFLAGS += $(SDL_LDFLAGS)
ifeq ($(USE_METAL),1)
LDFLAGS += -framework MetalKit -framework Metal -framework Foundation -lobjc
endif

# Shaders (only relevant if Metal enabled)
ifeq ($(USE_METAL),1)
SHADERS := $(shell find shaders -type f -name '*.metal')
AIR_FILES := $(patsubst %.metal,$(BUILD_DIR)/%.air,$(SHADERS))
METAL_LIB := $(BUILD_DIR)/shaders/Shader.metallib
else
SHADERS :=
AIR_FILES :=
METAL_LIB :=
endif

# Source files: find all .cpp files
ifeq ($(USE_METAL),1)
# Exclude shaders/metal_dummy.cpp when Metal is enabled
SRCS := $(shell find . -type f -name '*.cpp' ! -name 'metal_dummy.cpp' -print)
else
# Exclude shaders/metal.cpp when Metal is disabled
SRCS := $(shell find . -type f -name '*.cpp' ! -name 'metal.cpp' -print)
endif

# Convert sources to build/*.o with mirrored directory structure
OBJS := $(patsubst ./%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

# Object files for main and test targets
MAIN_OBJ := $(BUILD_DIR)/main.o
TEST_OBJ := $(BUILD_DIR)/test.o
OTHER_OBJS := $(filter-out $(MAIN_OBJ) $(TEST_OBJ), $(OBJS))

OBJS_MAIN := $(MAIN_OBJ) $(OTHER_OBJS)
OBJS_TEST := $(TEST_OBJ) $(OTHER_OBJS)

DEPS := $(OBJS:.o=.d)

.PHONY: all clean
all: $(BUILD_DIR)/main $(BUILD_DIR)/test

$(BUILD_DIR)/main: $(OBJS_MAIN) $(METAL_LIB)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS_MAIN) -o $@ $(LDFLAGS)

$(BUILD_DIR)/test: $(OBJS_TEST) $(METAL_LIB)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS_TEST) -o $@ $(LDFLAGS)

# Metal-specific rules (only defined when USE_METAL=1)
ifeq ($(USE_METAL),1)
# Compile .metal -> .air
$(BUILD_DIR)/%.air: %.metal
	@mkdir -p $(dir $@)
	$(METAL) -c $< -o $@

# Link .air -> .metallib
$(METAL_LIB): $(AIR_FILES)
	@mkdir -p $(dir $@)
	$(METALLIB) $^ -o $@
endif

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
