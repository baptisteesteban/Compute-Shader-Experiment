all: compute

compute: main.o
	g++ -o $@ $< -lGL -lSDL2 -lGLEW

main.o: main.cc
	g++ -o $@ -c $< -Wall -Wextra -Werror -std=c++17

clean:
	${RM} main.o compute *~

.PHONY: compute clean main.o
