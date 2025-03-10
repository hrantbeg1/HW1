#include <arpa/inet.h>
#include <iostream>
#include <string>
#include <unistd.h>

#define SERVER_PORT 8888
#define BUFFER_SIZE 4096

int main() {
    int socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        perror("inet_pton");
        return 1;
    }

    if (connect(socketFD, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("connect");
        return 1;
    }
    std::string userInput;
    char response[BUFFER_SIZE];

    while (true) {
        std::getline(std::cin, userInput);

        if (userInput.empty()) break;

        if (send(socketFD, userInput.c_str(), userInput.size() + 1, 0) <= 0) {
            perror("send");
            break;
        }

        ssize_t receivedBytes = recv(socketFD, response, BUFFER_SIZE - 1, 0);
        if (receivedBytes <= 0) {
            perror("recv");
            break;
        }

        response[receivedBytes] = '\0';
        std::cout << response << '\n';
    }

    close(socketFD);
    return 0;
}
