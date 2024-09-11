.PHONY: build clean

build: main.o
	gcc -o myls main.o
main.o: main.c
	gcc -c main.c -Wall -Werror
clean:
	rm *.o myls