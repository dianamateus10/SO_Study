//Implemente um programa que execute concorrentemente uma lista de executáveis
//especificados com argumentos da linha de comando. Considere os executáveis
//sem quaisquer argumentos próprios. O programa deverá esperar pelo fim da
//execução de todos os processos por si criados.

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) 
{
    for(size_t i = 1; i <= argc; i++)
    {
        pid_t child = fork();
        if(child == 0)
        {
            char *args[] = {argv[i], NULL};
            execvp(args[0], args);
            _exit(1);
        }
    }

    for(size_t i = 1; i <= argc; i++)
    {
        int status;
        pid_t child = wait(&status);
        if(WIFEXITED(status))
            printf("Child %d exited with status %d\n", child, WEXITSTATUS(status));
    }

    return 0;
}