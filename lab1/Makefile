.PHONY: build clean

build: main.o
	gcc -o myls main.o
main.o: main.c
	gcc -c main.c
clean:
	rm *.o myls