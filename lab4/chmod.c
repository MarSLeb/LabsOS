#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>   
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <regex.h>

bool isNum(char* str){
    for (int i = 0; i < strlen(str); i++){
        if (!isdigit(str[i])){
            return false;
        }
    }
    return true;
}

mode_t compytingMode(mode_t mode, char sep, char group, bool r, bool w, bool x){
    mode_t newMode = mode;
    if (sep == '+'){
        if (group == 'u'){
            newMode |= r ? S_IRUSR : 0;
            newMode |= w ? S_IWUSR : 0;
            newMode |= x ? S_IXUSR : 0;
        }
        if (group == 'g'){
            newMode |= r ? S_IRGRP : 0;
            newMode |= w ? S_IWGRP : 0;
            newMode |= x ? S_IXGRP : 0;
        }
        if (group == 'o'){
            newMode |= r ? S_IROTH : 0;
            newMode |= w ? S_IWOTH : 0;
            newMode |= x ? S_IXOTH : 0;
        }
    }
    else if (sep == '-'){
        if (group == 'u'){
            newMode &= r ? ~S_IRUSR : 511;
            newMode &= w ? ~S_IWUSR : 511;
            newMode &= x ? ~S_IXUSR : 511;
        }
        if (group == 'g'){
            newMode &= r ? ~S_IRGRP : 511;
            newMode &= w ? ~S_IWGRP : 511;
            newMode &= x ? ~S_IXGRP : 511;
        }
        if (group == 'o'){
            newMode &= r ? ~S_IROTH : 511;
            newMode &= w ? ~S_IWOTH : 511;
            newMode &= x ? ~S_IXOTH : 511;
        }
    }
    else if (sep == '='){
        if (group == 'u'){
            newMode = r ? newMode | S_IRUSR : newMode & ~S_IRUSR;
            newMode = w ? newMode | S_IWUSR : newMode & ~S_IWUSR;
            newMode = x ? newMode | S_IXUSR : newMode & ~S_IXUSR;
        }
        if (group == 'g'){
            newMode = r ? newMode | S_IRGRP : newMode & ~S_IRGRP;
            newMode = w ? newMode | S_IWGRP : newMode & ~S_IWGRP;
            newMode = x ? newMode | S_IXGRP : newMode & ~S_IXGRP;
        }
        if (group == 'o'){
            newMode = r ? newMode | S_IROTH : newMode & ~S_IROTH;
            newMode = w ? newMode | S_IWOTH : newMode & ~S_IWOTH;
            newMode = x ? newMode | S_IXOTH : newMode & ~S_IXOTH;
        }
    }
    return newMode;
}

mode_t parsMode(mode_t fileMode, char* str, int sepInd, char sep){
    mode_t newMode = fileMode;
    bool x = false;
    bool w = false;
    bool r = false;
    for (int i = sepInd + 1; i < strlen(str); i++){
        r = r || str[i] == 'r';
        w = w || str[i] == 'w';
        x = x || str[i] == 'x';
    }
    if (sepInd == 0){
        newMode = compytingMode(newMode, sep, 'g', r, w, x);
        newMode = compytingMode(newMode, sep, 'u', r, w, x);
        newMode = compytingMode(newMode, sep, 'o', r, w, x);
    }
    for (int i = 0; i < sepInd; i++){
        if (str[i] == 'a'){
            newMode = compytingMode(newMode, sep, 'g', r, w, x);
            newMode = compytingMode(newMode, sep, 'u', r, w, x);
            newMode = compytingMode(newMode, sep, 'o', r, w, x);
        }
        else{
            newMode = compytingMode(newMode, sep, str[i], r, w, x);
        }
    }
    return newMode;
}

struct bukovki{
    bool r;
    bool w;
    bool x;
};

struct bukovki numToMode(int num){
    struct bukovki buk;
    if (num == 0) {buk.r = false; buk.w = false; buk.x = false;}
    if (num == 1) {buk.r = false; buk.w = false; buk.x = true ;}
    if (num == 2) {buk.r = false; buk.w = true ; buk.x = false;}
    if (num == 3) {buk.r = false; buk.w = true ; buk.x = true ;}
    if (num == 4) {buk.r = true ; buk.w = false; buk.x = false;}
    if (num == 5) {buk.r = true ; buk.w = false; buk.x = true ;}
    if (num == 6) {buk.r = true ; buk.w = true ; buk.x = false;}
    if (num == 7) {buk.r = true ; buk.w = true ; buk.x = true ;}
    return buk;
}

mode_t parsNumMode(mode_t fileMode, char* str){
    mode_t newMode = fileMode;
    char group[3] = {'u', 'g', 'o'};
    for (int i = 0; i < strlen(str); i++){
        struct bukovki buk = numToMode(str[i] - '0');
        newMode = compytingMode(newMode, '=', group[i], buk.r, buk.w, buk.x);
    }
    return newMode;
}

int match(char *string, char *pattern, regex_t *re){
    int status;
    if ((status = regcomp(re, pattern, REG_EXTENDED)) != 0){
        return(status);
    }
    status = regexec(re, string, 0, NULL, 0);
    return(status);
}

int main(int argc, char** argv){
    if (argc < 2 || argc > 3){
        fprintf(stderr, "mychmod: missing operand\n");
        return 1;
    }

    struct stat st;
    if (stat(argv[2], &st) == -1) {
        fprintf(stderr, "mychmod: file is not exist");
        return 1;
    }
    mode_t mode = st.st_mode;

    char* plusMode  = "[guoa]{0,3}\\+[rwx]{1,3}";
    char* minusMode = "[guoa]{0,3}-[rwx]{1,3}";
    char* eqMode    = "[guoa]{0,3}=[rwx]{1,3}";
    char* numMode   = "[0-7]{3}";

    regex_t re; 

    if (!isNum(argv[1])){
        regex_t re;   
        if (match (argv[1], plusMode,  &re) == 0){
            for(int i = 0; i < strlen(argv[1]); i++){
                if (argv[1][i] == '+'){
                    mode = parsMode(mode, argv[1], i, '+');
                }
            }
            
        }
        else if (match (argv[1], minusMode, &re) == 0){
            for(int i = 0; i < strlen(argv[1]); i++){
                if (argv[1][i] == '-'){
                    mode = parsMode(mode, argv[1], i, '-');
                }
            }
        }
        else if (match (argv[1], eqMode,    &re) == 0){
            for(int i = 0; i < strlen(argv[1]); i++){
                if (argv[1][i] == '='){
                    mode = parsMode(mode, argv[1], i, '=');
                }
            }
        }
        else{
            fprintf(stderr, "mychmod: invalid mode: '%s'\n", argv[1]);
            return 1;
        }
    }
    else {
        if (match (argv[1], numMode,  &re) == 0){
            mode = parsNumMode(mode, argv[1]);
        }
        else{
            fprintf(stderr, "mychmod: invalid mode: '%s'\n", argv[1]);
            return 1;
        }
    }

    if (chmod(argv[2], mode) == -1){
        fprintf(stderr, "mychmod: %d", errno);
        return 1;
    }

    regfree(&re);
    return 0;
}