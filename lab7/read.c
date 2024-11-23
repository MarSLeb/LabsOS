#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdbool.h>

#define SHM_SIZE 128

int main() {
    int shmid;
    char *data;
    key_t key;

    if ((key = ftok("shmem", 'a')) == -1) {
        perror("ftok");
        exit(1);
    }

    if ((shmid = shmget(key, SHM_SIZE, IPC_EXCL | IPC_CREAT)) == -1) {
        perror("shmget");
        exit(1);
    }

    if ((data = shmat(shmid, NULL, 0)) == (void *)-1) {
        perror("shmat");
        exit(1);
    }

    int pid = getpid();
    while (true) {
        printf("%s, read PID - %d\n", data, pid);
        sleep(1);
    }

    if (shmdt(data) == -1) {
        perror("shmdt");
        exit(1);
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl");
        exit(1);
    }

    free(data);
    return 0;
}
