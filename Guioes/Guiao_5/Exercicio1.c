//Escreva três programas que ilustrem a operação de pipes com nomes.
//O primeiro cria um pipe com o nome "fifo".
//O segundo repete para este pipe todas as linhas de texto lidas a partir
//do standard input
//O terceiro repete para o seu standard output todas as linhas de texto lidas
//a partir deste mesmo pipe.
//Note que a abertura para escrita de um pipe com nome bloqueia até que um 
//processo o abra para leitura, e vice-versa.

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

//primeiro programa
/*
int main()
{
    if(mkfifo("fifo", 0666) < 0)
    {
        perror("mkfifo");
        exit(1);
    }
}*/

//segundo programa
/*int main()
{
    int fifo = open("./fifo", O_WRONLY);
    if(fifo < 0)
    {
        perror("open");
        exit(1);
    }

    char buffer[1024];

    ssize_t bytes_read = 0;
    while((bytes_read = read(0, buffer, 1024)) > 0)
    {
        if(write(fifo, buffer, bytes_read) < 0)
        {
            perror("write");
            exit(1);
        }
    }

    close(fifo);

}*/

//terceiro programa
int main()
{
    int fifo = open("./fifo", O_RDONLY);
    if(fifo < 0)
    {
        perror("open");
        exit(1);
    }

    char buffer[1024];

    ssize_t bytes_read = 0;
    while((bytes_read = read(fifo, buffer, 1024)) > 0)
    {
        if(write(1, buffer, bytes_read) < 0)
        {
            perror("write");
            exit(1);
        }
    }

    close(fifo);

}