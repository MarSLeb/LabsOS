#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h> 

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


void help(){
    puts("Usage: ./rawr [OPTION] [FILE]...");
    puts("'rawr' saves many files together into a single tape or disk archive, \
and can restore individual files from the archive.");
    puts("Examples: \n\
    ./rawr -i archive foo bar    # Create archive.tar from files foo and bar.\n\
    ./rawr -s archive            # List all files in archive verbosely.\n\
    ./rawr -e archive bar        # Extract file bar from archive.\n\
    ./rawr -h                    # You are here.");
}

int main(int argc, char** argv){
    int rez = 0;
    bool i = false, e = false, s = false;
    while ((rez = getopt(argc, argv, "iesh")) != -1){
		switch (rez) {
		case 'i': {
            i = true;
            break;
        }
		case 'e': {
            e = true;
            break;
        }
        case 's': {
            s = true;
            break;
        }
		case 'h': {
            help();
            return 0;
        }
		case '?': {
            return 1;
        }
		} 
	}
    
    if (argc < 3){
        fprintf(stderr, "./rawr: option requires an argument\nTry './rawr -h'\n");
        return 1;
    }

    char* archName = argv[optind];
    if (i){
        //int arch = open(archName, O_WRONLY | O_CREAT);

    }
    else{
        int arch = open(archName, O_RDONLY);
        if (arch == -1){
            fprintf(stderr, "./rawr: %s: Cannot open: No such file or directory\n", archName);
            return 1;
        }

        if (e){


        }
        if (s){


        }
    }


    return 0;
}