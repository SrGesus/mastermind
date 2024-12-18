SRC_DIRS := client common server
BUILD_DIR := build

CFLAGS := -pedantic -Wall -std=c++20
CC := g++

SOURCE := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)) $(addsuffix /*.cpp, $(SRC_DIRS)))
HEADER := $(wildcard $(addsuffix /*.h, $(SRC_DIRS)) $(addsuffix /*.hpp, $(SRC_DIRS)))

all: $(BUILD_DIR)/GS $(BUILD_DIR)/player

$(BUILD_DIR)/GS: $(wildcard server/*) $(wildcard common/*)
	$(CC) $(CFLAGS) server/main.cpp -o $@ -I.

$(BUILD_DIR)/player: $(wildcard client/*) $(wildcard common/*)
	$(CC) $(CFLAGS) client/main.cpp -o $@ -I.

tidy: $(SOURCE) $(HEADER)
	clang-tidy $^ -- -I.

format: $(SOURCE) $(HEADER)
	clang-format -i $^

$(BUILD_DIR):
	mkdir $@

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/GS $(BUILD_DIR)/client

