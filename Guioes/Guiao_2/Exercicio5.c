#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

//Pretende-se determinar a existência de um determinado número inteiro
//nas linhas de uma matriz de números inteiros, em que o número de colunas
//é muito maior do que o número de linhas. Implemente, usando processos, um
//programa que determine a existência de um determinado número, recebido como
//argumento, numa matriz gerada aleatoriamente.

#define ROWS 10
#define COLS 100

int matrix[ROWS][COLS];

int searchNumber(int number, int startRow, int endRow) {
    for (int i = startRow; i < endRow; i++) {
        for (int j = 0; j < COLS; j++) {
            if (matrix[i][j] == number) {
                return 1; // Número encontrado
            }
        }
    }
    return 0; // Número não encontrado
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: ./programa <número>\n");
        return 1;
    }

    int number = atoi(argv[1]);
    int num_processes = 4; // Número de processos a serem criados
    int rows_per_process = ROWS / num_processes; // Linhas por processo

    // Preenche a matriz com números aleatórios
    srand(time(NULL));
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            matrix[i][j] = rand() % 1000;
        }
    }

    pid_t pid;
    int status;
    int found = 0; // Indica se o número foi encontrado

    for (int i = 0; i < num_processes; i++) {
        pid = fork();

        if (pid < 0) {
            fprintf(stderr, "Erro ao criar processo filho\n");
            return 1;
        } else if (pid == 0) {
            // Código a ser executado pelo processo filho
            int startRow = i * rows_per_process;
            int endRow = startRow + rows_per_process;
            int result = searchNumber(number, startRow, endRow);
            exit(result);
        }
    }

    // Código a ser executado pelo processo pai
    for (int i = 0; i < num_processes; i++) {
        pid_t terminated_pid = wait(&status);

        if (WIFEXITED(status)) {
            int result = WEXITSTATUS(status);
            if (result == 1) {
                found = 1;
                break;
            }
        }
    }

    if (found) {
        printf("O número %d foi encontrado na matriz.\n", number);
    } else {
        printf("O número %d não foi encontrado na matriz.\n", number);
    }

    return 0;
}
