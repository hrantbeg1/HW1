#include <iostream>
#include <unistd.h>
#include <fstream>
#include <fcntl.h>
#include <sys/stat.h>
bool file(const char* path) {
    struct stat buffer;
    return (stat(path, &buffer) == 0);
}

void overwrite(const char* path) {
    int fd = open(path, O_WRONLY);
    if (fd == -1) {
        std::cerr << "Error opening " << std::endl;
        exit(1);
    }

    struct stat st;
    if (fstat(fd, &st) != 0) {
        std::cerr << "Error getting size" << std::endl;
        close(fd);
        exit(1);
    }

    off_t size = st.st_size;
    char null_byte = '\0';
    for (off_t i = 0; i < size; ++i) {
        if (write(fd, &null_byte, 1) != 1) {
            std::cerr << "Error overwriting" << std::endl;
            close(fd);
            exit(1);
        }
    }

    close(fd);
}

void delete(const char* path) {
    if (unlink(path) != 0) {
        std::cerr << "Error deleting" << std::endl;
        exit(1);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: ./hacker-rm <file-to-erase>" << std::endl;
        return 1;
    }

    const char* file_path = argv[1];

    if (!file(file_path)) {
        std::cerr << "File does not exist" << std::endl;
        return 1;
    }

    overwrite(file_path);
    delete(file_path);

    return 0;
}
