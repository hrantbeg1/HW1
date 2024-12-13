#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>

void split_input(const std::string &input, std::vector<std::string> &args) {
    std::istringstream iss(input);
    std::string token;
    while (iss >> token) {
        args.push_back(token);
    }
}

bool is_exit_command(const std::string &command) {
    return command == "exit";
}

int execute_command(const std::vector<std::string> &args, bool silent) {
    pid_t pid = fork();

    if (pid < 0) {
        std::cerr << "Fork failed" << std::endl;
        return -1;
    } else if (pid == 0) {
        if (silent) {
            std::string log_file = std::to_string(getpid()) + ".log";
            int fd = open(log_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd < 0) {
                std::cerr << "Failed to open log file" << std::endl;
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            dup2(fd, STDERR_FILENO);
            close(fd);
        }
        const char *current_path = getenv("PATH");
        std::string new_path = "PATH=";
        new_path += ".:";
        if (current_path) {
            new_path += current_path;
        }
        putenv(const_cast<char *>(new_path.c_str()));

        std::vector<char *> c_args;
        for (const auto &arg : args) {
            c_args.push_back(const_cast<char *>(arg.c_str()));
        }
        c_args.push_back(nullptr);

        execvp(c_args[0], c_args.data());
        std::cerr << args[0] << ": command not found" << std::endl;
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
    }
}

void shell_loop() {
    while (true) {
        std::cout << ">: ";
        std::string input;
        if (!std::getline(std::cin, input)) {
            break;
        }

        if (is_exit_command(input)) {
            break;
        }

        std::vector<std::string> args;
        split_input(input, args);

        if (args.empty()) {
            std::cout << "Error: No command entered" << std::endl;
            continue;
        }

        bool silent = false;
        if (args[0] == "silent") {
            silent = true;
            args.erase(args.begin());
        }

        execute_command(args, silent);
    }
}

int main() {
    shell_loop();
    return 0;
}
