#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>

int openFile(const char* filename, int flags, mode_t mode = 0) {
    return open(filename, flags, mode);
}

ssize_t readFromFile(int fd, char* buffer, size_t size) {
    return read(fd, buffer, size);
}

ssize_t writeToFile(int fd, const char* buffer, size_t size) {
    return write(fd, buffer, size);
}

int main(int argc, char** argv) {
    if (argc == 3) {
        char buffer[256];
        int sourceFd = openFile(argv[1], O_RDONLY);
        if (sourceFd == -1) {
            std::cerr << "Error opening source file, code " << errno << std::endl;
            return errno;
        }

        int destFd = openFile(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (destFd == -1) {
            close(sourceFd);
            std::cerr << "Error opening destination file, code " << errno << std::endl;
            return errno;
        }

        ssize_t bytesRead, bytesWritten;
        while ((bytesRead = readFromFile(sourceFd, buffer, sizeof(buffer))) > 0) {
            bytesWritten = writeToFile(destFd, buffer, bytesRead);
            if (bytesWritten == -1) {
                std::cerr << "Error writing file, code " << errno << std::endl;
                close(sourceFd);
                close(destFd);
                return errno;
            }
        }
        if (bytesRead == -1) {
            std::cerr << "Error reading file, code " << errno << std::endl;
            close(sourceFd);
            close(destFd);
            return errno;
        }

        close(sourceFd);
        close(destFd);
        std::cout << "File was copied successfully" << std::endl;
        return 0;
    } else {
        std::cerr << "Invalid arguments, use [source file] [destination file]" << std::endl;
        return 1;
    }
}
