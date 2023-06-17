//Experimente de seguida provocar um atraso antes do pai enviar o inteiro
//(sleep(5)). Note agora que a leitura do filho bloqueia enquanto o pai não
//realizar a operação de escrita no pipe.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SIZE sizeof("Get this parent\n")
#define BUFFER 64

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

        ssize_t nbytes = 0;
        for(char *buf_ptr = buffer; 
            (nbytes = read(fd[0], buf_ptr, BUFFER)) > 0; 
            buf_ptr += nbytes);
        close(fd[0]);
        write(1, buffer, SIZE);
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

        sleep(5);  // Introduce a delay of 5 seconds

        int number = 123;
        write(pipefd[1], &number, sizeof(int));  // Write an integer to the pipe

        printf("Parent sent: %d\n", number);

        close(pipefd[1]);  // Close the write end of the pipe in the parent process
    }

    return 0;
}

*/