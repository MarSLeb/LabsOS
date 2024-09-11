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
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <ctype.h>

#define RESET      "\033[0m"
#define BLUE       "\033[1;34m"
#define GREEN      "\033[1;32m"
#define TURQUOISE  "\033[1;36m"

char* decodePermissions(char* str, unsigned long num){
    *str = num & 0b100 ? 'r' : '-';
    str++;
    *str = num & 0b010 ? 'w' : '-';
    str++;
    *str = num & 0b001 ? 'x' : '-';
    str++;

    return str;
}

int lenInt(int num){
    int len = 0;
    while (num % 10 != 0){
        num /= 10;
        len++;
    }
    return len;
}

void cuteOut(char* name, struct stat data, bool flagL, int lenUser, int lenGroup, int lenSize){
    char* color = RESET;
    char mode[11];
    mode[0] = !strcmp(color,BLUE) ? 'd' : '-';
    char* pointer = &mode[1];
    pointer = decodePermissions(pointer, data.st_mode >> 6);
    pointer = decodePermissions(pointer, data.st_mode >> 3);
    pointer = decodePermissions(pointer, data.st_mode);

    if (S_ISREG(data.st_mode)) { // файл
        if (mode[3] == 'x' || mode [6] == 'x' || mode[9] == 'x'){ // исполняемый
            color = GREEN;
        }
    }
    if (S_ISDIR(data.st_mode)) { // директория
        color = BLUE;
    }
    if (S_ISLNK(data.st_mode)) { // ссылка
        color = TURQUOISE;
    }
    
    if (flagL){
        //представим, что getpwuid и getgrgid не могут выдать NULL
        char* user = getpwuid(data.st_uid)->pw_name; 
        char* group = getgrgid(data.st_gid)->gr_name;

        char* time = ctime(&data.st_mtime);
        memcpy(time, time + 4, strlen(time) - 4);
        time[strlen(time) - 9 - 4] = '\0';

        printf("%s ", mode);
        printf("%li ", data.st_nlink);
        printf("%*s ", lenUser, user);
        printf("%*s ", lenGroup, group);
        printf("%*li ", lenSize, data.st_size);
        printf("%s ", time);
        printf("%s%s%s\n", color, name, RESET);
    }
    else{
        printf("%s%s%s  ", color, name, RESET);
    }
}

char* toLower(const char* str){
    char* lower = calloc(strlen(str), sizeof(char));
    for(int i = 0; i < strlen(str); i++){
        lower[i] = tolower(str[i]);
    }
    return lower;
}

int charCmp(const void* x, const void* y)
{   
	return strcmp((toLower(*(char**)x)), (toLower(*(char**)y)));
}

void outLog(const char* directory, const bool flagA, const bool flagL){
    //проверка наличия директории
    DIR* dp;
    struct dirent* dent;
    if ((dp = opendir(directory)) == NULL) {
        fprintf(stderr, "opendir: %s: %s\n", directory, strerror(errno));
    }

    char** names = calloc(1, sizeof(char*));
    int countNames = 0;
    while ((dent = readdir(dp)) != NULL) {
        names = realloc(names, (countNames + 1) * sizeof(char*));
        names[countNames] = dent->d_name;
        countNames += 1;
    }; 
    qsort(names, countNames, sizeof(char*), charCmp);

    //определение сколько блоков памяти занимает директория
    int total = 0;
    struct stat* data = calloc(countNames, sizeof(struct stat));
    for (int i = 0; i < countNames; i++){
        char* path = calloc(strlen(directory) + strlen(names[i]), sizeof(char));
        strcpy(path, directory);
        memcpy(path + strlen(path), names[i], strlen(names[i]));
        stat(path, &data[i]);
        free(path);
        if (!flagA && names[i][0] == '.'){
            continue;
        }
        total += data[i].st_blocks;
    }

    //проход для определения ширины столбцов
    int maxLenUser = 0, maxLenGroup = 0, maxSize = 0;
    for (int i = 0; i < countNames; i++){
        int lenUser = strlen(getpwuid(data[i].st_uid)->pw_name); 
        int lenGroup = strlen(getgrgid(data[i].st_gid)->gr_name);
        maxLenUser = maxLenUser < lenUser ? lenUser : maxLenUser;
        maxLenGroup = maxLenGroup < lenGroup ? lenGroup : maxLenGroup;
        maxSize = maxSize < data[i].st_size ? data[i].st_size : maxSize;
    }

    //проход для вывода
    if (flagL){
        printf("total %i\n", total / 2); //сделано методом тыка и надо понять почему так
        //upd:
        // stat думает, что блоки занимают 512, a ls думает, что блоки по 1024
        // и не важно что на самом деле в системе....
    }
    for (int i = 0; i < countNames; i++){
        if (!flagA && names[i][0] == '.'){
            continue;
        }
        cuteOut(names[i], data[i], flagL, maxLenUser, maxLenGroup, lenInt(maxSize));
    }
    if (!flagL){
        printf("\n");
    }
    
    free(data);
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
        directories = realloc(directories, (size + 1) * sizeof(char*));
        directories[size] = calloc(strlen(argv[optind]) + 1, sizeof(char));
        strcpy(directories[size],argv[optind]);
        size += 1;
        optind += 1;
    }
    if (size == 0){
        directories[0] = calloc(strlen(".") + 1, sizeof(char));
        strcpy(directories[0], ".");
        size += 1;
    }
    
    //вывод
    for (int i = 0; i < size; i++){
        if (directories[i][strlen(directories[i])] != '/'){
            directories[i] = realloc(directories[i], (strlen(directories[i]) + 1) * sizeof(char));
            memcpy(directories[i] + strlen(directories[i]), "/", 1);
        }
        if (size != 1){
            printf("%s:\n", directories[i]);
        }
        outLog(directories[i], flagA, flagL);
        if (size != 1){
            printf("\n");
        }
    }

    free(directories);
    
    return 0;
}