.PHONY: build clean

build: read.o write.o
	gcc -o read  read.o
	gcc -o write write.o 
read.o: read.c
	gcc -c read.c 
write.o: write.c
	gcc -c write.c 
clean:
	rm *.o read write