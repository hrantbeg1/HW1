#include <errno.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#define CLIENTS 10
#define BUFSIZE 4096
#define USERNAME_LENGTH 128

struct Client {
    int socket;
    bool active;
    char username[USERNAME_LENGTH];
};

struct Server {
    Client clients[CLIENTS];
    pthread_mutex_t mutex;
    int server_socket;
};

int find_free_slot(Server *server) {
    for (int i = 0; i < CLIENTS; i++) {
        if (!server->clients[i].active) {
            return i;
        }
    }
    return -1;
}

void send_to_all(const std::string &message, int sender_id, Server *server) {
    pthread_mutex_lock(&server->mutex);
    for (int i = 0; i < CLIENTS; i++) {
        if (server->clients[i].active && i != sender_id) {
            send(server->clients[i].socket, message.c_str(), message.size(), 0);
        }
    }
    pthread_mutex_unlock(&server->mutex);
}

void send_user_list(int client_socket, Server *server) {
    std::string list = "Online users:\n";
    pthread_mutex_lock(&server->mutex);
    for (int i = 0; i < CLIENTS; i++) {
        if (server->clients[i].active) {
            list += server->clients[i].username;
            list += "\n";
        }
    }
    pthread_mutex_unlock(&server->mutex);
    send(client_socket, list.c_str(), list.size(), 0);
}

void *client_handler(void *arg) {
    Server *server = (Server *)arg;
    int client_slot = find_free_slot(server);
    if (client_slot == -1) {
        std::cerr << "No available client slots\n";
        return nullptr;
    }

    int client_socket = server->clients[client_slot].socket;
    ssize_t received = recv(client_socket, server->clients[client_slot].username, USERNAME_LENGTH, 0);
    if (received <= 0) {
        server->clients[client_slot].active = false;
        return nullptr;
    }
    server->clients[client_slot].username[received] = '\0';
    server->clients[client_slot].active = true;

    char buf[BUFSIZE];
    while (true) {
        received = recv(client_socket, buf, BUFSIZE, 0);
        if (received <= 0) {
            break;
        }
        buf[received] = '\0';

        if (strcmp(buf, "/exit") == 0) {
            break;
        } else if (strcmp(buf, "/list") == 0) {
            send_user_list(client_socket, server);
        } else {
            std::string message = std::string(server->clients[client_slot].username) + ": " + buf + "\n";
            send_to_all(message, client_slot, server);
        }
    }

    close(client_socket);
    server->clients[client_slot].active = false;
    return nullptr;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket error");
        exit(errno);
    }

    struct sockaddr_in server_info{};
    server_info.sin_family = AF_INET;
    server_info.sin_addr.s_addr = htonl(INADDR_ANY);
    server_info.sin_port = htons(8888);

    if (bind(server_socket, (struct sockaddr *)&server_info, sizeof(server_info)) < 0) {
        perror("Bind error");
        exit(errno);
    }

    if (listen(server_socket, CLIENTS) < 0) {
        perror("Listen error");
        exit(errno);
    }

    Server server{};
    server.server_socket = server_socket;
    pthread_mutex_init(&server.mutex, nullptr);
    
    for (int i = 0; i < CLIENTS; i++) {
        server.clients[i].active = false;
    }

    while (true) {
        struct sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Accept error");
            continue;
        }

        int client_slot = find_free_slot(&server);
        if (client_slot < 0) {
            std::cerr << "Server full, rejecting connection\n";
            close(client_socket);
            continue;
        }
server.clients[client_slot].socket = client_socket;
        pthread_t thread;
        if (pthread_create(&thread, nullptr, client_handler, &server) != 0) {
            perror("Thread error");
            close(client_socket);
        } else {
            pthread_detach(thread);
        }
    }

    pthread_mutex_destroy(&server.mutex);
    return 0;
}
