#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

int main() {
    // pipe declaration
    int fd[2];
    if(pipe(fd) == -1){
        fprintf(stderr,"Pipe failed");
        return 1;
    }


    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }


    if(pid > 0){
        close(fd[0]);  //Closing read end of pipe in parent
        // Parent process
        int n;
        printf("Enter an integer n: ");
        scanf("%d", &n);

        write(fd[1], &n, sizeof(n));  // Send value of n to child

        if(n < 0){
            printf("Please enter a non-negative integer.\n");
            exit(EXIT_FAILURE);
        }

        int array[] = {3,15,4,6,7,17,9,2};
        int size = sizeof(array) / sizeof(array[0]);

        // To check if an index is visited or not
        int visited[size];
        for(int i = 0; i < size; i++){
            visited[i] = 0;
        } 

        int visited_count = 0;

        srand(time(NULL));   // Random number generator

        while (visited_count < size) {
            int index = rand() % size;   // random index
            if(!visited[index]){         // mark as visited
                visited[index] = 1;
                visited_count++;
            }
            
            int x = array[index];

            // printf("Parent: Sending %d\n", x);
            write(fd[1], &x, sizeof(x));

            sleep(x % n);   // parent sleep logic
        }
        close(fd[1]);  // Closing write end of pipe in parent
        wait(NULL);    // Waiting for child process to finish
    }
    else{
        // Child process
        close(fd[1]);  // Closing write end of pipe in child
        int received_number;
        int n;

        read(fd[0],&n,sizeof(n));  // Read n from parent
        while(read(fd[0], &received_number, sizeof(received_number))){
            // printf("Child: Received %d\n", received_number);
            printf("Factors of %d: ", received_number);
            for(int i = 1; i <= received_number; i++){
                if(received_number % i == 0){
                    printf("%d ", i);
                }
            }
            printf("\n");
            sleep(time(NULL) % n);  // Child sleep logic
        }
        
        close(fd[0]);  // Closing read end of pipe in child
        
    }
    return 0;
}
