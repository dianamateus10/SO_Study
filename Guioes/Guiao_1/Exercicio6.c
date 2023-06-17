#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NOME 256

typedef struct {
    char nome[MAX_NOME];
    int idade;
} Pessoa;

int main(int argc, char *argv[]) {
    int fd = open("pessoas.bin", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    switch(argv[1][2])
    {
        case 'i':
        {
            Pessoa pessoa;
            pessoa.idade = atoi(argv[3]);
            memcpy(pessoa.nome, argv[2], strlen(argv[2]));
            lseek(fd, 0, SEEK_END);
            write(fd, &pessoa, sizeof(Pessoa));
            break;
        }
        case 'u':
        {
            Pessoa pessoa;
            while (read(fd, &pessoa, sizeof(Pessoa)) == sizeof(Pessoa)) {
                if (strcmp(pessoa.nome, argv[2]) == 0) 
                {
                    pessoa.idade = atoi(argv[3]);
                    lseek(fd, -sizeof(Pessoa), SEEK_CUR);
                    write(fd, &pessoa, sizeof(Pessoa));
                    break;
                }
            }
            break;
        }
        case 'o':
        {
            lseek(fd, sizeof(Pessoa) * atoi(argv[3]), SEEK_CUR);
            Pessoa pessoa;
            read(fd, &pessoa, sizeof(Pessoa));
            pessoa.idade = atol(argv[3]);
            lseek(fd, -sizeof(Pessoa), SEEK_CUR);
            write(fd, &pessoa, sizeof(Pessoa));
        }
    }
    close(fd);

    return 0;
}