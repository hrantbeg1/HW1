#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << argv[0];
        return 1;
    }
    int file = open(argv[1], O_WRONLY | O_CREAT | O_APPEND);
    if (file == -1) {
        std::cerr << "Error " << argv[1] << '\n';
        return 1;
    }
    std::string input;
    while (true) {
        std::getline(std::cin, input);
        if (input == "stop") {
            break;
        }
        input += "\n";
        write(file, input.c_str(), input.size());
    }
    close(file);
    return 0;
}
