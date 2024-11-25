SRC_DIRS := client common server
BUILD_DIR := build

CFLAGS := -O3 -pedantic -Wall -std=c++20
CC := g++

SOURCE := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)) $(addsuffix /*.cpp, $(SRC_DIRS)))
HEADER := $(wildcard $(addsuffix /*.h, $(SRC_DIRS)) $(addsuffix /*.hpp, $(SRC_DIRS)))

all: $(BUILD_DIR)/server $(BUILD_DIR)/client
	

$(BUILD_DIR)/server: $(wildcard server/*) $(wildcard common/*) $(BUILD_DIR)
	$(CC) $(CFLAGS) server/main.cpp -o $@ -I.

$(BUILD_DIR)/client: $(wildcard client/*) $(wildcard common/*) $(BUILD_DIR)
	$(CC) $(CFLAGS) client/main.cpp -o $@ -I.

tidy: $(SOURCE) $(HEADER)
	clang-tidy $^ -- -I.

format: $(SOURCE) $(HEADER)
	clang-format -i $^

$(BUILD_DIR):
	mkdir $@

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/server $(BUILD_DIR)/client

.PHONY: all tidy clean
	
