/**
 * 1. Um dos primeiros sistemas interativos multi-utilizador usada Multilevel Queues
 * com fatias de tempo de cerca de 100ms que iam aumentando à medida que a prioridade
 * descia. Uma vez por segundo transferiam-se todos os processos de cada queue para a
 * queue imediatamente superior. Porém, com o passar doa anos, esta tecnica conduziu a
 * longos tempo de espera e foi abandonada. Procure explicar onde estaria o problema e
 * como terá sido corrigido.
 * RESPOSTA: O problema da MLQ é que ao transferir todos os processos de cada fila para
 * a fila imediatamente superior, pode resultar em longos tempos de espera para processos
 * de alta prioridade. Isto acontece porque os processos de baixa prioridade podem estar
 * a monopolizar a CPU, impedindo que os processos de alta prioridade sejam executados.
 * Para corrigir este problema e manter o uso de MLQ, uma abordagem adotada foi introduzir
 * um conceito aging, que permite que os processos de alta prioridade sejam promovidos 
 * periodicamente para filas de prioridade mais alta, mesmo que ainda estejam em execução.
 * Desta forma, têm a probabilidade de serem atendidos mais frequentemente e não precisam
 * de esperar até que todos sejam transferidos. Desta forma, processo de alta prioridade
 * conseguem receber uma maior fatiua de tempo de CPU, mesmo quando existem processos de
 * baixa prioridade em execução. 
 * 
 * 2. Arquiteturas modernas suportam páginas de memória virtual que podem ser configuradas
 * para terem tamanhos grandes.
 * a) Descreva as vantagens e desvantagens destas huge pages face ás paginas classicas.
 * RESPOSTA: Eficiência da memória - huge pages tem um tamanho maior em comparação com as
 * classicas, o que reduz a sobrecarga do SO ao mapear grandes regioes de memória, resultando
 * numa melhor utilização da memória física disponível. Redução de Fragmentação - menos entradas
 * na tabela de páginas são necessárias para mapear uma determinada quantidade de memória;
 * Melhor desempenho - acesso mais rápido em certos casos, especialmente para aplicações que
 * manipulam grandes voluymes de dados, devido ao menor número de traduções de endereço.
 * Desvantagens: Maior consumo inicial de memória - pois uma quantidade maior de memória é
 * reservada para cada página, problemático em sistemas com restrições de memória; Maior
 * tempo de inicialização - o SO necessita de alocar um número menor de huge pages, mas pode
 * resultar num tempo de inicialização mais longo.
 * 
 * b) Dê um exemplo de uma estrutura de dados apropriada para ser alocada numa região de 
 * memória baseada neste tipo de páginas. Justifique.
 * RESPOSTA: Um exemplo é uma matriz multidimensional de grande porte. Por exemplo, em aplicações
 * cientificas que lidam com simulações computacionalmente intensivas, é comum usar matrizes
 * para armazenar e processar grandes conjuntos de dados. Alocar uma matriz pode trazer benefícios
 * como melhor desempenho de acesso e redução da fragmentação. Matrizes multidimensionais requerem
 * uma quantidade significativa de memória contígua para armazenar os dados. Ao alocá-las em
 * huge pages, é possível reduzir o número de páginas necessárias para mapear a matriz.
 * 
 * c) Se fosse mpear em memória variados tipos de ficheiros, diga se deveria considerar o 
 * uso de huge pages. Justifique.
 * RESPOSTA: No caso de mapear em memória vários tipos de arquivos, a consideração do uso de
 * huge pages depende de fatores como tamanho dos arquivos e o padrão de acesso. Se forem grandes
 * e frequentemente acessados de forma sequencial ou aleatória, o uso de huge pages pode trazer
 * benefícios. Isto ocorre porque as huge pages podem reduzir a sobrecarga do sistema ao mapear grandes
 * regiões de memória, melhorando o desempenho de acesso aos dados. No entanto, se os ficheiros
 * forem pequenos e/ou tiverem um padrão de acesso irregular, o uso de huge pages pode não trazer
 * vantagens significativas e até mesmo levar a um maior consumo de memória devido ao tamanho
 * inicial maior das páginas.
 * 
*/

