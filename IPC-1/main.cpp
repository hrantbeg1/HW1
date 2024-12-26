#include <pwd.h>
#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>
#include <csignal>
#include <cstring>
#include <iostream>

void signal_handler(int signum, siginfo_t *info, void *context) {
    struct passwd *pw = getpwuid(info->si_uid);

    std::cout << "Received a SIGUSR1 signal from process [" 
              << info->si_pid 
              << "] executed by [" 
              << info->si_uid 
              << "] ([";

    if (pw != nullptr) {
        std::cout << pw->pw_name;
    } else {
        std::cout << "unknown";
    }

    std::cout << "]).\n";

    ucontext_t *uc = (ucontext_t *)context;

    std::cout << "State of the context: RIP = [" 
              << uc->uc_mcontext.gregs[REG_RIP] 
              << "], RAX = [" 
              << uc->uc_mcontext.gregs[REG_RAX] 
              << "], RBX = [" 
              << uc->uc_mcontext.gregs[REG_RBX] 
              << "].\n";
}

int main() {
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    std::cout << "Signal Echo Program started with PID: " << getpid() << "\n";

    while (true) {
        sleep(10);
    }

    return 0;
}
