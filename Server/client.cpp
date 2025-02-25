#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        return EXIT_FAILURE;
    }

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr);

    if (connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        close(client_socket);
        return EXIT_FAILURE;
    }

    std::cout << "Connected to the server. Type messages and press Enter to send.\n";
    
    char buffer[BUFFER_SIZE];
    while (true) {
        std::cout << "You: ";
        if (!std::cin.getline(buffer, BUFFER_SIZE)) {
            break;
        }
        strcat(buffer, "\n");

        if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            break;
        }
    }

    std::cout << "Disconnected from server\n";
    close(client_socket);
    return 0;
}
