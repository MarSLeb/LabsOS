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

void sizeCount(bool* beforLineHaveEnter, int* count, char* filename, bool flagB, bool flagN){
    FILE* file = fopen(filename, "r");
    if (!file && filename != NULL){
        fprintf(stderr, "./mycat: %s: No such file or directory\n", filename);
        *count = -1;
        return;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1){
        if (flagB && strcmp(line, "\n") && strcmp(line, "") && *beforLineHaveEnter){
            (*count)++;
        }
        if (flagN && !flagB && *beforLineHaveEnter){
            (*count)++;
        }
        if (line[strlen(line) - 1] != '\n'){
            *beforLineHaveEnter = false;
        }
        else{
            *beforLineHaveEnter = true;
        }
    }
    free(line);
    fclose(file);
}

void outLog (char* filename, int* count, int sizeCount, bool* beforLineHaveEnter, 
             bool flagB, bool flagN, bool flagE, bool lastFile){
    FILE* file = fopen(filename, "r");
    if (!file && filename != NULL){
        fprintf(stderr, "./mycat: %s: No such file or directory\n", filename);
        *count = -1;
        return;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1){
        bool haveEnter = false;
        if (line[strlen(line) - 1] == '\n'){
            line[strlen(line) - 1] = '\0';
            haveEnter = true;
        }
        if (flagB && strcmp(line, "\n") && strcmp(line, "") && *beforLineHaveEnter){
            printf("    %*i  ", sizeCount, *count);
            (*count)++;
        }
        if (flagN && !flagB && *beforLineHaveEnter){
            printf("    %*i  ", sizeCount, *count);
            (*count)++;
        }
        printf("%s", line);
        if (!haveEnter){
            if (flagE && lastFile){
                printf("$");
            }
            *beforLineHaveEnter = false;
        }
        else{
            if (flagE){
                printf("$");
            }
            printf("\n");
            *beforLineHaveEnter = true;
        }
    }
    free(line);
    fclose(file);
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

    char** files = calloc(1, sizeof(char*));
    int countFile = 0;
    while (optind < argc){ // подсчет и сохраниение всех файлов для чтения
        files[countFile] = calloc(strlen(argv[optind]) + 1, sizeof(char));
        strcpy(files[countFile], argv[optind]);
        countFile++;
        optind++;
    }

    bool beforLineHaveEnter = true;
    int countLine = 0;
    for (int i = 0; i < countFile; i++){
        sizeCount(&beforLineHaveEnter, &countLine, files[i], flagB, flagN);
    }

    int nowCount = 1;
    beforLineHaveEnter = true;
    for (int i = 0; i < countFile; i++){
        outLog(files[i], &nowCount, lenInt(countLine), &beforLineHaveEnter, 
               flagB, flagN, flagE, i == countFile - 1);
        if (nowCount == -1){
            return 1;
        }
    }
    if (countFile == 0){
        puts("-- no --");
    }

    for (int i = 0; i < countFile; i++){
        free(files[i]);
    }
    free(files);

    return 0;
}