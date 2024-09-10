#define _GNU_SOURCE 
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
 

struct dirData{
    char* name;

};


void outLog(char* directory, bool flagA, bool flagL){
    DIR *dp;
    struct dirent *dent;
    if ((dp = opendir(directory)) == NULL) {
        fprintf(stderr, "opendir: %s: %s\n", directory, strerror(errno));
    }

    int total = 0;
    struct dirData* data = calloc(1, sizeof(struct dirData));
    int size = 0;
    while ((dent = readdir(dp)) != NULL) {
        data = realloc(data, (size * sizeof(struct dirData)) + (1 * sizeof(struct dirData)));
        total += dent->d_type;
        struct dirData d;
        d.name = dent->d_name;
        data[size] = d;
        size += 1;
    };

    printf("total %i\n", total);
    for (int i = 0; i < size; i++){
        printf("%s\n", data[i].name);
    }

    free(data);
    closedir(dp);
}


int main (int argc, char** argv) {
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
		case '?': printf("ls: invalid option\n"); break;
		} 
	}

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
    
    for (int i = 0; i < size; i++){
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