#define _GNU_SOURCE 
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>



void outLog(const char* directory, const bool flagA, const bool flagL){
    //проверка наличия директории
    DIR* dp;
    struct dirent* dent;
    if ((dp = opendir(directory)) == NULL) {
        fprintf(stderr, "opendir: %s: %s\n", directory, strerror(errno));
    }

    //запоминаем все имена (для вывода) и пути к ним(для проверки данных)
    char** names = calloc(1, sizeof(char*));
    char** paths = calloc(1, sizeof(char*));
    int countNames = 0;

    while ((dent = readdir(dp)) != NULL) {
        names = realloc(names, countNames * sizeof(char*) + (1 * sizeof(char*)));
        paths = realloc(paths, countNames * sizeof(char*) + (1 * sizeof(char*)));
        names[countNames] = dent->d_name;

        struct stat stat;
        if (directory[strlen(directory)] != '/'){
            paths[countNames] = calloc(strlen(directory) + 1 + strlen(names[countNames]), sizeof(char));
            memcpy(paths[countNames], directory, strlen(directory));
            memcpy(paths[countNames] + strlen(directory), "/", 1);
            memcpy(paths[countNames] + strlen(directory) + 1, names[countNames], strlen(names[countNames]));
        } 
        else{
            paths[countNames] = calloc(strlen(directory) + strlen(names[countNames]), sizeof(char));
            memcpy(paths[countNames], directory, strlen(directory));
            memcpy(paths[countNames] + strlen(directory), names[countNames], strlen(names[countNames]));
        }

        countNames += 1;
    }; 

    // тут должна быть сортировка 

    int total = 0;
    struct stat* data = calloc(countNames, sizeof(struct stat));
    for (int i = 0; i < countNames; i++){
        stat(paths[i], &data[i]);
        total += data[i].st_blocks;
    }

    if (flagL){
        printf("total %i\n", total);
    }
    for (int i = 0; i < countNames; i++){
        if (!flagA && (!strcmp(names[i], ".") || !strcmp(names[i], ".."))){
            continue;
        }
        printf("%s\n", names[i]);
    }
    

    //for (int i = 0; i < countNames; i++){
    //    free(names[i]);
    //    free(paths[i]);
    //}
    free(paths);
    free(names);
    closedir(dp);
} 



int main (int argc, char** argv) {
    //обработка наличия флагов
    int rez = 0;
    bool flagA = false, flagL = false; 
	while ((rez = getopt(argc, argv, "al")) != -1){
		switch (rez) {
		case 'a': {
            flagA = true;
            break;
        }
		case 'l': {
            flagL = true;
            break;
        }
		case '?': {
            printf("ls: invalid option\n"); 
            break;
        }
		} 
	}

    //обаботка поданынх директорий для простмотра
    char** directories = calloc(1, sizeof(char*));
    int size = 0;
    while (optind < argc) {
        directories = realloc(directories, (size * sizeof(char*)) + (1 * sizeof(char*)));
        directories[size] = argv[optind];
        size += 1;
        optind += 1;
    }
    if (size == 0){
        directories[0] =  ".";
        size += 1;
    }
    
    //вывод
    for (int i = 0; i < size; i++){
        if (size != 1){
            printf("%s:\n", directories[i]);
        }
        outLog(directories[i], flagA, flagL);
        if (size != 1){
            printf("\n");
        }
        //free(directories[i]);
    }

    free(directories);
    
    return 0;
}