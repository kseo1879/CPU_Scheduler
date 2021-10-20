#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_INT (4294967294)
#define NUM_THREAD (5)

void* worker(void *arg) {

    unsigned int i = 0;

    while(i < MAX_INT) {
        i ++;
        printf("%d\n", i);
    }

    return NULL;
}

int main() {

    pthread_t thread[NUM_THREAD];

    for(int i = 0; i < NUM_THREAD; i++) {
        pthread_create(&thread[i], NULL, worker, NULL);
    }

    for(int i = 0; i < NUM_THREAD; i ++) {
        pthread_join(thread[i], NULL);
    }

    return 0;
}    
