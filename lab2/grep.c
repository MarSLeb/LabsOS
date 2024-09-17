#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <regex.h>

#define RESET      "\033[0m"
#define RED        "\033[1;31m"

//испльзовать регекс

int search(char* text, regex_t regex){
    char res[512];
    size_t n = 256;
    regmatch_t p[256] = {};
    int err = regexec(&regex, text, n, p, 0);
    int step = 0;
    while (!err){
        for (int i = 0; i < n; i++){
            if (p[i].rm_eo == -1){
                break;
            }
            memcpy(res, text + step, p[i].rm_so);
            res[p[i].rm_so] = '\0';
            printf("%s%s", res, RED);
            memcpy(res, text + step + p[i].rm_so, p[i].rm_eo - p[i].rm_so);
            res[p[i].rm_eo - p[i].rm_so] = '\0';
            printf("%s%s", res, RESET);
            
            step += p[i].rm_eo;
        }
        if (step > strlen(text)){
            return 1;
        }
        err = regexec(&regex, text + step, n, p, 0);
    }
    if (err == REG_NOMATCH){
        memcpy(res, text + step, strlen(text) - step + 1);
        res[strlen(text) - step + 1] = '\0';
        if (res[strlen(res) - 1] != '\n'){
            res[strlen(res) + 1] = '\0';
            res[strlen(res)] = '\n';
        }
        printf("%s", res);
        return 1;
    }
    else{
        char msgbuf[100];
        regerror(err, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        return -1;
    }
}

int outLog (char* filename, regex_t regex, bool manyFile){
    FILE* file = fopen(filename, "r");
    if (!file && filename != NULL){
        fprintf(stderr, "./mygrep: %s: No such file or directory\n", filename);
        return -1;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, file)) != -1){
        size_t n = 256;
        regmatch_t p[256] = {};
        int err = regexec(&regex, line, n, p, 0);
        if (manyFile && !err){
            printf("%s: ", filename);
            search(line, regex);
        }
        else{
            search(line, regex);
        }
    }
    free(line);
    fclose(file);
    return 1;
}

int main(int argc, char** argv){
    if (argc < 2){
        fprintf(stderr, "Usage: grep PATTERNS [FILE]...\n");
        return 1;
    }

    regex_t regex;
    char* reg = calloc(strlen(argv[1]) + 2, sizeof(char));
    strcpy(reg + (argv[1][0] == '\"'), argv[1]);
    if (reg[strlen(reg) - 1] == '\"'){
        reg[strlen(reg) - 1] = '\0';
    }
    int err = 0;
    if ((err = regcomp(&regex, reg, REG_EXTENDED)) != 0){
        char errbuf[256];
        regerror(err, &regex, errbuf, 256);
        fprintf(stderr, "./mygrep: %s\n", errbuf);
        return 1;
    }
    free(reg);

    if (argc > 2){
        int i = 2;
        bool manyFile = argc > 3;
        while (i < argc){
            int err;
            if ((err = outLog(argv[i], regex, manyFile)) == -1){
                return 1;
            }
            i++;
        }
    }
    else{
        char *line = NULL;
        size_t len;
        while(getline(&line, &len, stdin) != -1) {
            search(line, regex);
        }
        free(line);
    }
    regfree(&regex);

    return 0;
}