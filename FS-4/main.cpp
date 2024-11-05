#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <cstdlib>

void initialize_input(int argc, char **argv)
{
    if (argc != 2) {
        std::cerr << "Invalid arguments" << std::endl;
        std::exit(1);
    }

    int input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        std::cerr << "Error opening file" << std::endl;
        std::exit(2);
    }

    if (dup2(input_fd, STDIN_FILENO) == -1) {
        std::cerr << "Error redirecting input" << std::endl;
        close(input_fd);
        std::exit(3);
    }

    close(input_fd);
}

int main(int argc, char **argv)
{
    initialize_input(argc, argv);

    std::string input_text;
    std::cin >> input_text;

    std::string reversed_str(input_text.rbegin(), input_text.rend());
    std::cout << reversed_str << std::endl;

    return 0;
}
