#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

ssize_t readln(int fd, char *line, size_t size) 
{
    ssize_t bytesRead = 0;
    ssize_t totalBytesRead = 0;

    while (totalBytesRead < size - 1) 
    {
        bytesRead = read(fd, line + totalBytesRead, 1);

        if (bytesRead == -1) 
        {
            // Ocorreu um erro durante a leitura
            return -1;
        } else if (bytesRead == 0) 
        {
            //Fim do arquivo
            break;
        } 
        else if (line[totalBytesRead] == '\n') 
        {
            //Fim da linha
            break;
        }

        totalBytesRead += bytesRead;
    }

    line[totalBytesRead] = '\0';

    return totalBytesRead;
}

int main() 
{
    char line[256];
    ssize_t bytesRead;
    int line_number = 0;

    while(bytesRead = readln(0, line, sizeof(line)))
    {
        if(bytesRead > 0)
        {
            line_number += 1;
            printf("Número da linha: %d\n", line_number);
        }
    }

    return 0;
}