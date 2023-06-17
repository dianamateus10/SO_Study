#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

//Implemente um programa que crie dez processos filhos que deverão executar
//em concorrência. O pai deverá esperar pelo fim da execução de todos os seus
//filhos, imprimindo os respetivos códigos de saída.

int main(int argc, char *argv[]) 
{
    
    for (int i = 0; i < 10; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Processo filho
            printf("Filho %d - PID: %d\n", i, getpid());
            printf("Filho %d - PPID: %d\n", i, getppid());
            return i;
        }
    }

    for (int i = 0; i < 10; i++) {
        int status;
        wait(&status);
        printf("Pai - PID: %d\n", getpid());
        printf("Pai - PPID: %d\n", getppid());
        printf("Pai - Código de saída do Filho: %d\n", WEXITSTATUS(status));
    }

    return 0;
}