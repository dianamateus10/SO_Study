/**
 *1. Explique porque o algoritmo de escalonamento RR é bastante popular,
 em comparação com outros que podem parecer melhores. Descreva a melhora
 possível sobre o algoritmo básico, adequada a cenátios interativos. 
 RESPOSTA: O RR é bastante popular porque garante a justiça - garante que
 todos os processos recebem uma fatia igual de tempo de CPU, cada processo
 é colocado numa fila circular e é executado por um intervalo de tempo pré-
 determinado - quantum - garantindo que nenhum processo monipoliza o CPU por
 longos períodos -, resposta rápida - para processos interativos -,
 implementação simples atraves de uma fila circular e uma lógica de troca de
 contexto simples. Apesar das vantagens tem uma limitação conhecida como efeito
 de convoy. Ocorre quando um processo de longa duração ocupa a CPU por um
 longo tempo, deixando outros processos à espera na fila, resultando numa baixa
 utilização do CPU e atrasos para processos que exigem uma resposta rápida.
 Uma melhoria possivel para cenários interativos seria a implementação de
 prioridades dinâmicas. Nesta abordagem cada processo tem uma prioridade inicial.
 À medida que o processo é executado, a sua prioridade diminui gradualmente. 
 Isto significa que processos com muito tempo de CPU terão prioridades mais
 baixas em comparação com processos recém-chegados. Desta forma, processos
 interativos tem maior probabilidade de serem atendidos logo, garantindo uma
 melhor capacidade de resposta como um todo.

 2. Durante as aulas foram analisadas situações em que foi possível executar,
 sem degradação visível, conjuntos de programas que pediam centenas de vezes mais
 memória que a RAM existente. Houve também casos em que a quantidade de memória
 pedida atrasou muito na execução de um ou mais programas. Dê exemplos de cada
 uma destas situações opostas/explique porque acontecem.
 RESPOSTA: A primeira situação ocorre quando os programas usam técnicas de gestão
 de memória virtual, como paginação. Neste cenário, os programas solicitam uma
 grande quantidade de memória virtual, mas apenas uma parte dessa memória é
 efetivamente alocada na RAM. Permite que o SO mapeie e gerencie eficientemente os
 endereços virtuais solicitados. A RAM armazena as páginas de memória mais 
 frequentemente acedidas, enquanto que o restante é mantido em dispositivos de
 armazenamentro secundário, como o disco. Á medida que os programas acedem a 
 diferentes páginas de memória, o SO  faz a troca dessas páginas entre RAM e disco.
 O desempenho pode ser afetado se houver muitas destas trocas, mas se o conjunto de
 programas não estiver a fazer um intenso acesso à memória, a degradação visível
 é mínima. Na segunda situação, ocorre quando o SO precisa de realizar várias trocas
 entre RAM e o disco para libertar espaço e alocar as páginas de memória necessárias.
 Estas operações requerem recursos e tempo, resultando em atrasos. O atraso é maior
 quando os programas tem acessos intensos à memória e precisam constantemente de 
 páginas do disco.
*/

/**
 * 3. Suponha que um professor pretende enviar por mail a cada aluno a respetiva
 * classificação. O referido professor já tem um ficheiro de texto devido formatado
 * com <aluno><nota>, 10 caracteres por linha, e quer evitar agora o trabalho de enviar
 * manualmente centenas de emails. 
 * $ mail -s Sistemas_Operativos a00000@alunos.uminho.pt
 * a00000 18
 * <ctrl-D>
 * Escreva um programa em C que leia linhas do ficheiro(passado como argumento) com
 * os pares <aluno><nota> e envie o respetivo email executando o comando mail. 
 * Espera-se uma solução concorrente mas limitando a concorrência a não mais de 10
 * processos em simultâneos de mail.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

#define MAX 10

void send_email(char *aluno, char *nota)
{
    char command[120];
    snprintf(command, sizeof(command), "mail -s Sistemas_Operativos %s@alunos.uminho.pt", aluno);

    int mail_pipe[2];
    if(pipe(mail_pipe) == -1)
    {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if(pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else if(pid==0)
    {
        close(mail_pipe[1]);
        dup2(mail_pipe[0], STDIN_FILENO);
        close(mail_pipe[0]);
        execlp("mail", "mail", "-s", "Sistemas_Operativos", aluno, NULL);
        perror("execlp");
        exit(1);
    }
    else
    {
        close(mail_pipe[0]);
        write(mail_pipe[1], nota, strlen(nota));
        close(mail_pipe[1]);
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status))
        {
            printf("O aluno %s foi notificado com sucesso\n", aluno);
        }
        else
        {
            printf("O aluno %s não foi notificado\n", aluno);
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        exit(1);
    }

    int fd = open(argv[1], O_RDONLY);
    if(fd == -1)
    {
        perror("open");
        exit(1);
    }

    char line[MAX];
    int mail_count = 0;
    ssize_t bytesRead; 
    while((bytesRead = read(fd, line, MAX)) > 0)
    {
        char aluno[7], nota[4];
        sscanf(line, "%s %s", aluno, nota);

        if(mail_count >= MAX)
        {
            int status;
            wait(&status);
            mail_count--;
        }

        pid_t pid = fork();
        if(pid == -1)
        {
            perror("fork");
            exit(1);
        }
        else if(pid == 0)
        {
            send_email(aluno, nota);
            exit(0);
        }
        else
        {
            mail_count++;
        }
    }
    
    close(fd);
    for(int i = 0; i < mail_count; i++)
    {
        int status;
        wait(&status);
    }
    return 0;
}

/**
 * 4. Assuma a existência de um programa patgrep que, recebendo um padrão
 * como argumento, escreve no stdout um X por cada bloco de 128 bytes que
 * contenha o padrão. Escreva um programa counter que escreva no stdout
 * quantos blocos de 128 bytes do seu stdin contém o pdrão passado por
 * argumento. Faça uma procura concorrente, através de 8 instãncias do
 * programa patgrep.
*/

#define MAX 8

int main(int argc, char * argv[])
{
    char* padrao = argv[1];
    int counter = 0;

    for(int i = 0; i < MAX; i++)
    {
        pid_t pid = fork();
        if(pid == -1)
        {
            perror("fork");
            exit(1);
        }
        else if(pid == 0)
        {
            dup2(STDOUT_FILENO, STDERR_FILENO);
            execlp("patgrep", "patgrep", padrao, NULL);
            perror("execlp");
            exit(1);
        }
    }

    for(int i = 0; i < MAX; i++)
    {
        int status;
        wait(&status);
        if(WIFEXITED(status))
        {
            if(WEXITSTATUS(status) == 0)
                counter++;
        }
    }

    printf("O padrão %s aparece %d vezes\n", padrao, counter);

    return 0;
}