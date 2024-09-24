#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 5
#define NUM_ITENS 10

int buffer[BUFFER_SIZE];
int in = 0;
int out = 0;

sem_t *empty;
sem_t *full;
sem_t *mutex;

void *producer(void *arg) {
    for (int i = 0; i < NUM_ITENS; i++) {
        int item = i;

        sem_wait(empty);
        sem_wait(mutex);

        buffer[in] = item;
        in = (in + 1) % BUFFER_SIZE;
        printf("Produzido: %d\n", item);

        sem_post(mutex);
        sem_post(full);
    }
    pthread_exit(NULL);
}

void *consumer(void *arg) {
    for (int i = 0; i < NUM_ITENS; i++) {
        sem_wait(full);
        sem_wait(mutex);

        int item = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        printf("Consumido: %d\n", item);

        sem_post(mutex);
        sem_post(empty);
    }
    pthread_exit(NULL);
}

int main() {
    sem_unlink("/empty_sem");
    sem_unlink("/full_sem");
    sem_unlink("/mutex_sem");

    empty = sem_open("/empty_sem", O_CREAT, 0644, BUFFER_SIZE);
    full = sem_open("/full_sem", O_CREAT, 0644, 0);
    mutex = sem_open("/mutex_sem", O_CREAT, 0644, 1);

    if (empty == SEM_FAILED || full == SEM_FAILED || mutex == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

    pthread_t producer_thread, consumer_thread;

    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    sem_close(empty);
    sem_unlink("/empty_sem");

    sem_close(full);
    sem_unlink("/full_sem");

    sem_close(mutex);
    sem_unlink("/mutex_sem");

    return 0;
}
