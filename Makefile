SRC_DIRS := client common server
BUILD_DIR := build

SOURCE := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)) $(addsuffix /*.cpp, $(SRC_DIRS)))
HEADER := $(wildcard $(addsuffix /*.h, $(SRC_DIRS)) $(addsuffix /*.hpp, $(SRC_DIRS)))

all: $(BUILD_DIR)/server $(BUILD_DIR)/client
	

$(BUILD_DIR)/server:
	

$(BUILD_DIR)/client:
	

tidy: $(SOURCE) $(HEADER)
	clang-tidy $^ -- -I.

format: $(SOURCE) $(HEADER)
	clang-format -i $^

clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/server $(BUILD_DIR)/client

.PHONY: all tidy clean
	
