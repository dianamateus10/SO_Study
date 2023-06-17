//Modifique o programa anterior de mode a que, depois de realizar os
//redirecionamentos, seja criado um novo processo que realize operações
//de leitura e escrita. Observe o conteúdo dos ficheiros. Repare que o 
//processo filho "nasce" com as mesmas associações de descritores de 
//ficheiros do processo pai

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main()
{
    int terminal = dup(1);

    int fd = open("/etc/passwd", O_RDONLY, 0640);
    int fd1 = open("saida.txt", O_CREAT | O_RDWR | O_TRUNC, 0640);
    int fd2 = open("erros.txt", O_CREAT | O_RDWR | O_TRUNC, 0640);

    dup2(fd, 0);
    close(fd);

    dup2(fd1, 1);
    close(fd1);

    dup2(fd2, 2);
    close(fd2);

    pid_t child = fork();
    if(child == 0)
    {
        char buffer[1024];
        
        ssize_t bytes = read(0, buffer, 1024);
        if(bytes < 0)
        {
            perror("read error");
            _exit(0);
        }

        write(1, buffer, bytes);
        _exit(0);
        
    }
    else
    {
        int status;
        child = wait(&status);
        if(WIFEXITED(status) && WEXITSTATUS(status) != 1)
        {
            printf("child %d terminated with exit code %d\n", child, WEXITSTATUS(status));
        }
        else
        {
            printf("child %d terminated abnormally\n", child);
        }
    }

    write(terminal, "terminei\n", 9);
}