//Implemente um programa semelhante ao anterior que execute o mesmo
//comando mas agora no contexto de um processo filho

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) 
{
    pid_t child = fork();
    if(child == 0)
    {
        char *args[] = {"ls", "-l", NULL};
        execvp(args[0], args);
        _exit(1);
    }
    else
    {
        int status;
        wait(&status);
    }

    return 0;
}