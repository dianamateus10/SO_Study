//Escreva um programa que redirecione o descritor associado ao seu 
//standard input para o ficheiro /etc/passwd e o standard output e 
//error para saida.txt e erros.txt, respectivamente. Imediatamente
//antes do programa terminar, o mesmo deve imprimir no terminal a 
//mensagem "terminei"

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main()
{
    int terminal = dup(1);

    int fd = open("/etc/passwd", O_RDONLY, 0640);
    int fd1 = open("saida.txt", O_CREAT | O_WRONLY | O_TRUNC, 0640);
    int fd2 = open("erros.txt", O_CREAT | O_WRONLY | O_TRUNC, 0640);

    dup2(fd, 0);
    close(fd);

    dup2(fd1, 1);
    close(fd1);

    dup2(fd2, 2);
    close(fd2);

    write(terminal, "terminei\n", 9);

}