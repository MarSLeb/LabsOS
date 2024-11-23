#include <sys/wait.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

int main(){
    puts("pipe:");

    int pipefd[2];
    pid_t cpid;
    char buf;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {   
        close(pipefd[1]);         
        while (read(pipefd[0], &buf, 1) > 0){
            write(STDOUT_FILENO, &buf, 1);
        }
        sleep(6);
        time_t mytime = time(NULL);
        struct tm *now = localtime(&mytime);
        char str[35];
        puts(" - parent time");
        strftime(str, sizeof(str), "%H:%M:%S - child time", now);
        puts(str);

        close(pipefd[0]);
        _exit(EXIT_SUCCESS);

    } else {
        close(pipefd[0]);
        time_t mytime = time(NULL);
        struct tm *now = localtime(&mytime);
        char str[50];
        strftime(str, sizeof(str), "%H:%M:%S", now);
        int id = getpid();
        while (id % 10 != 0){
            char c[1] = {'0' + id % 10};
            write(pipefd[1], c, 1);
            id /= 10;
        }
        write(pipefd[1], " - parent id.\n", 14);
        write(pipefd[1], str, strlen(str));
        close(pipefd[1]);
        wait(NULL);
    }

    puts("FIFO:");

    if (mkfifo("FIFO", 0666) == -1) {
        perror("mkfifo");
    }
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {          
        int fd = open("FIFO", O_RDONLY);
        while (read(fd, &buf, 1) > 0){
            write(STDOUT_FILENO, &buf, 1);
        }
        sleep(6);
        time_t mytime = time(NULL);
        struct tm *now = localtime(&mytime);
        char str[35];
        puts(" - parent time");
        strftime(str, sizeof(str), "%H:%M:%S - child time", now);
        puts(str);
        _exit(EXIT_SUCCESS);

    } else {
        int fd = open("FIFO", O_WRONLY);
        time_t mytime = time(NULL);
        struct tm *now = localtime(&mytime);
        char str[50];
        strftime(str, sizeof(str), "%H:%M:%S", now);
        int id = getpid();
        while (id % 10 != 0){
            char c[1] = {'0' + id % 10};
            write(fd, c, 1);
            id /= 10;
        }
        write(fd, " - parent id.\n", 14);
        write(fd, str, strlen(str));
        close(fd);
        wait(NULL); 
        unlink("FIFO");
    }



    return 0;
}