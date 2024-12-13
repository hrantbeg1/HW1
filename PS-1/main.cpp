#include <sys/wait.h>
#include <unistd.h>

#include <chrono>
#include <iostream>

void do_command(char** argv) {
    auto start_time = std::chrono::steady_clock::now();
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to fork a new process.\n";
        return;
    }
    if (pid == 0) {
        execvp(argv[0], argv);
        std::cerr << "Failed to execute command: " << argv[0] << "\n";
        _exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        auto end_time = std::chrono::steady_clock::now();
        std::chrono::duration<double> duration = end_time - start_time;
        if (WIFEXITED(status)) {
            std::cout << "Command completed with " << WEXITSTATUS(status)
                      << " exit code and took " << duration.count() << " seconds.\n";
        } else if (WIFSIGNALED(status)) {
            std::cout << "Command terminated by signal " << WTERMSIG(status)
                      << " and took " << duration.count() << " seconds.\n";
        } else {
            std::cout << "Command execution failed in an unknown manner.\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./do-command <command> [args...]\n";
        return 1;
    }
    do_command(argv + 1);
    return 0;
}
