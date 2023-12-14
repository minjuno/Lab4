#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 5

int buffer[BUFFER_SIZE];
int in = 0;  
int out = 0; 
int count = 0; 

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;

int generate_random_number() {
    return rand() % 100; 
}

void* producer(void* arg) {
    for (int i = 0; i < 10; ++i) {
        pthread_mutex_lock(&mutex);

        while (count == BUFFER_SIZE) {
            pthread_cond_wait(&empty, &mutex);
        }

        int data = generate_random_number();
	buffer[in] = i;
        in = (in + 1) % BUFFER_SIZE;
        count++;

        printf("%d번 고객 주문\n", i);

        pthread_cond_signal(&full);

        pthread_mutex_unlock(&mutex);

	usleep(rand() % 1000);
    }

    pthread_exit(NULL);
}

void* consumer(void* arg) {
    for (int i = 0; i < 10; ++i) {
        pthread_mutex_lock(&mutex);

        while (count == 0) {
            pthread_cond_wait(&full, &mutex);
        }

        int data = buffer[out];
        out = (out + 1) % BUFFER_SIZE;
        count--;

        printf("%d번 출고 완료되었습니다.\n", data);

        pthread_cond_signal(&empty);

        pthread_mutex_unlock(&mutex);

	usleep(rand() % 1000);
    }

    pthread_exit(NULL);
}

int main() {
    srand((unsigned int)time(NULL));

    pthread_t producer_thread, consumer_thread;

    pthread_create(&producer_thread, NULL, producer, NULL);
    pthread_create(&consumer_thread, NULL, consumer, NULL);

    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);

    return 0;
}

