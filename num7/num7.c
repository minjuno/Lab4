#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUF_SIZE 1024
#define PORT 8080

void handle_get_request(int client_socket, const char *request);
void handle_post_request(int client_socket, const char *request);
void execute_cgi_program(int client_socket, const char *cgi_program);

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Error listening for connections");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        if ((client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len)) == -1) {
            perror("Error accepting connection");
            continue;
        }

        printf("Connection accepted from %s\n", inet_ntoa(client_addr.sin_addr));

        char request[MAX_BUF_SIZE];
        memset(request, 0, sizeof(request));
        if (read(client_socket, request, sizeof(request)) == -1) {
            perror("Error reading request");
            close(client_socket);
            continue;
        }

        if (strncmp(request, "GET", 3) == 0) {
            handle_get_request(client_socket, request);
        } else if (strncmp(request, "POST", 4) == 0) {
            handle_post_request(client_socket, request);
        }

        close(client_socket);
    }

    close(server_socket);

    return 0;
}

void handle_get_request(int client_socket, const char *request) {
    char resource[MAX_BUF_SIZE];
    sscanf(request, "GET %s HTTP", resource);

    if (strstr(resource, ".cgi") != NULL) {
        execute_cgi_program(client_socket, resource + 1); 
    } else {
        char response[MAX_BUF_SIZE];
        sprintf(response, "HTTP/1.1 200 OK\nContent-Type: text/html\n\n<html><body><h1>Juno's Web Server</h1></body></html>");
        write(client_socket, response, strlen(response));
    }
}

void handle_post_request(int client_socket, const char *request) {
}

void execute_cgi_program(int client_socket, const char *cgi_program) {
}

