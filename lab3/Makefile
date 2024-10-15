.PHONY: build clean

build: fork.o
	gcc -o fork fork.o -Wall -Werror
fork.o: fork.c
	gcc -c fork.c -Wall -Werror
clean:
	rm *.o fork