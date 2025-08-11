CC := gcc
CFLAGS := -std=c++17 -Wall -Wextra -I./include -fPIC -x c++
LDFLAGS := -shared -fPIC -lstdc++ -pthread
TARGET_LIB := liblogger.so
TARGET_APP := logger_app
BUILD_DIR := build

SRC_LIB := src/Part1.cpp
SRC_APP := src/Part2.cpp

DEFAULT_FILENAME := log.txt
DEFAULT_LEVEL := middle

ifndef VERBOSE
.SILENT:
endif

.PHONY: all clean run

all: $(BUILD_DIR)/$(TARGET_LIB) $(BUILD_DIR)/$(TARGET_APP)

$(BUILD_DIR)/$(TARGET_LIB): $(SRC_LIB) 
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

$(BUILD_DIR)/$(TARGET_APP): $(SRC_APP) | $(BUILD_DIR)/$(TARGET_LIB)
	$(CC) $(CFLAGS) $^ -o $@ -L$(BUILD_DIR) -llogger -pthread -lstdc++

run: $(BUILD_DIR)/$(TARGET_APP)
	LD_LIBRARY_PATH=$(BUILD_DIR) ./$(BUILD_DIR)/$(TARGET_APP) $(or $(word 2, $(MAKECMDGOALS)), $(DEFAULT_FILENAME)), $(or $(word 3, $(MAKECMDGOALS)), $(DEFAULT_LEVEL))

%:
	@:

clean:
	rm -rf $(BUILD_DIR)
