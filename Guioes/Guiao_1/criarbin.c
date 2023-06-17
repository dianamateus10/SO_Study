#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição da estrutura para uma pessoa
typedef struct {
    char nome[50];
    int idade;
    // Outros campos podem ser adicionados aqui
} Pessoa;

int main() {
    Pessoa pessoas[] = {
        {"João Pedro", 25},
        {"Maria Joaquina", 30},
        {"Pedro Costa", 40},
        {"Jose Mourinho", 58},
        {"Beatriz Andrade", 28},        
        // Adicione mais pessoas aqui, se desejar
    };

    int numPessoas = sizeof(pessoas) / sizeof(Pessoa);

    // Abre o arquivo binário para escrita em modo binário
    FILE *arquivo = fopen("pessoas.bin", "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    // Escreve os registros de pessoas no arquivo
    for (int i = 0; i < numPessoas; i++) {
        fwrite(&pessoas[i], sizeof(Pessoa), 1, arquivo);
    }

    // Fecha o arquivo
    fclose(arquivo);

    printf("Arquivo pessoas.bin criado com sucesso.\n");

    return 0;
}
