# Detect compiler
CLANG := $(shell command -v clang++ 2>/dev/null)
GCC := $(shell command -v g++ 2>/dev/null)

ifeq ($(CLANG),)
ifneq ($(GCC),)
CC := $(GCC)
else
$(error No suitable C++ compiler found (clang++ or g++))
endif
else
CC := $(CLANG)
endif

# Compilation flags based on mode
MODE ?= fast

BUILD_DIR = build

# Detect macOS
UNAME_S := $(shell uname -s)
IS_MAC := $(filter Darwin,$(UNAME_S))

# Try to find metal and metallib tools
METAL := $(shell xcrun --find metal 2>/dev/null || echo)
METALLIB := $(shell xcrun --find metallib 2>/dev/null || echo)

# Flag to check if metal is available
ifeq ($(IS_MAC),Darwin)
ifneq ($(METAL),)
USE_METAL := 1
else
USE_METAL := 0
endif
else
USE_METAL := 0
endif

ifeq ($(IS_MAC),Darwin)
# Metal C++ headers location
METAL_CPP_DIR := /Library/Developer/metal-cpp
METAL_FLAGS := -I$(METAL_CPP_DIR)

# Install metal-cpp if not present
install-metal-cpp:
	@if [ ! -d "$(METAL_CPP_DIR)" ]; then \
		echo "metal-cpp not found. Installing to $(METAL_CPP_DIR)..."; \
		TMP_DIR=$$(mktemp -d); \
		curl -L -o "$$TMP_DIR/metal-cpp.zip" https://developer.apple.com/metal/cpp/files/metal-cpp_26.zip; \
		unzip -q "$$TMP_DIR/metal-cpp.zip" -d "$$TMP_DIR"; \
		sudo mkdir -p $(METAL_CPP_DIR); \
		sudo cp -r "$$TMP_DIR/metal-cpp/"* $(METAL_CPP_DIR)/; \
		rm -rf "$$TMP_DIR"; \
		echo "metal-cpp installed successfully."; \
	else \
		echo "metal-cpp already installed."; \
	fi
else
METAL_FLAGS :=
install-metal-cpp: ;
endif

SDL_FLAGS = $(shell sdl2-config --cflags)
SDL_LDFLAGS = $(shell sdl2-config --libs)
DEBUG_FLAGS = -g -Wall -Wextra -Werror -fsanitize=address -fsanitize=undefined -MMD -MP -std=c++23 -I. $(SDL_FLAGS) $(METAL_FLAGS)
OPTIMIZED_FLAGS = -O3 -march=native -funroll-loops -fstrict-aliasing -flto -ffast-math -fno-math-errno -fomit-frame-pointer -MMD -MP -std=c++23 -I. $(SDL_FLAGS) $(METAL_FLAGS)

CFLAGS = $(if $(filter debug,$(MODE)),$(DEBUG_FLAGS),$(OPTIMIZED_FLAGS))
LDFLAGS = $(if $(filter debug,$(MODE)),-fsanitize=address -fsanitize=undefined,)
LDFLAGS += $(SDL_LDFLAGS) -framework MetalKit -framework Metal -framework Foundation

# Shader build
SHADERS := $(shell find shaders -type f -name '*.metal')
AIR_FILES := $(patsubst %.metal,$(BUILD_DIR)/%.air,$(SHADERS))
METAL_LIB := $(BUILD_DIR)/shaders/Shader.metallib

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

# When Metal is enabled we need to ensure headers exist before building
ifeq ($(USE_METAL),1)
OBJ_PREREQ := install-metal-cpp
.PHONY: install-metal-cpp
else
OBJ_PREREQ :=
endif

$(BUILD_DIR)/main: $(OBJS_MAIN) $(METAL_LIB)
	$(CC) $(OBJS_MAIN) -o $@ $(LDFLAGS)

$(BUILD_DIR)/test: $(OBJS_TEST) $(METAL_LIB)
	$(CC) $(OBJS_TEST) -o $@ $(LDFLAGS)

# Compile .metal -> .air
$(BUILD_DIR)/%.air: %.metal
	mkdir -p $(dir $@)
	$(METAL) -c $< -o $@

# Link .air -> .metallib
$(METAL_LIB): $(AIR_FILES)
	mkdir -p $(dir $@)
	$(METALLIB) $^ -o $@

# Pattern rule supporting nested directories, install metal-cpp if needed
$(BUILD_DIR)/%.o: %.cpp $(OBJ_PREREQ)
	mkdir -p $(dir $@)
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

.PHONY: all main test leaks-main leaks-test install-metal-cpp clean
