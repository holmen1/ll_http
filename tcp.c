#include "tcp.h"
#include <stdio.h>

server_status_e bind_tcp_port(tcp_server *server, int port) {
    memset(server, 0, sizeof(*server));
    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server->socket_fd == -1) {
        fprintf(stderr, "Socket creation failed\n");
        return SERVER_SOCKET_ERROR;
    }

    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(port);

    if (bind(server->socket_fd, (struct sockaddr *)&server->address, sizeof(server->address)) < 0) {
        fprintf(stderr, "Bind failed\n");
        close(server->socket_fd);
        return SERVER_BIND_ERROR;
    }

    if (listen(server->socket_fd, 5) < 0) {
        fprintf(stderr, "Listen failed\n");
        close(server->socket_fd);
        return SERVER_LISTEN_ERROR;
    }

    fprintf(stdout, "SERVER: Bound and listening on port %d\n", ntohs(server->address.sin_port));
    return SERVER_OK;
}

int accept_client(int server_fd) {
    struct sockaddr_in client_address = {0};
    socklen_t client_len = sizeof(client_address);

    int client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
    if (client_fd < 0) {
        fprintf(stderr, "Accept failed");
        return -1;
    }

    return client_fd;
}
