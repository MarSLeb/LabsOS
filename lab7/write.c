#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <unistd.h>

#define SHM_SIZE 1024

void handler() {
  remove("shmem");
  exit(EXIT_SUCCESS);
}

int main()
{
    signal(SIGTSTP, handler);
    signal(SIGINT, handler);


    key_t key;
    int shmid;
    char *data;

    FILE *fp = fopen("shmem", "a");

    if ((key = ftok("shmem", 'a')) == -1) {
        perror("ftok");
        exit(1);
    }

    if ((shmid = shmget(key, SHM_SIZE, IPC_EXCL | IPC_CREAT | 0660)) == -1) {
        fprintf(stderr, "./write: write stream alredy exist\n");
        exit(1);
    }

    if ((data = shmat(shmid, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    int pid = getpid();
    while(true) {
        time_t mytime = time(NULL);
        struct tm *now = localtime(&mytime);
        char str[50];
        strftime(str, sizeof(str), "%H:%M:%S", now);
        sprintf(data, "time: %s. write PID - %d", str, pid);
        sleep(1);
    }

    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shmid, IPC_NOWAIT, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }

    return 0;
}