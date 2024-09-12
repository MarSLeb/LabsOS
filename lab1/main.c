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

struct linkOrFile{
    struct stat file;
    char* linkname;
    struct stat linkdata;
};

struct typeData{
    char* color;
    char mode[11];
};

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
    while (num != 0){
        num /= 10;
        len++;
    }
    return len;
}

struct typeData colorAndMode(struct stat data){
    struct typeData res;
    char* color = calloc(sizeof(TURQUOISE)/sizeof(char) + 1, sizeof(char));
    res.color = calloc(sizeof(TURQUOISE)/sizeof(char) + 1, sizeof(char));
    strcpy(color, RESET);
    char mode[11];
    mode[0] = '-';
    char* pointer = &mode[1];
    pointer = decodePermissions(pointer, data.st_mode >> 6);
    pointer = decodePermissions(pointer, data.st_mode >> 3);
    pointer = decodePermissions(pointer, data.st_mode);
    if (S_ISREG(data.st_mode)) { // файл
        if (mode[3] == 'x' || mode [6] == 'x' || mode[9] == 'x'){ // исполняемый
            strcpy(color, GREEN);
        }
    }
    if (S_ISDIR(data.st_mode)) { // директория
        strcpy(color, BLUE);
        mode[0] = 'd';
    }
    if (S_ISLNK(data.st_mode)) { // ссылка
        strcpy(color, TURQUOISE);
        mode[0] = 'l';
    }
    mode[10] = '\0';
    strcpy(res.color, color);
    strcpy(res.mode, mode);
    free(color);
    return res;
}

bool isHaveSpace(char* str){
    for (int i = 0; i < strlen(str); i++){
        if (str[i] == ' '){
            return true;
        }
    }
    return false;
}

void cuteOut(char* linkname, struct stat linkdata, char* name, struct stat data, bool flagL, int lenUser, int lenGroup, int lenSize, int lenNLinks){
    struct typeData d = colorAndMode(data);
    char* color = d.color;
    char mode[11];
    strcpy(mode, d.mode);
    
    if (flagL){
        //представим, что getpwuid и getgrgid не могут выдать NULL
        char* user = calloc(strlen(getpwuid(data.st_uid)->pw_name) + 1, sizeof(char));
        strcpy(user, getpwuid(data.st_uid)->pw_name); 
        char* group = calloc(strlen(getgrgid(data.st_gid)->gr_name) + 1, sizeof(char));
        strcpy(group, getgrgid(data.st_gid)->gr_name);

        char* time = calloc(strlen(ctime(&data.st_mtime)) + 1, sizeof(char));
        strcpy(time, ctime(&data.st_mtime));
        int s = strlen(time);
        memcpy(time, time + 4, s - 4);
        time[s - 9 - 4] = '\0';
        
        printf("%s ", mode);
        printf("%*li ", lenNLinks, data.st_nlink);
        printf("%*s ", -lenUser, user);
        printf("%*s ", -lenGroup, group);
        printf("%*li ", lenSize, data.st_size);
        printf("%s ", time);
        if (isHaveSpace(name)){
            printf("%s'%s'%s ", color, name, RESET);
        }
        else{
            printf(" %s%s%s ", color, name, RESET);
        }
        if (!strcmp(color, TURQUOISE)){
            struct typeData l = colorAndMode(linkdata);
            if (isHaveSpace(linkname)){
                printf("-> %s'%s'%s", l.color, linkname, RESET);
            }
            else{
                printf("-> %s%s%s", l.color, linkname, RESET);
            }
            free(l.color);
        }
        printf("\n");

        free(user);
        free(group);
        free(time);
    }
    else{
        printf("%s%s%s  ", color, name, RESET);
    }
    free(color);
}

char* toLower(const char* str){
    char* lower = calloc(strlen(str) + 1, sizeof(char));
    for(int i = 0; i < strlen(str); i++){
        lower[i] = tolower(str[i]);
    }
    lower[strlen(lower)] = '\0';
    return lower;
}

int charCmp(const void* x, const void* y)
{   
    char* first = toLower(*(char**)x);
    char* second = toLower(*(char**)y);
    int res = strcmp(first + (first[0] == '.'), second + (second[0] == '.'));
    free(first);
    free(second);
	return res;
}

