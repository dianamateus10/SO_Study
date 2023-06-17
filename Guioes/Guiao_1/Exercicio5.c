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

void adicionarPessoa(const char* nome, int idade, int fd) {
    Pessoa pessoa;
    pessoa.idade = idade;
    memcpy(pessoa.nome, nome, strlen(nome) + 1);
    lseek(fd, 0, SEEK_END);
    write(fd, &pessoa, sizeof(Pessoa));
}

void atualizarIdade(const char* nome, int novaIdade, int fd) {
    Pessoa pessoa;

    while (read(fd, &pessoa, sizeof(Pessoa)) == sizeof(Pessoa)) {
        if (strcmp(pessoa.nome, nome) == 0) {
            pessoa.idade = novaIdade;
            lseek(fd, -sizeof(Pessoa), SEEK_CUR);
            write(fd, &pessoa, sizeof(Pessoa));
        }
    }
}

int main(int argc, char *argv[]) {
    const char* opcao = argv[1];
    const char* nome = argv[2];
    int idade = atoi(argv[3]);

    int fd = open("pessoas.bin", O_RDWR | O_CREAT, 0600);

    if (fd == -1) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    if (strcmp(opcao, "-i") == 0) {
        adicionarPessoa(nome, idade, fd);
        printf("Pessoa adicionada com sucesso.\n");
    } else if (strcmp(opcao, "-u") == 0) {
        atualizarIdade(nome, idade, fd);
        printf("Idade atualizada com sucesso.\n");
    } else {
        printf("Opção inválida.\n");
    }

    close(fd);

    return 0;
}
