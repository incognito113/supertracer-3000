CC = g++
CFLAGS = -g -Wall -Wextra -Werror -MMD -MP
BUILD_DIR = build
TARGET = main

# Source files
SRCS = main.cpp color.cpp plane.cpp shape.cpp sphere.cpp vector.cpp
OBJS = $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

# Default target
all: main

# Build the executable
build: $(BUILD_DIR)/$(TARGET)

$(BUILD_DIR)/$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Pattern rule for object files
$(BUILD_DIR)/%.o: %.cpp | build_dir
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

# Run the program
main: build
	./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: build_dir clean main build
build_dir:
	mkdir -p $(BUILD_DIR)
