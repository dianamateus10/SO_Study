#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>

//Implemente um progama que imprima o seu identificador de processo e do seu pai.
//Comprove inovando o comando ps -  que o pai do seu processo é o interpretador de
//comandos que utilizou para o executar

int main(int argc, char *argv[]) 
{
    pid_t pid = getpid();
    pid_t ppid = getppid();

    printf("PID: %d\n", pid);
    printf("PPID: %d\n", ppid);

    return 0;
}