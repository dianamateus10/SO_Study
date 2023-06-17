//Pretende-se um programa que leia um ficheiro de texto muito grande
//dimensao e imprima as linhas que contém uma palavra-chave. Use a
//função strstr() para procurar a palavra em cada linha. Use vários 
//processos e evite armazenar o ficheiro todo em memória.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHUNK_SIZE 1024   // Chunk size in bytes

void searchKeyword(const char* filename, const char* keyword) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    char line[CHUNK_SIZE];
    int line_number = 1;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (strstr(line, keyword) != NULL) {
            printf("Line %d: %s", line_number, line);
        }
        line_number++;
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <filename> <keyword>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    const char* keyword = argv[2];

    // Count the number of lines in the file
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("fopen");
        return 1;
    }

    int total_lines = 0;
    char line[CHUNK_SIZE];
    while (fgets(line, sizeof(line), file) != NULL) {
        total_lines++;
    }
    fclose(file);

    // Determine the number of chunks and lines per chunk
    int num_processes = total_lines < sysconf(_SC_NPROCESSORS_ONLN) ? total_lines : sysconf(_SC_NPROCESSORS_ONLN);
    int lines_per_chunk = total_lines / num_processes;
    int remaining_lines = total_lines % num_processes;

    int start_line = 1;

    for (int i = 0; i < num_processes; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {
            // Child process
            int lines_to_read = lines_per_chunk;

            if (i == num_processes - 1) {
                // Last process should read remaining lines
                lines_to_read += remaining_lines;
            }

            int end_line = start_line + lines_to_read - 1;

            printf("Process ID: %d, Searching lines %d to %d\n", getpid(), start_line, end_line);
            searchKeyword(filename, keyword);

            exit(EXIT_SUCCESS);
        }

        start_line += lines_per_chunk;
    }

    // Wait for all child processes to finish
    while (num_processes > 0) {
        wait(NULL);
        num_processes--;
    }

    return 0;
}
