#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <cstring>

const int BUFFER_SIZE = 1024;

void handle_error(const char *message)
{
    std::cerr << message << std::endl;
    exit(errno);
}

void copy_data(int src_fd, int dst_fd, char buffer[BUFFER_SIZE], int size, int &data_bytes, int &copied_bytes)
{
    ssize_t bytes_read, bytes_written;

    while (size > 0)
    {
        bytes_read = read(src_fd, buffer, std::min(size, BUFFER_SIZE));
        if (bytes_read < 0)
            handle_error("Error reading from source file");
        if (bytes_read == 0)
            break;

        bytes_written = write(dst_fd, buffer, bytes_read);
        if (bytes_written < 0)
            handle_error("Error writing to destination file");

        data_bytes += bytes_read;
        copied_bytes += bytes_written;
        size -= bytes_read;
    }
}

void skip_hole(int dst_fd, int size, int &hole_bytes, int &copied_bytes)
{
    while (size > 0)
    {
        int skip = std::min(size, BUFFER_SIZE);
        lseek(dst_fd, skip, SEEK_CUR);
        hole_bytes += skip;
        copied_bytes += skip;
        size -= skip;
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
        handle_error("Invalid arguments. Usage: ./copy source-file destination-file");

    int src_fd = open(argv[1], O_RDONLY);
    if (src_fd == -1)
        handle_error("Error opening source file");

    int dst_fd = open(argv[2], O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (dst_fd == -1)
        handle_error("Error opening destination file");

    char buffer[BUFFER_SIZE];
    off_t pos = 0;
    off_t data_pos, hole_pos;
    int copied_bytes = 0, data_bytes = 0, hole_bytes = 0;
    bool first_pass = true;

    while (true)
    {
        data_pos = lseek(src_fd, pos, SEEK_DATA);
        hole_pos = lseek(src_fd, pos, SEEK_HOLE);

        if (first_pass && data_pos >= 0 && hole_pos == lseek(src_fd, pos, SEEK_END))
        {
            lseek(src_fd, pos, SEEK_SET);
            copy_data(src_fd, dst_fd, buffer, hole_pos - data_pos, data_bytes, copied_bytes);
            break;
        }

        if (data_pos < 0 && hole_pos < 0)
        {
            if (errno == ENXIO)
                break;
            else
                handle_error("lseek error");
        }

        if (data_pos == hole_pos)
            break;

        if (data_pos < hole_pos || hole_pos == -1)
        {
            copy_data(src_fd, dst_fd, buffer, hole_pos - data_pos, data_bytes, copied_bytes);
            pos = hole_pos;
        }
        else
        {
            skip_hole(dst_fd, data_pos - pos, hole_bytes, copied_bytes);
            pos = data_pos;
        }

        first_pass = false;
    }

    close(src_fd);
    close(dst_fd);

    std::cout << "Successfully copied " << copied_bytes << " bytes (data: " << data_bytes << ", hole: " << hole_bytes << ")." << std::endl;

    return 0;
}