void outLog(char* directory, const bool flagA, const bool flagL){
    //проверка наличия директории
    DIR* dp;
    if ((dp = opendir(directory)) == NULL) {
        closedir(dp);
        fprintf(stderr, "./myls: cannot access '%s': No such file or directory\n", directory);
        errno = 1;
        return;
    }
    
    if (directory[strlen(directory) - 1] != '/'){
        int s = strlen(directory);
        directory = realloc(directory, (s + 2) * sizeof(char));
        directory[s] = '/';
        directory[s + 1] = '\0';
    }
    
    char** names = calloc(1, sizeof(char*));
    int countNames = 0;
    struct dirent* dent;
    while ((dent = readdir(dp)) != NULL) {
        names = realloc(names, (countNames + 1) * sizeof(char*));
        names[countNames] = calloc(strlen(dent->d_name) + 1, sizeof(char));
        strcpy(names[countNames],dent->d_name);
        countNames++;
    }
    qsort(names, countNames, sizeof(char*), charCmp);

    //определение сколько блоков памяти занимает директория
    int total = 0;
    struct linkOrFile data[countNames];
    
    for (int i = 0; i < countNames; i++){
        char* path = calloc(strlen(directory) + strlen(names[i]) + 1, sizeof(char));
        strcpy(path, directory);
        strcpy(path + strlen(path), names[i]);
        lstat(path, &data[i].file);
        data[i].linkname = NULL;
        if (S_ISLNK(data[i].file.st_mode)) { 
            char* linkname = calloc(data[i].file.st_size + 1, sizeof(char));
            int a = readlink(path, linkname, data[i].file.st_size + 1);
            linkname[a] = '\0';
            data[i].linkname = linkname;

            char* linkPath = calloc(strlen(directory) + strlen(linkname) + 1, sizeof(char));
            if(linkname[0] == '/'){
                strcpy(linkPath, linkname);
            }
            else{
                strcpy(linkPath, directory);
                strcpy(linkPath + strlen(linkPath), linkname);
            }
            lstat(linkPath, &data[i].linkdata);
            free(linkPath);
        }
        free(path);
        if (!flagA && names[i][0] == '.'){
            continue;
        }
        total += data[i].file.st_blocks;
        
    }
    
    //проход для определения ширины столбцов
    int maxLenUser = 0, maxLenGroup = 0, maxSize = 0, maxLinks = 0;
    for (int i = 0; i < countNames; i++){
        char* user = calloc(strlen(getpwuid(data[i].file.st_uid)->pw_name) + 1, sizeof(char));
        char* group = calloc(strlen(getgrgid(data[i].file.st_gid)->gr_name) + 1, sizeof(char));
        strcpy(user, getpwuid(data[i].file.st_uid)->pw_name);
        strcpy(group, getgrgid(data[i].file.st_gid)->gr_name);
        int lenUser = strlen(user); 
        int lenGroup = strlen(group);

        maxLenUser = maxLenUser < lenUser ? lenUser : maxLenUser;
        maxLenGroup = maxLenGroup < lenGroup ? lenGroup : maxLenGroup;
        maxSize = maxSize < data[i].file.st_size ? data[i].file.st_size : maxSize;
        maxLinks = maxLinks < data[i].file.st_nlink ? data[i].file.st_nlink : maxLinks;
        free(user);
        free(group);
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
        cuteOut(data[i].linkname, data[i].linkdata, names[i], data[i].file, flagL, maxLenUser, maxLenGroup, lenInt(maxSize), lenInt(maxLinks));
    }
    if (!flagL){
        printf("\n");
    }
    
    for(int i = 0; i < countNames; i++){
        free(names[i]);
        free(data[i].linkname);
    }
    free(dent);
    free(names);
    free(directory);
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
            return 1;
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
        directories[0] = calloc(2, sizeof(char));
        strcpy(directories[0], ".");
        size += 1;
    }
    
    //вывод
    for (int i = 0; i < size; i++){
        if (size != 1){
            printf("%s:\n", directories[i]);
        }
        outLog(directories[i], flagA, flagL);
        if (errno != 0){
            return 1;
        }
        if (size != 1){
            printf("\n");
        }
    }

    free(directories);

    return 0;
}