SRC_DIRS := client common server

SOURCE := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)) $(addsuffix /*.cpp, $(SRC_DIRS)))
HEADER := $(wildcard $(addsuffix /*.h, $(SRC_DIRS)) $(addsuffix /*.hpp, $(SRC_DIRS)))

tidy: $(SOURCE) $(HEADER)
	clang-tidy $^ -- -Icommon -Iserver -Iclient

format: $(SOURCE) $(HEADER)
	clang-format -i $^

.PHONY: all tidy clean
