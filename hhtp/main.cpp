#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <fstream>

#define BUFFER_SIZE 4096

int main() {
    struct addrinfo hints{}, *server_info, *ptr;
    struct sockaddr_in server_addr;
    char ip_address[INET_ADDRSTRLEN];
    int sock_fd;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo("httpforever.com", "80", &hints, &server_info) != 0) {
        perror("Failed to resolve hostname");
        return 1;
    }

    for (ptr = server_info; ptr != nullptr; ptr = ptr->ai_next) {
        if (ptr->ai_family == AF_INET) {
            struct sockaddr_in *addr_ptr = (struct sockaddr_in *)ptr->ai_addr;
            inet_ntop(ptr->ai_family, &(addr_ptr->sin_addr), ip_address, sizeof(ip_address));
            server_addr = *addr_ptr;
            break;
        }
    }
    freeaddrinfo(server_info);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_port = htons(80);

    if (connect(sock_fd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    std::string http_request = "GET / HTTP/1.1\r\nHost: httpforever.com\r\nConnection: close\r\n\r\n";
    if (send(sock_fd, http_request.c_str(), http_request.length(), 0) == -1) {
        perror("Error sending request");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    std::ofstream output_file("response.html", std::ios::out | std::ios::binary);
    if (!output_file) {
        perror("File creation error");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    int received_bytes;
    while ((received_bytes = recv(sock_fd, buffer, BUFFER_SIZE, 0)) > 0) {
        output_file.write(buffer, received_bytes);
    }
    if (received_bytes < 0) {
        perror("Receiving data failed");
    }

    std::cout << "Response saved to response.html" << std::endl;
    close(sock_fd);
    output_file.close();
    return 0;
}
