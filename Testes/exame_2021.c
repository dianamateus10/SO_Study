/**
 * 1. Relativamente aos estados de um processo, explique o que é um processo
 * bloqueado e a importância da existência do estado. Dê exemplos.
 * RESPOSTA: Um processo bloqueado refere-se a um processo que não pode prosseguir
 * a sua execução normal devido à espera por algum evento externo ou interno. Nesse
 * estado, o processo está temporariamente suspenso até que a condição que o 
 * bloqueou seja resolvida. O estado bloqueado é importante para permitir que o SO
 * gerencie de forma eficiente os recursos e evite que os processos desperdicem
 * recursos ao ficarem ociosos enquanto aguardam. A importância deste estado reside
 * na eficiência do sistema - através da remoção desse processo da lista de processos
 * prontos para execução, para que o CPU seja alocado a outros; sincronização e 
 * comunicação entre processos e prevenção de espera ativa - um processo não pode
 * avançar devido à falta de um recurso ou evente, mantendo um loop ativo, verificando
 * repetidamente a disponibilidade do recurso é ineficiente e desperdiça recursos. 
 * Exemplos: espera por operação I/O, sincronização entre processos.
 * 
 * 2. Justifique porque, no que diz respeito a algoritmos de substituição de páginas,
 * não é usado normalmente o LRU, apesar de este se aproximar razoavelmente do otimo.
 * Descreva um dos algoritmos usados na prática.
 * RESPOSTA: Embora o LRU se aproxime razoavelmente do ótimo, não é amplamente utilizado
 * na prática devido ao seu custo computacional elevado - exigindo um acompanhamento
 * detalhado do histórico de acesso às páginas, cada vez que uma é acessada, precisa de
 * ser atualizada na estrutura de dados requerendo operações de inserção e  atualização
 * frequentes - e ainda o facto de não ser fácil de implementar - exige uma estrutura de
 * dados adicional, como uma lista encadeada ou matriz, para rastrear a ordem de acesso
 * às páginas. Manter essa estrutura atualizada e realizar operações necessárias para
 * manter a ordem correta das páginas requer uma implementação mais complexa e propensa 
 * a erros. Um dos algoritmos usados na prática é o SC (Second Chance). Baseia-se no
 * conceito de bit de referência, em que cada página recebe um bit definido quando a 
 * página é acedida. O algoritmo percorre as páginas em ordem circular e verifica o bit
 * de referência de cada página. Se estiver definido, a página recebe uma SC e o bit de
 * referência é desativado. Caso contrário, a página é selecionada para substituição.
*/

/**
 * 3. Implemente um programa memoria que executa 10 vezes, sequencialmente, um programa
 * (com eventuais argumentos) apresentando no final a memória virtual usada (valor mínimo
 * médio e máximo das 10 execuções).
 * $ ./memoria programa arg1 --- argN
 * memória: 257891 276878 294632
 * Considere que a memória virtual (KB) usada por um processo pode ser obtida através do
 * standard output resultando da execução de (com [pid] substituído pelo pid do processo
 * apropriado): $ grep vmPeak /proc/[pid]/nenstats | cut -d" " -f4
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_EXECUTIONS 10

void executeProgram(char *program, char **arguments)
{
    pid_t pid = fork();

    if (pid == 0) {
        execvp(program, arguments);
        exit(0);
    } else {
        wait(NULL);
    }
}

void getMemoryUsage(pid_t pid, int *min, int *avg, int *max)
{
    char command[100];
    sprintf(command, "grep vmPeak /proc/%d/status | cut -d' ' -f4", pid);

    FILE *fp = popen(command, "r");
    int memoryUsage;
    int totalMemory = 0;
    int count = 0;

    while (fscanf(fp, "%d", &memoryUsage) == 1) {
        if (count == 0 || memoryUsage < *min) {
            *min = memoryUsage;
        }

        totalMemory += memoryUsage;
        count++;
    }

    pclose(fp);

    *avg = count > 0 ? totalMemory / count : 0;
    *max = count > 0 ? memoryUsage : 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Uso: %s programa [argumentos...]\n", argv[0]);
        return 1;
    }

    char *program = argv[1];
    char **arguments = argv + 1;

    int minMemory = 0;
    int avgMemory = 0;
    int maxMemory = 0;

    for (int i = 0; i < MAX_EXECUTIONS; i++) {
        executeProgram(program, arguments);

        pid_t pid = getpid();
        int min, avg, max;
        getMemoryUsage(pid, &min, &avg, &max);

        if (i == 0 || min < minMemory) {
            minMemory = min;
        }

        avgMemory += avg;

        if (max > maxMemory) {
            maxMemory = max;
        }
    }

    avgMemory /= MAX_EXECUTIONS;

    printf("memória: %d %d %d\n", minMemory, avgMemory, maxMemory);

    return 0;
}

/**
 * 4. Implemente um programa que corre em rondas sequenciais. Em cada ronda
 * deverão ser executadas 100 instâncias concorrentes do comando cmd (sem argumentos).
 * Cada ronda deve terminar no máximo após 20 segundos, devendo eventuais instâncias
 * do comando ainda a executar serem forçadas a terminar. Ao fim de cada ronda,
 * o programa deverá escrever o número de vezes que o comando executou completamente
 * nessa ronda (sem ser interrompido). Ao ser interrompido com SIGINT, o programa
 * deverá escrever o número de rondas finalizadas, abortar a ronda em curso e terminar.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define NUM_INSTANCES 100
#define MAX_ROUND_TIME 20

int numExecutions = 0;
int numRounds = 0;

void sigintHandler(int signum)
{
    printf("\nNúmero de rondas finalizadas: %d\n", numRounds);
    exit(0);
}

void sigalrmHandler(int signum)
{
    printf("Número de execuções completas na ronda %d: %d\n", numRounds, numExecutions);
    exit(0);
}

void executeCommand()
{
    execlp("cmd", "cmd", NULL);
    exit(0);
}

int main()
{
    signal(SIGINT, sigintHandler);
    signal(SIGALRM, sigalrmHandler);

    while (1)
    {
        numRounds++;
        numExecutions = 0;

        alarm(MAX_ROUND_TIME);

        for (int i = 0; i < NUM_INSTANCES; i++)
        {
            pid_t pid = fork();

            if (pid == 0)
            {
                executeCommand();
            }
            else if (pid > 0)
            {
                wait(NULL);
                numExecutions++;
            }
        }
    }

    return 0;
}
