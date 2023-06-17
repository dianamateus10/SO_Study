//Escreva um programa que crie um pipe anónimo e de seguida crie um processo
//filho. Experimente o pai enviar um inteiro através do descritor de escrita 
//do pipe, e o filho receber um inteiro a partir do respetivo descritor de leitura.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE sizeof("Get this parent\n")

int main() {
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t child = fork();
    if (child == 0)
    {
        close(fd[1]);
        char buffer[SIZE];
        if(read(fd[0], buffer, SIZE) < 0)
        {
            perror("Child: couldn't read from pipe");
        }
        write(1, buffer, SIZE);
        close(fd[0]);
        _exit(0);
    }
    else
    {
        close(fd[0]);
        if(write(fd[1], "Get this parent\n", SIZE) < 0)
        {
            perror("Parent: couldn't write to pipe");
        }
        close(fd[1]);
    }
    wait(NULL);

    return 0;
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
        close(pipefd[1]);  // Close the write end of the pipe

        int received_int;
        read(pipefd[0], &received_int, sizeof(int));  // Read an integer from the pipe

        printf("Child received: %d\n", received_int);

        close(pipefd[0]);  // Close the read end of the pipe in the child process
    } else {
        // Parent process
        close(pipefd[0]);  // Close the read end of the pipe

        int number = 123;
        write(pipefd[1], &number, sizeof(int));  // Write an integer to the pipe

        printf("Parent sent: %d\n", number);

        close(pipefd[1]);  // Close the write end of the pipe in the parent process
    }

    return 0;
}
*/