#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define NUM_LEITORES 5
#define NUM_ESCRITORES 2
#define NUM_OPERACOES 5

int readcount = 0;
int writecount = 0;
int shared_variable = 0;

sem_t *mutex_readcount;
sem_t *mutex_writecount;
sem_t *read_try;
sem_t *resource;

void *reader(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < NUM_OPERACOES; i++) {
        sem_wait(read_try);
        sem_wait(mutex_readcount);
        readcount++;
        if (readcount == 1) {
            sem_wait(resource);
        }
        sem_post(mutex_readcount);
        sem_post(read_try);

        printf("Leitor %d está lendo o valor %d\n", id, shared_variable);
        sleep(1);

        sem_wait(mutex_readcount);
        readcount--;
        if (readcount == 0) {
            sem_post(resource);
        }
        sem_post(mutex_readcount);

        sleep(1);
    }
    pthread_exit(NULL);
}

void *writer(void *arg) {
    int id = *(int *)arg;
    for (int i = 0; i < NUM_OPERACOES; i++) {
        sem_wait(mutex_writecount);
        writecount++;
        if (writecount == 1) {
            sem_wait(read_try);
        }
        sem_post(mutex_writecount);

        sem_wait(resource);
        shared_variable++;
        printf("Escritor %d escreveu o valor %d\n", id, shared_variable);
        sleep(1);
        sem_post(resource);

        sem_wait(mutex_writecount);
        writecount--;
        if (writecount == 0) {
            sem_post(read_try);
        }
        sem_post(mutex_writecount);

        sleep(1);
    }
    pthread_exit(NULL);
}

int main() {
    sem_unlink("/mutex_readcount");
    sem_unlink("/mutex_writecount");
    sem_unlink("/read_try");
    sem_unlink("/resource");

    mutex_readcount = sem_open("/mutex_readcount", O_CREAT, 0644, 1);
    mutex_writecount = sem_open("/mutex_writecount", O_CREAT, 0644, 1);
    read_try = sem_open("/read_try", O_CREAT, 0644, 1);
    resource = sem_open("/resource", O_CREAT, 0644, 1);

    if (mutex_readcount == SEM_FAILED || mutex_writecount == SEM_FAILED ||
        read_try == SEM_FAILED || resource == SEM_FAILED) {
        perror("sem_open");
        exit(EXIT_FAILURE);
    }

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

    sem_close(mutex_readcount);
    sem_unlink("/mutex_readcount");

    sem_close(mutex_writecount);
    sem_unlink("/mutex_writecount");

    sem_close(read_try);
    sem_unlink("/read_try");

    sem_close(resource);
    sem_unlink("/resource");

    return 0;
}
