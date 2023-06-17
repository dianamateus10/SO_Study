//Escreva um programa "servidor" que fique a correr em background e
//acrescente a um ficheiro de log todas as mensagens que lhe são enviadas
//por "clientes". Escreva um programa cliente que envia para o servido o 
//seu argumento.

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int log = open("./log.txt", O_TRUNC | O_CREAT | O_WRONLY, 0640);
    int fifo = mkfifo("./fifo2", 0666);

    for(char buffer[1024];;)
    {
        if (fork() == 0)
        {
            fifo = open("./fifo2", O_RDONLY, 0666);
            for(ssize_t bytes = 0; bytes = read(fifo, buffer, 1024);)
            {
                write(log, buffer, bytes);
            }
            close(fifo);
        }

        wait(NULL);
    }

    close(log);

}