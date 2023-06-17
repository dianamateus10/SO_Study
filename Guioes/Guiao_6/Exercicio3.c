//Modifique novamente o programa inicial de modo a que seja executado
//o comando wc sem argumentos, depois do redirecionamento dos descritores
// de entrada e saída. Note que as associações e redirecionamentos de
//descritores de ficheiros sao preservados na primitiva exec()

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

    int worked = execl("wc", "wc", NULL);
    if(worked < 0)
    {
        perror("exec error");
        _exit(1);
    }
    else
    {
        printf("exec worked\n");
    }

    write(terminal, "TERMINEI\n", 9);
}