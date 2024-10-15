#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

void exitHandler(){
    printf("Процеcc %d завершает работу.\n", getpid());
    exit(EXIT_SUCCESS);
}

void sigintHandler(int sig){
    printf("Получен сигнал SIGINT.\n");
    exit(EXIT_SUCCESS);
}

void sigtermHandler(int sig){
    printf("Получен сигнал SIGTERM.\n");
}


int main(){
    signal(SIGINT, sigintHandler);
    signal(SIGTERM, sigtermHandler);
    atexit(exitHandler);
    int res = 0;
    switch(res = fork()) {
    case -1:
            fprintf(stderr, "Error with fork\n");
            return 1;

    case 0:
            printf("Дочерний процесс запущен. PID: %d\n", getpid());
            sleep(5);
            printf("Дочерний процесс завершает работу.\n");
            exit(EXIT_SUCCESS);

    default:
            printf("Родительский процесс. PID: %d, PID дочернего процесса: %d\n", getpid(), res);
            int status;
            pid_t wpid = waitpid(res, &status, 0);
            if (wpid == -1) {
                perror("Ошибка waitpid");
                exit(EXIT_FAILURE);
            }            
            if (WIFEXITED(status)) {
                printf("Дочерний процесс %d завершился с кодом %d\n", res, WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Дочерний процесс %d был завершен сигналом %d\n", res, WTERMSIG(status));
            }

    }
    return 0;
}