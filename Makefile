CC = g++
CFLAGS = -std=c++14 -ggdb -Wall -Wextra -pedantic
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system

all: main

main: main.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp Terrain.hpp
	$(CC) $(CFLAGS) $< -c

.PHONY: clean
clean:
	rm -f *.o main
