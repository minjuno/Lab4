#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <errno.h>
#define PORT 9000
#define MAX_CLIENTS 30
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket[MAX_CLIENTS];
    struct sockaddr_in server_addr, client_addr;
    fd_set readfds;
    char buffer[BUFFER_SIZE];
    int max_clients = MAX_CLIENTS;
    int activity, i, valread, sd;
    int addrlen = sizeof(server_addr);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&i, sizeof(i)) < 0) {
        perror("Setsockopt failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    for (i = 0; i < MAX_CLIENTS; i++) {
        client_socket[i] = 0;
    }

    printf("Server is listening on port %d\n", PORT);

    while (1) {
        FD_ZERO(&readfds);

        FD_SET(server_socket, &readfds);
        int max_sd = server_socket;

        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];

            if (sd > 0) {
                FD_SET(sd, &readfds);
            }

            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Select error\n");
        }

        if (FD_ISSET(server_socket, &readfds)) {
            if ((sd = accept(server_socket, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen)) < 0) {
                perror("Accept failed");
                exit(EXIT_FAILURE);
            }

            printf("New connection, socket fd is %d, IP : %s, port: %d\n", sd, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            char welcome_message[] = "Welcome to the chat room\n";
            send(sd, welcome_message, strlen(welcome_message), 0);

            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_socket[i] == 0) {
                    client_socket[i] = sd;
                    printf("Adding to list of sockets as %d\n", i);
                    break;
                }
            }
        }

        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                    getpeername(sd, (struct sockaddr *)&client_addr, (socklen_t *)&addrlen);
                    printf("Host disconnected, IP %s, port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

                    close(sd);
                    client_socket[i] = 0;
                } else {
                    for (int j = 0; j < MAX_CLIENTS; j++) {
                        int dest_sd = client_socket[j];
                        if (dest_sd != 0 && dest_sd != sd) {
                            send(dest_sd, buffer, valread, 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}

