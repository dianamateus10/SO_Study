/**
 * 1. Explique os motivos para o algoritmo de escalonamento de processos
 * RR continuar a ser muito popular, apesar do SRT tipicamente conseguir menores
 * tempos de espera na ready queue.
 * RESPOSTA: Apesar do algoritmo RR não ter a menor espera na fila de processos
 * prontos em comparaçã com o SRT, é popular porque apresenta uma boa justiça e
 * equidade no tratamento dos processos - garante que todos tenham oportunidades
 * iguais de utilizar o CPU, sendo importante em sistemas compartilhados, onde é
 * necessário garantir que nenhum processo monipolize os recursos do CPU -,
 * responde rapidamente - como cada processo recebe um quantum de tempo curto, os
 * processos interativos têm a oportunidade de serem executados com frequência e
 * assim fornecerem respostas rápidas aos utilizadores, sendo importante em SO de
 * desktop e em ambientes interativos -, baixa latência - os processos não precisam
 * de esperar até que todos os processos de menor tempo de execução sejam concluídos
 *  - e simplicidade - é relativamente simples de implementar e entender em compa
 * ração com outros algoritmos mais complexos, requerendo apenas a definição do quantum
 * e um mecanismo para interromper e trocar de processo quando a fatia de tempo expira.
 * 
 * 2. Explique em que consiste o swapping de processos, e explique porque pode ser
 * útil num sistema que tenha memória virtual suportada por paginação.
 * RESPOSTA: O swapping é um mecanismo utilizado em SO com memória virtual suportada
 * por paginação para mover processos inteiros da memória principal (RAM) para um
 * espaço de armazenamento secundário, como o disco, libertando espaço na memória RAM
 * para outros processos. Quando um SO precisa de alocar memória para um novo
 * processo e não há memória livre suficiente, o SO pode mover um processo que não
 * está a ser utilizado para o disco, libertando espaço na memória RAM para o novo
 * processo. Quando o processo que foi movido para o disco é necessário novamente,
 * o SO pode mover o processo de volta para a memória RAM. O swapping é útil porque
 * permite que o SO execute mais processos do que a memória RAM pode suportar, pois
 * permite que o SO utilize o espaço de armazenamento secundário como uma extensão
 * da memória RAM. Desta forma, permite o aumento da capacidade de execução de processos
 * , permitindo que a memória virtual seja maior do que a RAM disponível - o SO pode
 * executar mais processos do que a quantidade de memória física disponível, já que os
 * processos inativos podem ser movidos para o armazenamento secundário, temporariamente.
 * Importa realçar o equilíbrio de carga, sendo que processos inativos ou com baixa
 * prioridade podem ser movidos para libertar espaço para processos mais ativos ou
 * importantes. Assim, utiliza a memória de forma eficiente, alocando memória
 * apenas quando necessário, conforme a demanda dos processos. Oferece também flexibilidade,
 * para lidar com situações em que a demanda varia ao longo do tempo. Á medida que os
 * processos são iniciados ou encerrados, o swapping permite ajustar a utilização da
 * memória de acordo com as necessidades do sistema.
*/

/**
 * 3. Implemente a função comand, int comando(const char * filtro, const char * entrada, 
 * const char * saida), que execute um programa filtro, sem argumentos. Esse comando
 * deverá processar o ficheiro de entrada no seu standard input e escrever no ficheiro
 * de saída o resultado do seu standard output. A função deverá retornar verdadeiro ou
 * falso consoante filtro escreva ou não no standard error. Não utilize ficheiros auxiliares.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_REQUESTS 5
#define MAX_ARG_LEN 20
#define LOG_FILE "comando.log"

typedef struct request
{
    char filtro[MAX_ARG_LEN];
    char entrada[MAX_ARG_LEN];
    char saida[MAX_ARG_LEN];
} Request;

int comando(const char * filtro, const char * entrada, const char * saida) {
    int fd[2];
    pid_t pid;
    int stderr_flag = 0;

    if (pipe(fd) == -1)
    {
        perror("pipe");
        exit(1);
    }

    pid = fork();
    if (pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (pid == 0)
    {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        execlp(filtro, filtro, NULL);
        _exit(1);
    }
    else
    {
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);

        FILE * output_file = open(saida, "w");
        if(output_file == NULL)
        {
            perror("open");
            exit(1);
        }

        char buffer[1024];
        ssize_t bytes_read;
        while (bytes_read = read(STDIN_FILENO, buffer, 1024) > 0)
        {
            if(write(STDOUT_FILENO, buffer, bytes_read) == -1)
            {
                perror("write");
                exit(1);
            }
        }

        close(output_file);

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) 
        {
            if(WIFEXITED(status) != 0)
            stderr_flag = 1;
        }
        else
        {
            stderr_flag = 1;
        }
    }
    
    return !stderr_flag;
}

/**
 * 4. Implemente um servidor acessível via pipe com nome que execute a referida
 * função a pedido dos clientes. Escolha um formato de mensagem que resulte em
 * código simples, tendo em conta que cada argumento cabe num array de 20chars, 
 * incluindo o terminador de string. Este servidor deve ir recebendo pedidos até
 * ter 5 pedidos com os respetivos argumentos - filtro, entrada e saída - e só
 * nessa altura invoca concorrentemente a função comando de cada pedido. De seguida, 
 * acrescenta ao ficheiro comando.log os argumentos das invocações que retornem 
 * verdadeiro (basta uma linha de texto com os argumentos enviados pelo cliente).
 * O servidor só deverá aceitar nova ronda de pedidos quando tiver acabado de
 * processar as 5 respostas anteriores.
*/

int process_request(Request requests[])
{
    FILE * log_file = fopen(LOG_FILE, "a");
    if(log_file == NULL)
    {
        perror("fopen");
        exit(1);
    }

    for(int i = 0; i < MAX_REQUESTS; i++)
    {
        Request request = requests[i];
        int stderr_flag = comando(request.filtro, request.entrada, request.saida);
        if(stderr_flag)
        {
            fprintf(log_file, "%s %s %s\n", request.filtro, request.entrada, request.saida);
        }
    }

    fclose(log_file);
    return 1;
}

int main()
{
    const char * pipe_name = "pipe";
    mkfifo(pipe_name, 0666);

    Request requests[MAX_REQUESTS];
    int request_count = 0;

    while(1)
    {
        int pipe_fd = open(pipe_name, O_RDONLY);
        if(pipe_fd == -1)
        {
            perror("open");
            exit(1);
        }

        while(request_count < MAX_REQUESTS)
        {
            Request request;
            if(read(pipe_fd, &request, sizeof(request)) == -1)
            {
                perror("read");
                exit(1);
            }

            if(read(pipe_fd, &request, sizeof(request)) == 0)
            {
                break;
            }

            requests[request_count++] = request;
        }
        close(pipe_fd); 

        if(request_count == MAX_REQUESTS)
        {
            pid_t pid = fork();
            if(pid == -1)
            {
                perror("fork");
                exit(1);
            }
            else if(pid == 0)
            {
                process_request(requests);
                _exit(0);
            }
            else
            {
                int status;
                waitpid(pid, &status, 0);
                request_count = 0;
            }
        }
    }

    return 0;
}