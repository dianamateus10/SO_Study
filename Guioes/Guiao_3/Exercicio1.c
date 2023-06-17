//Implemente um programa que execute o comando ls -l. Note que no caso
//da execução ser bem sucedida, mais nenhuma outra instrução é executada
//do programa original.

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) 
{
    char *args[] = {"ls", "-l", NULL};
    execvp(args[0], args);

    return 0;
}