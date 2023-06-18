/**
 * 1. Distinga os conceitos de interrupção e desinfetação forçada. 
 * Explique os objetivos de cada um destes mecanismos e dê exemplos que considere
 * adequados.
 * RESPOSTA: Os conceitos de desinfetação forçada e interrupção são mecanismos
 * utilizados em SO para lidar com situações excecionais e garantir a estabilidade
 * e segurança do sistema. A interrupção é um mecanismo pelo qual um dispositivo 
 * externo ou um evento do SO sinaliza a necessidade de atenção imediata por parte
 * do sistema. A interrupção interrompe a execução normal do programa em andamento
 * e transfere o controlo para um tratador de interrupção específico. O objetivo
 * principal é permitir uma resposta rápida e eficiente a eventos ou situações 
 * críticas. Alguns exemplos são: interrupção de hardware - quando um dispositivo
 * de I/O envia um sinal para o SO indicando ação do user; interrupção de tempo -
 * quando um temporizador interno envia um sinal periódico para garantir que tarefas
 * críticas sejam realizadas em intervalos regulares e interrução de execeção - 
 * quando ocorre uma situação excecional, como uma falha de segmentação ou tentativa
 * de acesso a uma área de memória não permitida. A force shutdown é um procedimento
 * no qual o SO encerra abruptamente um processo ou desliga o sistema sem permitir que
 * os processos em execução terminem normalmente. O objetivo é garantir a segurança
 * do sistema e evitar que processos maliciosos ou com falhas causem danos ao sistema.
 * Alguns exemplos são: processo encravado - um processo não responde ou está num loop
 * infinito, ocupando recursos importantes; sistema congelado - quando o SO não responde
 * e não é possível realizar tarefas básicas; falha grave, como erro de hardware ou
 * corrupção de dados e a continuidade do funcionamento normal é impossível.
 * 
 * 2. Descreva em que consiste o fenómeno conhecido por thrashing. Explique como a 
 * interação entre o sistema de memória virtual e um escalonador ingénuo que não têm
 * em conta o uso de memória, mas apenas a utilização do CPU, pode levar ao thrashing.
 * RESPOSTA: O thrashing ocorre em sistemas de memória virtual quando o desempenho do
 * sistema diminui significativamente devido a um alto volume de page faults, ou seja,
 * quando o sistema precisa de procurar constantemente páginas de memória no disco.
 * O thrashing é causado pela interação entre o sistema de memória virtual e um escalonador
 * ingénuo que prioriza apenas a utilização do CPU, sem considerar o uso eficiente da
 * memória. Isto pode levar a: escassez de memória física - o SO aloca mais processos do
 * que a RAM pode acomodar, quando o tamanho combinado dos processos excede a capacidade
 * da memória; falha de páginas na memória física - muitas páginas são movidas para o
 * espaço de armazenamento secundário, como o disco, para libertar espaço na RAM; 
 * interrupções constantes de page faults - o escalonador não leva em consideração o tempo
 * gasto na procura de páginas no disco, alternando a execução entre os processos que estão
 * a enfrentar page faults; ciclo vicioso - o alto volume de page faults resulta num ciclo
 * vicioso em que o sistema gasta a maior parte do tempo à procura de páginas no disco e
 * trocando-as por memoria, em vez de executar as instruções dos processos. Isto leva a 
 * um uso ineficiente do CPU e a um baixo desempenho do sistema em geral.
 * O resultado é uma queda significativa no desempenho dos sistema, com uma alta carga de
 * trocas de páginas e baixa taxa de conclusão de processos. Pode levar a tempos de resposta
 * lentos, atrasos na execução de tarefas e uma utilização ineficiente dos recursos.
 * Para evitar o thrashing é necessário um escalonador que leve em consideração tanto o
 * uso do CPU quando o uso eficiente da memória.
 * */

