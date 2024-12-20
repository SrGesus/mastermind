SRC_DIRS := client common server

CFLAGS := -pedantic -Wall -std=c++20
CC := g++

SOURCE := $(wildcard $(addsuffix /*.c, $(SRC_DIRS)) $(addsuffix /*.cpp, $(SRC_DIRS)))
HEADER := $(wildcard $(addsuffix /*.h, $(SRC_DIRS)) $(addsuffix /*.hpp, $(SRC_DIRS)))

all: GS player

GS: $(wildcard server/*) $(wildcard common/*)
	$(CC) $(CFLAGS) server/main.cpp -o $@ -I.

player: $(wildcard client/*) $(wildcard common/*)
	$(CC) $(CFLAGS) client/main.cpp -o $@ -I.

tidy: $(SOURCE) $(HEADER)
	clang-tidy $^ -- -I.

format: $(SOURCE) $(HEADER)
	clang-format -i $^

clean:
	rm -f *.o GS client

