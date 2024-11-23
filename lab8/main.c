#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
 

int* data = NULL;
int size = 0;

pthread_mutex_t m; 

void handler() {
  free(data);
  exit(EXIT_SUCCESS);
}

void* writeThread(void* thread_id)
{
    int id = *(int*)thread_id;
    while(true){
        pthread_mutex_lock( &m );
        printf("%d: working...\n", *((unsigned long *)thread_id));
        if (data == NULL){
            data = calloc(1, sizeof(int));
            size = 1;
            data[0] = 1;
        }
        else{
            data = realloc(data, (size + 1) * sizeof(int));
            size += 1;
            data[size - 1] = size;
        }
        pthread_mutex_unlock( &m ); 
        sleep(1);
    }
    return NULL;
}
 
void* readThread(void* thread_id)
{
    int id = *(int*)thread_id;
    while (true){
        pthread_mutex_lock( &m );
        if (data == NULL){
            continue;
        }
        printf("%d: ", *((unsigned long *)thread_id));
        for (int i = 0; i < size; i++){
            printf("%d ", data[i]);
        }
        printf("\n");
        pthread_mutex_unlock( &m );
        sleep(1);
    } 
    return NULL;
}

int main(void) 
{
    signal(SIGTSTP, handler);
    signal(SIGINT, handler);

    pthread_t t1, t2, t3, t4, t5, t6, t7, t8 , t9, t10, t11;
    int t1_id = 1, t2_id = 2, t3_id = 3, t4_id = 4, t5_id = 5, t6_id = 6;
    int t7_id = 7, t8_id = 8, t9_id = 9, t10_id = 10, t11_id = 11;

    pthread_mutex_init(&m, NULL); 
    pthread_create(&t1, NULL, writeThread, &t1_id);
    pthread_create(&t2, NULL, readThread, &t2_id);
    pthread_create(&t2, NULL, readThread, &t3_id);
    pthread_create(&t2, NULL, readThread, &t4_id);
    pthread_create(&t2, NULL, readThread, &t5_id);
    pthread_create(&t2, NULL, readThread, &t6_id);
    pthread_create(&t2, NULL, readThread, &t7_id);
    pthread_create(&t2, NULL, readThread, &t8_id);
    pthread_create(&t2, NULL, readThread, &t9_id);
    pthread_create(&t2, NULL, readThread, &t10_id);
    pthread_create(&t2, NULL, readThread, &t11_id);
 
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);
    pthread_join(t5, NULL);
    pthread_join(t5, NULL);
    pthread_join(t7, NULL);
    pthread_join(t8, NULL);
    pthread_join(t9, NULL);
    pthread_join(t10, NULL);
    pthread_join(t11, NULL);
     
    pthread_mutex_destroy(&m);  

    return 0;
}