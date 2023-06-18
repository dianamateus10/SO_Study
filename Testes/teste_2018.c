/**
 * 1. Admita que um grande conjunto de dados tem de ser processado em cadeia. Demonstre
 * que neste caso a comunicação entre processos por pipes anónimos é muito mais eficiente
 * e conveniente do que a utilização de ficheiros, sendo provável que também contribua
 * para a redução de tempos de resposta.
 * RESPOSTA: Ao processar um grande conjunto de dados em cadeia, a comunicação por pipes
 * anónimos é mais eficiente porque oferecem uma comunicação de baixo overhead entre
 * processos já que não há necessidade de escrever e ler dados em disco. Os dados são
 * transferidos diretamente da saída de um professo para a entrada do próximo processo na
 * cadeia, resulando numa transferência mais rápida e eficiente. Além disso, no que concerne
 * à memória, os dados não precisam de ser armazenados no disco, economizando recursos de
 * memória. Este tipo de pipes fornecem ainda um mecanismo natural de sincronização entre
 * processos, sendo que o produtor só pode escrever quando o consumidor estiver pronto a ler
 * evitando a necessidade de mecanismos complexos de sincronização. É ainda fácil de usar e
 * implementar, sendo criados automaticamente pelo SO quando os processos são criados e podem
 * ser associados por meio de descritores de ficheiros.
 * 
 * 2. Suponha que ao instalar um SO num computador lhe aparece uma "caixa" que pede para
 * escolher o grau de multiprogramação. Por omissão fica a 1 mas pode ajustar o valor entre
 * 1 e 999. Explique os criterios em que basearia a sua escolha e justifique.
 * RESPOSTA: A escolha depende da capacidade de processamento - se o computador possui recursos
 * de processamento mais robustos, como um CPU rápido e múltiplos núcleos, pode-se aumentar o 
 * grau de multiprogramação para permitir a execução simultânea de mais processos. Quanto à
 * demanda de recursos é importante considerar que se os processos exigem uma quantidade significativa
 * de recursos, como CPU, I/O, memória é aconselhavel reduzir o GM para evitar o 
 * congestionamento e garantir um desempenho adequado. A natureza das aplicações também é 
 * importante, se forem interativos e exigirem uma resposta rápida, pode ser preferível ter um 
 * GM menor para garantir um tempo de resposta mais rápido. ALém disso, a tolerância a falhas
 * um maior GM pode aumentar a probabilidade de conflitos e erros devido à concorrência entre
 * os processos.
 * 
 * 3. Se um determinado programa demora a executar 5 minutos quando não há mais nada em execução
 * num computador, qual o tempo médio de execução se forem executados simultanemanente 4 processos
 * idênticos, no caso do escalonamento FIFO ou RR, respetivamente. Escolha uma fatia de tempo adequada
 * ao computador em causa e apresente as contas. QUe conclusões tira?
 * RESPOSTA: Para calcular o tempo médio de execução dos processos, considerando o escalonamento
 * FIFO e RR assumimos que cada processo recebe uma fatia de tempo igual para executar antes de ser
 * interrompido e ceder o CPU ao proóximo processo. Cada fatia de tempo é de 1 minuto. Assim, para
 * o FIFO os processos são executados na ordem que chegaram. Nesse caso o primeiro executará por 5
 * minutos, já que não há outros processos concorrentes. Os demais precisarão de aguardar pela sua
 * vez na fila. Assim, o tempo médio de execução será de 5 minutos para todos os processos.
 * No RR, cada processo recebe uma fatia de tempo para executar, antes de ser interrompido e ceder
 * a CPU para o próximo. Se cada processo recebe 1 min de cada vez, cada um executará por 1 min 
 * antes de ser interrompido. A sequencia será: p1 em 1 min, p2 em 1 min, p3 em 1 min e p4 em 1 min.
 * Depois que o quarto processo terminar a sua primeira fatia, o CPU volta ao primeiro processo
 * novamente, repetindo o ciclo até que todos sejam concluídos. Neste caso, cada processo terá
 * executado por 5 minutos, mas em intervalos de tempo diferentes. Portanto, o tempo médio de
 * execução será de 5 minutos para todos os processos.
*/

/**
 * 4. Escreva um programa, invocado da forma controlador p c, para processamento de linhas de texto
 * produzidas concorrentemente por p processos, instâncias de um programa produtor, por c processos,
 * instancias de um programa consumidor, ambos invocados sem argumentos. Assuma que cada linha produzida
 * começa por um digito d, e faça com que o resto da linha seja processada pelo consumidor d, quando 
 * 0<=d<c, ou descartada caso contrario.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_LINE_LENGTH 100

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s p c\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_producers = atoi(argv[1]);  // Número de processos produtores
    int num_consumers = atoi(argv[2]);  // Número de processos consumidores

    // Array para armazenar os PIDs dos processos produtores
    pid_t producer_pids[num_producers];
    // Array para armazenar os PIDs dos processos consumidores
    pid_t consumer_pids[num_consumers];

    // Criar os processos produtores
    for (int i = 0; i < num_producers; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Erro ao criar processo produtor");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Processo produtor
            // Código do programa produtor

            // Exemplo simples: produzir linhas de texto com um número no início
            for (int j = 0; j < 10; j++) {
                printf("%d: Linha %d do produtor %d\n", i % num_consumers, j, i);
                fflush(stdout);
                sleep(1);
            }

            exit(EXIT_SUCCESS);
        } else {
            producer_pids[i] = pid;
        }
    }

    // Criar os processos consumidores
    for (int i = 0; i < num_consumers; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Erro ao criar processo consumidor");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Processo consumidor
            // Código do programa consumidor

            char line[MAX_LINE_LENGTH];

            while (fgets(line, sizeof(line), stdin) != NULL) {
                int digit = line[0] - '0';

                if (digit >= 0 && digit < num_consumers && digit == i) {
                    // Processar a linha
                    printf("Consumidor %d: %s", i, line);
                }
            }

            exit(EXIT_SUCCESS);
        } else {
            consumer_pids[i] = pid;
        }
    }

    // Aguardar o término de todos os processos filhos produtores
    for (int i = 0; i < num_producers; i++) {
        waitpid(producer_pids[i], NULL, 0);
    }

    // Fechar a entrada padrão dos processos consumidores para que eles terminem
    fclose(stdin);

    // Aguardar o término de todos os processos filhos consumidores
    for (int i = 0; i < num_consumers; i++) {
        waitpid(consumer_pids[i], NULL, 0);
    }

    return 0;
}
