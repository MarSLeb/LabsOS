#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>
#include <stdbool.h>

int lenInt(int num){
    int len = 0;
    while (num != 0){
        num /= 10;
        len++;
    }
    return len;
}
int sizeCount (char* filename, bool flagB, bool flagN){
    int count = 1;
    FILE* file = fopen(filename, "r");
    if (!file && filename != NULL){
        fprintf(stderr, "./mycat: %s: No such file or directory\n", filename);
        return -1;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1) {
        if (flagB && strcmp(line, "")){
            count++;
        }
        if (flagN && !flagB){
            count++;
        }
    }
    free(line);
    fclose(file);
    return lenInt(count);
}

int outLog (char* filename, int sizeCount, bool flagB, bool flagN, bool flagE){
    FILE* file = fopen(filename, "r");
    if (!file && filename != NULL){
        fprintf(stderr, "./mycat: %s: No such file or directory\n", filename);
        return -1;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    int count = 1;
    while ((read = getline(&line, &len, file)) != -1) {
        bool lastLine = true;
        if (line[strlen(line) - 1] == '\n'){
            line[strlen(line) - 1] = '\0';
            lastLine = false;
        }
        if (flagB && strcmp(line, "")){
            printf("    %*i  ", sizeCount, count);
            count++;
        }
        if (flagN && !flagB){
            printf("    %*i  ", sizeCount, count);
            count++;
        }
        printf("%s", line);
        if (flagE){
            printf("$");
        }
        if (!lastLine){
            printf("\n");
        }
    }
    free(line);
    fclose(file);
    return 0;
}

int main(int argc, char** argv){
    int rez = 0;
    bool flagN = false;
    bool flagB = false;
    bool flagE = false;
	while ((rez = getopt(argc, argv, "nbE")) != -1){
		switch (rez) {
		case 'n': { //нумерует все строки выходного файла, начиная с 1 
            flagN = true;
            break;
        }
		case 'b': { //нумерует все непустые строки выходного файла, начиная c 1 
            flagB = true;
            break;
        }
        case 'E': { //выводит в конце каждой строки символ $ 
            flagE = true;
            break;
        }
		case '?': {
            return 1;
            break;
        }
		} 
	}
    char* filename = NULL;
    int countFile = 0;
    int firstFile = optind;
    while (optind < argc) {
        filename = calloc(strlen(argv[optind]) + 1, sizeof(char));
        strcpy(filename, argv[optind]);
        countFile++;
        int size = sizeCount(filename, flagB, flagN);
        if (size == -1) {
            return 1;
        }
        outLog(filename, size, flagB, flagN, flagE);
        optind++;
    }
    if (countFile == 0){
        puts("-- no --");
    }
    free(filename);
    return 0;
}