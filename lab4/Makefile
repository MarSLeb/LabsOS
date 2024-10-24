.PHONY: build clean

build: chmod.o
	gcc -o mychmod chmod.o 
chmod.o: chmod.c
	gcc -c chmod.c 
clean:
	rm *.o mychmod