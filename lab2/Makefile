.PHONY: build clean

build: cat.o grep.o
	gcc -o mycat cat.o
	gcc -o mygrep grep.o
cat.o: cat.c
	gcc -c cat.c -Wall -Werror
grep.o: grep.c
	gcc -c grep.c -Wall -Werror
clean:
	rm *.o mycat mygrep