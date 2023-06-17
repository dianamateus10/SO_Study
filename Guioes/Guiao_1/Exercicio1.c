#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    //Implemente um programa mycp com funcionalidade semelhante ao comando cp.
    //Varie o tamanho do buffer e medir o tempo necessário para copiar um ficheiro de grande dimensão.

    int fread = open("ficheiro.txt", O_RDONLY, 0660);
    if  (fread < 0)
    {
        perror("Error opening file");
        close(fread);
        return -1;
    }

    int fwrite = open("copy.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    if(fwrite < 0)
    {
        perror("Error opening file");
        close(fwrite);
        return -1;
    }

    unsigned char buffer[1024];

    for (ssize_t nbytes = read(fread, buffer, 1024); nbytes>0; nbytes = read(fread, buffer, 1024))
    {
        write(fwrite, buffer,  nbytes);
    }

    close(fread);
    close(fwrite);

    return 0;
}