/**
 * 3. Assuma a existẽncia de um programa agrep, que recebe dois argumentos, uma palavra e um nome
 * de ficheiro, e produz no stdout todas as linhas do ficheiro contendo a palavra, cada uma escrita
 * atomicamente num único write; que cada linha não excede os 100 bytes; a existência de uma função
 * readline, com o mesmo propósito de read. Escreva um programa com dois argumentos, uma palavra e 
 * um nome de ficheiro de saída, que recebe pelo stdin uma lista de nomes de ficheiros, um por linha.
 * Pretende-se que este programa, fazendo uso do agrep, concatene no ficheiro de saída todas as linhas
 * dos ficheiros listados que contém a palavra (sem preocupação da ordem das linhas). Explore a
 * concorrência, mas limite a 10 o número máximo de processos a correr agrep em cada momento, e dê
 * a cada execução de agrep um minuto para terminar, forçando a terminação se necessário.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE_LENGTH 100
#define MAX_PROCESSES 10

void agrep(const char *word, const char *filename, const char *output_file) {
    char command[256];
    snprintf(command, sizeof(command), "agrep '%s' '%s' >> '%s'", word, filename, output_file);
    system(command);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s word output_file\n", argv[0]);
        return 1;
    }

    const char *word = argv[1];
    const char *output_file = argv[2];

    FILE *input_file = stdin;

    char line[MAX_LINE_LENGTH];
    int processes_count = 0;
    int total_processes = 0;

    while (fgets(line, sizeof(line), input_file) != NULL) {
        line[strcspn(line, "\n")] = '\0'; // Remove o caractere de nova linha

        pid_t pid = fork();

        if (pid < 0) {
            perror("Error forking process");
            return 1;
        } else if (pid == 0) {
            agrep(word, line, output_file);
            exit(0);
        } else {
            processes_count++;
            total_processes++;

            if (processes_count >= MAX_PROCESSES) {
                int status;
                pid_t terminated_pid = wait(&status);
                processes_count--;

                if (terminated_pid < 0) {
                    perror("Error waiting for process");
                    return 1;
                }

                if (WIFSIGNALED(status)) {
                    printf("Process %d terminated abnormally\n", terminated_pid);
                }
            }
        }
    }

    // Espera pelos processos restantes
    while (processes_count > 0) {
        int status;
        pid_t terminated_pid = wait(&status);
        processes_count--;

        if (terminated_pid < 0) {
            perror("Error waiting for process");
            return 1;
        }

        if (WIFSIGNALED(status)) {
            printf("Process %d terminated abnormally\n", terminated_pid);
        }
    }

    printf("Total processes executed: %d\n", total_processes);

    return 0;
}

/**
 * 4. COnsidere um programa que lê URLs do seu stdin(um por linha), descarrega a página Web
 * correspondente e imprime os URLs nela contidos, cada um escrito atomicamente num único write.
 * Para obter todos os URLs sem repetições, contidos nas páginas dadas por uma lista de URLs,
 * usa-se na shell em conjunto com o programa uniq da seguinte forma:
 * $ fetchurls < seedURLs.txt | uniq
 * Escreva um programa com a mesma funcionalidade e fazendo uso do programs fetchurls e uniq,
 * mas que permita ter 8 processos a descarregar páginas concorrentemente. Assuma que cada URL
 * não execede os 100 bytes e a existẽncia de uma função readline, com o mesmo protótipo de read.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_URL_LENGTH 100

void fetchurls(const char *url) {
    char command[256];
    snprintf(command, sizeof(command), "fetchurls '%s' | uniq", url);
    system(command);
}

int main() {
    char url[MAX_URL_LENGTH];
    int processes_count = 0;
    int total_processes = 0;

    while (readline(url, sizeof(url)) != NULL) {
        url[strcspn(url, "\n")] = '\0'; // Remove o caractere de nova linha

        pid_t pid = fork();

        if (pid < 0) {
            perror("Error forking process");
            return 1;
        } else if (pid == 0) {
            fetchurls(url);
            exit(0);
        } else {
            processes_count++;
            total_processes++;

            if (processes_count >= 8) {
                int status;
                pid_t terminated_pid = wait(&status);
                processes_count--;

                if (terminated_pid < 0) {
                    perror("Error waiting for process");
                    return 1;
                }

                if (WIFSIGNALED(status)) {
                    printf("Process %d terminated abnormally\n", terminated_pid);
                }
            }
        }
    }

    // Espera pelos processos restantes
    while (processes_count > 0) {
        int status;
        pid_t terminated_pid = wait(&status);
        processes_count--;

        if (terminated_pid < 0) {
            perror("Error waiting for process");
            return 1;
        }

        if (WIFSIGNALED(status)) {
            printf("Process %d terminated abnormally\n", terminated_pid);
        }
    }

    printf("Total processes executed: %d\n", total_processes);

    return 0;
}


