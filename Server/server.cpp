#include <iostream>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <cstring>
#include <mutex>

#define PORT 8888
#define BUFFER_SIZE 1024

std::mutex cout_mutex;

struct ClientInfo {
    int socket;
    sockaddr_in address;
};

void* handle_client(void* arg) {
    ClientInfo* info = static_cast<ClientInfo*>(arg);
    int client_socket = info->socket;
    sockaddr_in client_address = info->address;
    delete info;

    char buffer[BUFFER_SIZE];
    int bytes_received;

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Client connected: " << inet_ntoa(client_address.sin_addr) << std::endl;
    }

    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        std::lock_guard<std::mutex> lock(cout_mutex);
        std::cout << "Message from " << inet_ntoa(client_address.sin_addr) << ": " << buffer;
    }

    {
        std::lock_guard<std::mutex> lock(cout_mutex);
        if (bytes_received == 0) {
            std::cout << "Client " << inet_ntoa(client_address.sin_addr) << " disconnected" << std::endl;
        } else {
            perror("recv error");
        }
    }

    close(client_socket);
    return nullptr;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        close(server_socket);
        return EXIT_FAILURE;
    }

    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        return EXIT_FAILURE;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (true) {
        sockaddr_in client_address;
        socklen_t client_addr_len = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_address, &client_addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        pthread_t thread;
        ClientInfo* info = new ClientInfo{client_socket, client_address};

        if (pthread_create(&thread, nullptr, handle_client, info) != 0) {
            perror("Thread creation failed");
            delete info;
            close(client_socket);
        } else {
            pthread_detach(thread);
        }
    }

    close(server_socket);
    return 0;
}
