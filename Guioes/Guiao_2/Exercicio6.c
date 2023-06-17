#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <stdbool.h>

#define ROWS 10
#define COLS 100

int matrix[ROWS][COLS];

int main(int argc, char *argv[]) 
{
    srand(time(NULL));

    for (size_t i = 0; i < ROWS; i++) {
        for (size_t j = 0; j < COLS; j++) {
            matrix[i][j] = rand() % 100;
        }
    }
    
    int number = atoi(argv[1]);
    for(size_t i=0; i < ROWS; i++) {
        pid_t child = fork();
        if(child == 0)
        {
            for(size_t j=0; j < COLS; j++) {
                if(matrix[i][j] == number) {
                    _exit(i+10);
                }
            }
            _exit(i);
        }
    }

    bool found = false;
    bool print[ROWS] = {false};
    for(size_t i=0; i < ROWS; i++) {
        int status;
        (void) wait(&status);
        if(WIFEXITED(status)) {
            int result = WEXITSTATUS(status);
            if(result > 10) {
                print[result-10] = true;
                found = true;
            }
        }
    }
    for(size_t i=0; i < ROWS; i++) {
        if(print[i]) {
            printf("Linha %ld\n", i);
        }
    }
    if(!found) {
        printf("Não encontrado\n");
    }
}
