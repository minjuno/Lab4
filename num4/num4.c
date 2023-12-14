#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_CLIENTS 4

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void* client_thread(void* arg) {
    int client_id = *(int*)arg;
    free(arg);

    pthread_mutex_lock(&mutex);
    printf("Client %d 메시지 전송 요청.\n", client_id);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);

    pthread_exit(NULL);
}

void* server_thread(void* arg) {

    while (1) {
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond, &mutex);

        printf("Server : 메시지 발송 완료.\n");

        pthread_mutex_unlock(&mutex);
    }

    pthread_exit(NULL);
}

int main() {
    pthread_t server, clients[MAX_CLIENTS];
    int i;

    if (pthread_create(&server, NULL, server_thread, NULL) != 0) {
        perror("Error creating server thread");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < MAX_CLIENTS; ++i) {
        int* client_id = (int*)malloc(sizeof(int));
        *client_id = i + 1;

        if (pthread_create(&clients[i], NULL, client_thread, client_id) != 0) {
            perror("Error creating client thread");
            exit(EXIT_FAILURE);
        }
    }

    pthread_join(server, NULL);
    for (i = 0; i < MAX_CLIENTS; ++i) {
        pthread_join(clients[i], NULL);
    }

    return 0;
}

