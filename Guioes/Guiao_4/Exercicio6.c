//Pretende-se determinar todas as ocorrências de um determinado número inteiro nas
//linhas de uma matriz de números inteiros, em que o número de colunas é muito maior
//do que o número de linhas. Implemente, utilizando processos e pipes, uma função que
//devolva num vetor todas as ocorrências encontradas. A matriz inicial, o valor a procurar
//e o vetor onde guardar os resultados devem ser fornecidos como parâmetros.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>  // For srand()

#define ROWS 10
#define COLS 100

void findOccurrences(int matrix[ROWS][COLS], int value, int result[], int* count) {
    int pipefd[2];  // File descriptors for the pipe

    // Create the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create child processes
    for (int i = 0; i < ROWS; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Child process
            close(pipefd[0]);  // Close the read end of the pipe

            int local_count = 0;

            for (int j = 0; j < COLS; j++) {
                if (matrix[i][j] == value) {
                    int occurrence = i * COLS + j;  // Calculate the occurrence index
                    write(pipefd[1], &occurrence, sizeof(int));
                    local_count++;
                }
            }

            close(pipefd[1]);  // Close the write end of the pipe in the child process
            exit(local_count);
        }
    }

    close(pipefd[1]);  // Close the write end of the pipe in the parent process

    int status;
    *count = 0;

    while (wait(&status) > 0) {
        if (WIFEXITED(status)) {
            int child_count = WEXITSTATUS(status);
            int received_occurrence;

            for (int i = 0; i < child_count; i++) {
                read(pipefd[0], &received_occurrence, sizeof(int));
                result[*count] = received_occurrence;
                (*count)++;
            }
        }
    }

    close(pipefd[0]);  // Close the read end of the pipe in the parent process
}

int main() {
    int matrix[ROWS][COLS];

    // Populate the matrix with random values between 1 and 10
    srand(time(NULL));
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            matrix[i][j] = rand() % 10 + 1;
        }
    }

    int value = 8;
    int occurrences[ROWS * COLS];
    int count;

    findOccurrences(matrix, value, occurrences, &count);

    printf("Occurrences of %d in the matrix: %d\n", value, count);
    for (int i = 0; i < count; i++) {
        int row = occurrences[i] / COLS;    // Calculate the row index
        int col = occurrences[i] % COLS;    // Calculate the column index
        printf("Row: %d, Column: %d, Number: %d\n", row, col, matrix[row][col]);
    }

    return 0;
}

