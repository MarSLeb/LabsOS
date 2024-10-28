.PHONY: build clean

build: rawr.o
	gcc -o rawr rawr.o -Wall -Werror
rawr.o: rawr.c
	gcc -c rawr.c -Wall -Werror
clean:
	rm *.o rawr