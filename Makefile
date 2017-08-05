CC = g++
CFLAGS = -std=c++14 -ggdb -Wall -Wextra -pedantic
LDFLAGS = -lsfml-graphics -lsfml-window -lsfml-system -lpthread -lX11

all: main

main: main.o Terrain.o Physics.o Renderer.hpp
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o: %.cpp Terrain.hpp Physics.hpp Renderer.hpp utils.hpp
	$(CC) $(CFLAGS) $< -c

.PHONY: clean
clean:
	rm -f *.o main
