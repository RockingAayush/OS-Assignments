#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int pid = fork();

    if (pid < 0) {
        // Fork failed
        perror("Fork failed");
        return 1;
    } else if (pid == 0) {
        // Child process
        printf("Hello from the child process! PID: %d\n", getpid());
    } else {
        // Parent process
        printf("Hello from the parent process! Child PID: %d\n", pid);
    }

    return 0;
}