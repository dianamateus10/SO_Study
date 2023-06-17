#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    //Implemente um programa mycat com funcionalidade semelhante ao comando cat.
    //Lê de um stdin e escreve para um stdout.

    unsigned char buffer[1024];

    for (ssize_t escrita = read(0, buffer, 1024); escrita>0; escrita = read(0, buffer, 1024))
    {
        write(1, buffer,  escrita);
    }
    
    return 0;
}