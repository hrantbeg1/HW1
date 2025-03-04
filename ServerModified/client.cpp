#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#define BUFSIZE 4096
#define USERNAME_LENGTH 128

void *receive_messages(void *socket_ptr) {
    int client_socket = *(int *)socket_ptr;
    char buffer[BUFSIZE];

    while (true) {
        ssize_t received = read(client_socket, buffer, BUFSIZE - 1);
        if (received <= 0) {
            std::cout << "\nDisconnected from server" << std::endl;
            close(client_socket);
            exit(0);
        }
        buffer[received] = '\0';
        std::cout << buffer;
    }
    return nullptr;
}

int main() {
    struct sockaddr_in server_address;
    char username[USERNAME_LENGTH];
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket creation error");
        exit(errno);
    }

    server_address.sin_family = AF_INET;
    inet_aton("127.0.0.1", &server_address.sin_addr);
    server_address.sin_port = htons(8888);

    if (connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("connection failed");
        close(client_socket);
        exit(errno);
    }

    std::cout << "Input your username: ";
    std::cin.getline(username, USERNAME_LENGTH);
    
    if (write(client_socket, username, strlen(username)) == -1) {
        perror("send username failed");
        close(client_socket);
        exit(errno);
    }

    pthread_t recv_thread;
    if (pthread_create(&recv_thread, nullptr, receive_messages, &client_socket) != 0) {
        perror("pthread_create");
        close(client_socket);
        exit(errno);
    }

    std::string input;
    while (true) {
        std::getline(std::cin, input);

        if (input == "/exit") {
            write(client_socket, input.c_str(), input.length());
            break;
        } else if (input == "/list") {
            write(client_socket, input.c_str(), input.length());
        } else if (!input.empty()) {
            write(client_socket, input.c_str(), input.length());
        }
    }

    close(client_socket);
    std::cout << "Disconnected" << std::endl;
    return 0;
}
