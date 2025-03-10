#include <arpa/inet.h>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <unistd.h>

#define SERVER_PORT 8888
#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 4096

struct MathRequest {
    std::string command;
    int operand1;
    int operand2;
    bool isValid;
};

MathRequest parseInput(const std::string &input) {
    MathRequest request;
    std::istringstream ss(input);
    ss >> request.command >> request.operand1 >> request.operand2;
    request.isValid = !ss.fail();
    return request;
}

std::string executeOperation(const MathRequest &request) {
    if (!request.isValid) return "Invalid input format";

    if (request.command == "ADD") return std::to_string(request.operand1 + request.operand2);
    if (request.command == "SUB") return std::to_string(request.operand1 - request.operand2);
    if (request.command == "MUL") return std::to_string(request.operand1 * request.operand2);
    if (request.command == "DIV") {
        if (request.operand2 == 0) return "Divided by zero";
        return std::to_string(request.operand1 / request.operand2);
    }
    return "ERROR: Unknown command";
}

void clientHandler(int clientSocket) {
    char buffer[BUFFER_SIZE];

    while (true) {
        ssize_t bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
        if (bytesRead <= 0) break;

        buffer[bytesRead] = '\0';
        std::string response = executeOperation(parseInput(buffer));
        send(clientSocket, response.c_str(), response.size() + 1, 0);
    }

    close(clientSocket);
}

int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("bind");
        return 1;
    }

    if (listen(serverSocket, MAX_CONNECTIONS) < 0) {
        perror("listen");
        return 1;
    }

    std::cout << "Server is running on port " << SERVER_PORT << "...\n";

    while (true) {
        sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);
        if (clientSocket < 0) {
            perror("accept");
            continue;
        }

        std::thread(clientHandler, clientSocket).detach();
    }

    close(serverSocket);
    return 0;
}
