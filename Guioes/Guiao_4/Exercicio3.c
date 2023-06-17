//Experimente agora inverter os papeis de modo à informação ser transmitida
//do filho para o pai

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define STRING \"Get this parent\n"
#define SIZE sizeof(STRING)
#define BUFFER 1024

int main()
{
    int fd[2];
    pipe(fd);
    pid_t child = fork();

    if(child == 0)
    {
        close(fd[1]);
        dup2(fd[0], 0);
        close(fd[0]);

        execlp("wc", "wc", NULL);
        _exit(1);
    }
    
    close(fd[0]);
    char buffer[BUFFER];

    for(ssize_t _read = 0; 
        (_read = read(0, buffer, BUFFER)) > 0; 
        write(fd[1], buffer, _read));
    close(fd[1]);
    wait(NULL);
}

/*
int main() {
    int pipefd[2];  // File descriptors for the pipe

    // Create the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create a child process
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Child process
        close(pipefd[0]);  // Close the read end of the pipe

        int number = 123;
        write(pipefd[1], &number, sizeof(int));  // Write an integer to the pipe

        printf("Child sent: %d\n", number);

        close(pipefd[1]);  // Close the write end of the pipe in the child process
    } else {
        // Parent process
        close(pipefd[1]);  // Close the write end of the pipe

        int received_int;
        read(pipefd[0], &received_int, sizeof(int));  // Read an integer from the pipe

        printf("Parent received: %d\n", received_int);

        close(pipefd[0]);  // Close the read end of the pipe in the parent process
    }

    return 0;
}

*/