#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file-path>" << std::endl;
        return 1;
    }

    int file_desc = open(argv[1], O_WRONLY | O_TRUNC);
    if (file_desc < 0) {
        std::cerr << "Failed to open the file: " << strerror(errno) << std::endl;
        return errno;
    }

    const char* first_line = "first line\n";
    ssize_t bytes_written = write(file_desc, first_line, strlen(first_line));
    if (bytes_written < 0) {
        std::cerr << "Error writing first line: " << strerror(errno) << std::endl;
        close(file_desc);
        return errno;
    }

    int second_file_desc = dup(file_desc);
    if (second_file_desc < 0) {
        std::cerr << "Failed to duplicate the file descriptor: " << strerror(errno) << std::endl;
        close(file_desc);
        return errno;
    }

    const char* second_line = "second line\n";
    bytes_written = write(second_file_desc, second_line, strlen(second_line));
    if (bytes_written < 0) {
        std::cerr << "Error writing second line: " << strerror(errno) << std::endl;
        close(file_desc);
        close(second_file_desc);
        return errno;
    }

    close(second_file_desc);
    close(file_desc);

    return 0;
}
