#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int fifo = open("./fifo2", O_WRONLY, 0666);
    if(fifo < 0)
    {
        perror("open");
        exit(1);
    }

    write(fifo, argv[1], strlen(argv[1]));
    close(fifo);

    return 0;
}