/**
 * 3. Pretende-se criar um banco de dados de registo e pesquisa relativamente aos cidadaos
 * vacinados contra a COVID-19. Assuma que os clientes enviam para o servidor, através de pipes
 * com nome, linhas de texto contendo o número de cartão de cidadão, idade, código da região.
 * Assuma que existem 9 regiões.
 * 
 * a)Implemente uma primeira versão do programa servidor, que deve guardar a lista de pessoas
 * vacinadas, em diferentes ficheiros, um por cada região, escrita linha a linha, com o mesmo
 * formato enviado pelos clientes. O servidor deve ser um programa sequencial, mas preparado
 * para a possibilidade de vários clientes se ligarem simultaneamente.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>

#define MAX_BUFFER 1024
#define MAX_REG 9

int main()
{
    int pipes[MAX_REG][2];
    for(int i=0; i < MAX_REG; i++)
    {
        if(pipe(pipes[i]) == -1)
        {
            perror("Pipe failed");
            exit(EXIT_FAILURE);
        }
    }

    int fd[MAX_REG];
    for(int i=0; i < MAX_REG; i++)
    {
        char filename[20];
        sprintf(filename, "reg%d.txt", i);
        fd[i] = open(filename, O_CREAT | O_WRONLY | O_APPEND, 0666);
        if(fd[i] == -1)
        {
            perror("Open failed");
            exit(EXIT_FAILURE);
        }
    }

    char buffer[MAX_BUFFER];

    while(1)
    {
        for(int i=0; i < MAX_REG; i++)
        {
            int flags = fcntl(pipes[i][0], F_GETFL);
            fcntl(pipes[i][0], F_SETFL, flags | O_NONBLOCK);

            ssize_t bytesRead = read(pipes[i][0], buffer, MAX_BUFFER);
            if(bytesRead == -1)
            {
                perror("Read failed");
                exit(EXIT_FAILURE);
            }
            if(bytesRead > 0)
            {
                write(fd[i], buffer, bytesRead);
                write(fd[i], "\n", 1);
                printf("Dados recebidos e armazenados na região: %d\n", i+1);
            }
        }
        usleep(1000);
    }
    for(int i = 0; i < MAX_REG; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
        close(fd[i]);
    }
    
    return 0;
}

/**
 * b) Escreva a função int vacinados(char* regiao, int idade), para ser usada
 * do lado do servidor, que devolva o número de pacientes de uma certa idade que
 * foram vacinados numa dada região. Faça uso dos comandos grep e wc.
 * Por exemplo, grep " 60 " ficheiro | wc -l escreve no stdout o número de linhas
 * encontradas no ficheiro correspondentes a pacientes com 60 anos.
*/


int vacinados(char* regiao, int idade)
{
    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if(pid == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if(pid == 0)
    {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        execlp("grep", "grep", idade, "ficheiro", NULL);
        perror("Exec failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        close(fd[1]);
        wait(NULL);

        int count;
        read(fd[0], &count, sizeof(int));
        printf("Número de pacientes com %d anos na região %s: %d\n", idade, regiao, count);
    }

    return 0;
}

/**
 * c) Escreva a função bool vacinado(char* cidadao), para ser usada do lado
 * do servidor, que permita saber se um certo paciente já foi vacinado. Recorra
 * ao comando grep cidadao ficheiro, para pesquisar em cada ficheiro. A função deve
 * explorar possível concorrẽncia e retornar o mais rapidamente possível, parando
 * eventuais pesquisas ainda em cursos para esse cidadao, que ficaram redundantes.
*/

bool vacinado(char* cidadao)
{
    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if(pid == -1)
    {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if(pid == 0)
    {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        execlp("grep", "grep", cidadao, "ficheiro", NULL);
        perror("Exec failed");
        exit(EXIT_FAILURE);
    }
    else
    {
        close(fd[1]);
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);
        if(result == -1)
        {
            perror("Wait failed");
            exit(EXIT_FAILURE);
        }
        else if (result == 0)
        {
            return false;
        }
        else
        {
            if(WIFEXITED(status))
            {
                int exitStatus = WEXITSTATUS(status);
                if(exitStatus == 0)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return false;
}
