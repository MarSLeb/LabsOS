#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h> 
#include <stdlib.h>
#include <stdbool.h>

void outLog (char* filename, int* count, bool* beforLineHaveEnter, 
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
            printf("    %5i  ", *count);
            (*count)++;
        }
        if (flagN && !flagB && *beforLineHaveEnter){
            printf("    %5i  ", *count);
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

    char** files = calloc(argc - optind, sizeof(char*));
    int countFile = 0;
    while (optind < argc){ // подсчет и сохраниение всех файлов для чтения
        files[countFile] = calloc(strlen(argv[optind]) + 1, sizeof(char));
        strcpy(files[countFile], argv[optind]);
        countFile++;
        optind++;
    }

    int nowCount = 1;
    bool beforLineHaveEnter = true;
    for (int i = 0; i < countFile; i++){
        outLog(files[i], &nowCount, &beforLineHaveEnter, 
               flagB, flagN, flagE, i == countFile - 1);
        if (nowCount == -1){
            return 1;
        }
    }

    if (countFile == 0){
        int count = 1;
        char *line = NULL;
        size_t len;
        while(getline(&line, &len, stdin) != -1) {
            line[strlen(line) - 1] = '\0';
            if (flagB && strcmp(line, "")){
                printf("    %5i  ", count);
                count++;
            }
            if (flagN && !flagB){
                printf("    %5i  ", count);
                count++;
            }
            printf("%s", line);
            if (flagE){
                printf("$");
            }
            printf("\n");
        }
        free(line);
    }

    for (int i = 0; i < countFile; i++){
        free(files[i]);
    }
    free(files);

    return 0;
}