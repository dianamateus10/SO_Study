#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

//Implemente um programa que crie um processo filho. Pai e filho devem imprimir
//o seu identificador de processo e o do seu pai. O pai deve ainda imprimir o PID
//do seu filho.

int main(int argc, char *argv[]) 
{
    pid_t pid = fork();

    if (pid == 0) {
        // Processo filho
        printf("Filho - PID: %d\n", getpid());
        printf("Filho - PPID: %d\n", getppid());
    } else {
        // Processo pai
        printf("Pai - PID: %d\n", getpid());
        printf("Pai - PPID: %d\n", getppid());
        printf("Pai - PID do Filho: %d\n", pid);
    }

    return 0;
}