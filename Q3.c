#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    if (argc < 3) {
        printf("Usage: mygrep pattern file1 [file2 ...]\n");
        exit(1);
    }

    int p1[2], p2[2], p3[2];
    pipe(p1);
    pipe(p2);
    pipe(p3);

    /* -------- grep -------- */
    if (fork() == 0) {
        dup2(p1[1], STDOUT_FILENO);
        close(p1[0]); close(p1[1]);
        execlp("grep", "grep", "-n", argv[1], argv[2], argv[3], NULL);
        perror("grep failed");
    }

    /* -------- cut -------- */
    if (fork() == 0) {
        dup2(p1[0], STDIN_FILENO);
        dup2(p2[1], STDOUT_FILENO);
        close(p1[0]); close(p1[1]);
        close(p2[0]); close(p2[1]);
        execlp("cut", "cut", "-d:", "-f2", NULL);
        perror("cut failed");
    }

    /* -------- sort -------- */
    if (fork() == 0) {
        dup2(p2[0], STDIN_FILENO);
        dup2(p3[1], STDOUT_FILENO);
        close(p2[0]); close(p2[1]);
        close(p3[0]); close(p3[1]);
        execlp("sort", "sort", "-n", NULL);
        perror("sort failed");
    }

    /* -------- uniq -------- */
    if (fork() == 0) {
        dup2(p3[0], STDIN_FILENO);
        close(p3[0]); close(p3[1]);
        execlp("uniq", "uniq", NULL);
        perror("uniq failed");
    }

    /* Parent closes all pipes */
    close(p1[0]); close(p1[1]);
    close(p2[0]); close(p2[1]);
    close(p3[0]); close(p3[1]);

    /* Wait for all children */
    for (int i = 0; i < 4; i++)
        wait(NULL);

    return 0;
}
