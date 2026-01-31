#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>



int main() {

    int n, k, r;
    printf("Enter n: ");
    scanf("%d", &n);
    printf("Enter k: ");
    scanf("%d", &k);
    printf("Enter r: ");
    scanf("%d", &r);

    int cmd_pipe[2], sync_pipe[2];
    pipe(cmd_pipe);   // parent → child
    pipe(sync_pipe);  // child → parent

    pid_t pid = fork();

    // Child process
    if (pid == 0) {
        close(cmd_pipe[1]);   // child reads
        close(sync_pipe[0]);  // child writes

        int iteration = 0;

        while (1) {
            int fd[2];
            pipe(fd);

            // child 1 to get ps output
            if (fork() == 0) {
                dup2(fd[1], STDOUT_FILENO);  // write to stdout 
                close(fd[0]); close(fd[1]);

                execlp("ps", "ps",
                       "-eo", "user,pid,%mem,time",
                       "--sort=-%mem", NULL);
                exit(1);
            }
            
            // child 2 to get head k output
            if (fork() == 0) {
                dup2(fd[0], STDIN_FILENO);  // read from stdin
                close(fd[0]); close(fd[1]);

                char kstr[16];
                snprintf(kstr, sizeof(kstr), "%d", k + 1);
                execlp("head", "head", "-n", kstr, NULL);
                exit(1);
            }

            close(fd[0]); close(fd[1]);
            wait(NULL); wait(NULL);

            iteration++;

            // control point
            if (iteration % r == 0) {
                int signal = 1;
                write(sync_pipe[1], &signal, sizeof(signal));

                int pid_to_kill;
                read(cmd_pipe[0], &pid_to_kill, sizeof(pid_to_kill));

                if (pid_to_kill == -2)
                    exit(0);    //stops execution of the program
                else if (pid_to_kill != -1)
                    kill(pid_to_kill, SIGKILL); 
            }

            sleep(n);
        }
    }

    // parent process
    else {
        close(cmd_pipe[0]);   // parent writes
        close(sync_pipe[1]);  // parent reads

        while (1) {
            int signal;
            read(sync_pipe[0], &signal, sizeof(signal));

            int pid_input;
            printf("\nEnter PID (-1 skip, -2 exit): ");
            scanf("%d", &pid_input);

            write(cmd_pipe[1], &pid_input, sizeof(pid_input)); // send pid to child

            if (pid_input == -2)
                break;  
        }

        wait(NULL);
    }

    return 0;
}
