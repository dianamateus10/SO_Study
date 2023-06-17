#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

//Implemente um programa que crie dez processos filhos que deverão executar
//sequencialmente. Para este efeito, podem imprimir o seu PID e o do seu pai, 
//e, finalmente, terminarem a sua execução com um valor de saída igual ao seu
//número de ordem (e.g., o primeiro filho termina com o valor 1, o segundo com
// O pai deverá imprimir o código de saída de cada filho.

int main(int argc, char *argv[]) 
{
    for (int i = 0; i < 10; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Processo filho
            printf("Filho %d - PID: %d\n", i, getpid());
            printf("Filho %d - PPID: %d\n", i, getppid());
            return i;
        } else {
            // Processo pai
            int status;
            wait(&status);
            printf("Pai - PID: %d\n", getpid());
            printf("Pai - PPID: %d\n", getppid());
            printf("Pai - PID do Filho: %d\n", pid);
            printf("Pai - Código de saída do Filho: %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}