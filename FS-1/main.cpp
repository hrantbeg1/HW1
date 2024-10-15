#include <iostream>
#include <fcntl.h>
#include <cerrno>
#include <unistd.h>

int main (int argc, char **argv)
{
    if (argc > 1)
    {
        int file_desc = open(argv[1], O_RDONLY);
        if (file_desc == -1)
        {
            std::cerr << "Error opening file, code: " << errno << std::endl;
            return errno;
        }
        const int BUFFER_SIZE = 256;
        char buffer[BUFFER_SIZE];
        int bytes_read;
        while ((bytes_read = read(file_desc, buffer, BUFFER_SIZE)) > 0)
        {
            std::cout.write(buffer, bytes_read);
        }
        if (close(file_desc) == -1)
        {
            std::cerr << "Error closing file, code: " << errno << std::endl;
            return errno;
        }
        if (bytes_read == -1)
        {
            std::cerr << "Error reading file, code: " << errno << std::endl;
            close(file_desc);
            return errno;
        }

    }
    else
    {
        std::cerr << "Invalid arguments" << std::endl;
        return 1;
    }
    return 0;
}
