//Modifique o programa anterior de modo à leitura do pipe ser realizada
//enquanto não for detetada a situação de end of file. Repare que esta
//situação acontece apenas quando nenhum processo - neste caso, pai e filho
//têm aberto o descritor de escrita do pipe.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFFER 1024

int main()
{
    int fd[2];
    pipe(fd);

    if(fork() == 0)
    {
        close(fd[0]);

        dup2(fd[1], 1);
        close(fd[1]);

        execlp("ls", "ls", "-l", NULL);

        _exit(1);
    }

    if(fork() == 0)
    {
        close(fd[1]);

        dup2(fd[0], 0);
        close(fd[0]);

        execlp("wc", "wc", "-l", NULL);

        _exit(1);
    }

    close(fd[0]);
    close(fd[1]);
    wait(NULL);
    wait(NULL);

    return 0;
}

/*
int main()
{
    static char *commands_grep[] = {"grep", "-v", "^#", "/etc/passwd", NULL};
    static char *commands_cut[] = {"cut", "-f7", "-d:", NULL};
    static char *commands_uniq[] = {"uniq", NULL};
    static char *commands_wc[] = {"wc", "-l", NULL};

    static char *commands[] = { commands_grep, commands_cut, commands_uniq, commands_wc};

    int fd[3][2];

    for(int i = 0; i < 4; i++)
    {
        if(i == 0)
        {
            pipe(fd[i]);
            if(fork() == 0)
            {
                dup2(fd[i][1], 1);
                close(fd[i][0]);
                close(fd[i][1]);

                execvp(commands[i][0], commands[i]);
                _exit(1);
            }
            close(fd[i][1]);
        }
        else if (i < 3)
        {
            pipe(fd[i]);
            if(fork() == 0)
            {
                dup2(fd[i-1][0], 0);
                close(fd[i-1][0]);
                dup2(fd[i][1], 1);
                close(fd[i][0]);
                close(fd[i][1]);

                execvp(commands[i][0], commands[i]);
                _exit(1);
            }
            close(fd[i-1][0]);
            close(fd[i][1]);

        }
        else
        {
            if(fork() == 0)
            {
                dup2(fd[i-1][0], 0);
                close(fd[i-1][1]);
                close(fd[i-1][0]);

                execvp(commands[i][0], commands[i]);
                _exit(1);
            }
            close(fd[i-1][0]);
        }
    }

    for(size_t i = 0; i < 4; i++)
    {
        wait(NULL);
    }
}
*/

/**
 #include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

        int numbers[] = {123, 456, 789};
        int num_count = sizeof(numbers) / sizeof(int);

        // Write a sequence of integers to the pipe
        for (int i = 0; i < num_count; i++) {
            write(pipefd[1], &numbers[i], sizeof(int));
            printf("Child sent: %d\n", numbers[i]);
            sleep(2);  // Introduce a delay between each write operation
        }

        close(pipefd[1]);  // Close the write end of the pipe in the child process
    } else {
        // Parent process
        close(pipefd[1]);  // Close the write end of the pipe

        int received_int;

        // Read integers from the pipe until end-of-file
        while (read(pipefd[0], &received_int, sizeof(int)) > 0) {
            printf("Parent received: %d\n", received_int);
            sleep(1);  // Introduce a delay before each read operation
        }

        close(pipefd[0]);  // Close the read end of the pipe in the parent process
    }

    return 0;
}

*/