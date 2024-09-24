#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_LEITORES 5
#define NUM_ESCRITORES 2
#define NUM_OPERACOES 5

int shared_variable = 0;

int readers_waiting = 0;
int writers_waiting = 0;
int active_readers = 0;
int active_writers = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t readers_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t writers_cond = PTHREAD_COND_INITIALIZER;

pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

void *reader(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < NUM_OPERACOES; i++) {
        pthread_mutex_lock(&mutex);
        readers_waiting++;
        while (writers_waiting > 0 || active_writers > 0) {
            pthread_cond_wait(&readers_cond, &mutex);
        }
        readers_waiting--;
        active_readers++;
        pthread_mutex_unlock(&mutex);

        pthread_rwlock_rdlock(&rwlock);
        printf("Leitor %d está lendo o valor %d\n", id, shared_variable);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);

        pthread_mutex_lock(&mutex);
        active_readers--;
        if (active_readers == 0 && writers_waiting > 0) {
            pthread_cond_signal(&writers_cond);
        }
        pthread_mutex_unlock(&mutex);

        sleep(1); 
    }
    pthread_exit(NULL);
}

void *writer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < NUM_OPERACOES; i++) {
        pthread_mutex_lock(&mutex);
        writers_waiting++;
        while (active_readers > 0 || active_writers > 0) {
            pthread_cond_wait(&writers_cond, &mutex);
        }
        writers_waiting--;
        active_writers++;
        pthread_mutex_unlock(&mutex);

        pthread_rwlock_wrlock(&rwlock);
        shared_variable++;
        printf("Escritor %d escreveu o valor %d\n", id, shared_variable);
        pthread_rwlock_unlock(&rwlock);
        sleep(1);

        pthread_mutex_lock(&mutex);
        active_writers--;
        if (writers_waiting > 0) {
            pthread_cond_signal(&writers_cond);
        } else if (readers_waiting > 0) {
            pthread_cond_broadcast(&readers_cond);
        }
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t readers[NUM_LEITORES], writers[NUM_ESCRITORES];
    int reader_ids[NUM_LEITORES], writer_ids[NUM_ESCRITORES];

    for (int i = 0; i < NUM_LEITORES; i++) {
        reader_ids[i] = i + 1;
        if (pthread_create(&readers[i], NULL, reader, &reader_ids[i]) != 0) {
            perror("Erro ao criar thread leitora");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_ESCRITORES; i++) {
        writer_ids[i] = i + 1;
        if (pthread_create(&writers[i], NULL, writer, &writer_ids[i]) != 0) {
            perror("Erro ao criar thread escritora");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_LEITORES; i++) {
        pthread_join(readers[i], NULL);
    }

    for (int i = 0; i < NUM_ESCRITORES; i++) {
        pthread_join(writers[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&readers_cond);
    pthread_cond_destroy(&writers_cond);
    pthread_rwlock_destroy(&rwlock);

    return 0;
}
