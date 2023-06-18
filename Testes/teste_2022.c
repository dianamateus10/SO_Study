/**
 * 1. Suponha que tem 6 tarefas independentes entre si num computador
 * dedicado à execução deste tipo de tarefas. Sabe-se que cada uma das
 * tarefas envolve cálculos demorados e que os resultados são escritos
 * só ao fim, em ficheiros, para serem analisados manualmente. Que
 * algoritmo de escalonamento de CPU seria apropriado a esta situação.
 * Justifique.
 * RESPOSTA: Para a situação descrita, é recomendado o uso de um algoritmo de
 * escalonamento baseado em threads. Um exemplo é o round-robin, no qual
 * cada tarefa recebe uma fatia de tempo para executar e, em seguida, a
 * CPU é passada para a próxima tarefa. Este algoritmo é especialmente
 * adequado quando as tarefas envolvem cálculos demorados, pois permite
 * que cada tarefa avance a sua execução num intervalo de tempo equitativo,
 * evitando a inanição e promovendo a utilização justa de CPU entre tarefas.
 * Ao utilizar threads para cada tarefa, é possível executar múltiplas
 * tarefas simultaneamente no mesmo processador, aproveitando o paralelismo
 * disponível, economizando recursos, com menor sobrecarga de trocas de contexto
 * e partilha eficiente de memória.
 * 
 * 2. Arquiteturas modernas suportam huge pages, páginas com tamanhos muito
 * maiores (2MB) do que o clássico.
 * 
 * a) Descreva que vantagens páginas deste tamanho podem oferecer. Dê um
 * exemplo de uma estrutura de dados apropriada para ser alocada numa região
 * de memória baseada neste tipo de páginas muito grandes. Justifique.
 * RESPOSTA: Páginas de tamanho grande oferecem redução da fragmentação - a
 * quantidade de páginas necessárias para alocar uma determinada quantidade de
 * memória é reduzida, diminuindo a fragmentação interna e externa da memória,
 * havendo menos espaços vazios entre as páginas -, melhor desempenho - em certas
 * situações, a transferência de dados entre a memória principal e a cache é
 * otimizada, pois mais dados podem ser obtidos de uma vez - e menor sobrecarga
 * de gestão da memória - há menos entradas na tabela de páginas e menos sobrecarga
 * na gestão da memória visto que reduzem a necessidade de armazenamento e procura
 * dessas entradas, resultando num uso mais eficiente da memória. Um exemplo de
 * uma estrutura de dados apropriada em regiões de memória baseadas em páginas
 * muito grandes é uma árvore B+, uma estrutura de dados que fornece uma procura
 * eficiente e suporta operações como inserção e exclusão de dados de forma escalavel.
 * 
 * b) Na sua opinião, estas huge pages deveriam ou não ser usadas para mapear em 
 * memória ficheiros de log, escritos em modo append e lidos sequencialmente à medida
 * que o ficheiro cresce? Note que um log poderá crescer lentamente caso a 
 * ocorrência de eventos a registar seja pouco frequente. Justifique.
 * RESPOSTA: Não é a abordagem mais adequada. O uso de huge pages para mapear
 * arquivos de log pode levar à fragmentação externa da memória, visto que
 * as huge pages são grandes blocos de memória contíguos, e se o ficheiro log crescer
 * lentamente e em pequenas quantidades, alocar uma huge page para cada utilização pode
 * resultar em desperdício de memória. Se o ficheiro log crescer lentamente, alocar
 * grandes páginas leva a um uso ineficiente da memória, pois a maior parte da 
 * página será ocupada por dados não utilizados. A abordagem convencional, que usa
 * páginas de tamanho padrão oferece mais flexibilidade, permite alocar apenas o
 * espaço necessário otimizando o uso da memória e evitando a fragmentação.
 * Os benefícios das huge pages são mais evidentes quando aplicadas a cargas de trabalho
 * intensivas da memória, que requerem acesso aleatório a grandes conjuntos de dados.
*/

/**
 * 3. Assuma que numa polataforma de suporte as conversas instantâneas entre utilizadores,
 * existe um servidor que guarda as mensagens dos clientes em diferentes ficheiros de texto.
 * O nome do ficheiro coincide com o nome do autor da mensagem, sendo as respetivas
 * mensagens acrescentadas a esse ficheiro, uma mensagem por linha.
 * 
 * a) Escreva uma função int mensagens(char* palavra, char* ficheiro) a ser utilizada no
 * servidor, que devolve um inteiro com o número de mensagens em que uma determinada palavra
 * ocorre num determinado ficheiro. Use os comandos grep e wc, tendo em conta que, por
 * exemplo, o comando grep palavra ficheiro | wc -l invocado na linha de comandos, escreveria
 * no STDOUT o número de linhas em que palavra ocorre no ficheiro.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int mensagens(char* palavra, char* ficheiro) 
{
    int fd[2];
    pipe(fd);

    pid_t pid = fork();
    if (pid == 0) {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        execlp("grep", "grep", palavra, ficheiro, NULL);
        _exit(1);
    }
    else if (pid == -1)
    {
        perror("fork");
        exit(1);
    }
    else
    {
        close(fd[1]);

        int wc_fd[2];
        pipe(wc_fd);

        pid_t wc_pid = fork();
        if(wc_pid == -1)
        {
            perror("fork");
            exit(1);
        }
        else if (wc_pid == 0)
        {
            close(wc_fd[0]);
            dup2(wc_fd[0], STDIN_FILENO);
            close(fd[0]);
            dup2(wc_fd[1], STDOUT_FILENO);
            close(wc_fd[1]);

            execlp("wc", "wc", "-l", NULL);
            _exit(1);
        }
        else
        {
            close(fd[0]);
            close(wc_fd[1]);
            
            int grep_status, wc_status;
            waitpid(pid, &grep_status, 0);
            waitpid(wc_pid, &wc_status, 0);

            if(WIFEXITED(grep_status) && WIFEXITED(wc_status))
            {
                char buffer[10]; 
                int count = 0;

                while(read(wc_fd[0], buffer, sizeof(buffer))>0)
                {
                    count += atoi(buffer);
                }
                close(wc_fd[0]);
                return count;
            }
            else
            {
                perror("grep or wc");
                exit(1);
            }
        }
    }
    
}

/**
 * b) Escreva a função int autores_que_usaram(char *palavra, int n, char* autores[]),
 * que devolve, para um conjunto de autores, passado um array de n elementos, quantos
 * utilizaram uma determinada palavra nas suas mensagens. A função deve desencadear
 * uma procura concorrente, recorrendo à função desenvolvida na pergunta anterior 
 * para acesso a ficheriros (não podendo usar chamadas ao sistema relativas a ficheiros).
*/

int autores_que_usaram(char* palavra, int n, char* autores[])
{
    int ocorrencias[n];
    pid_t pid[n];

    for(int i = 0; i < n; i++)
    {
        pid[i] = fork();

        if(pid == -1)
        {
            perror("fork");
            exit(1);
        }
        else if(pid[i] == 0)
        {
            int count = mensagens(palavra, autores[i]);
            _exit(count);
        }
    }
    int total_ocorrencias = 0;
    for(int i = 0; i < n; i++)
    {
        int status;
        waitpid(pid[i], &status, 0);

        if(WIFEXITED(status))
        {
            ocorrencias[i] = WEXITSTATUS(status);
            total_ocorrencias += ocorrencias[i];
        }
    }

    for(int i = 0; i < n; i++)
    {
        printf("O autor %s usou a palavra %s %d vezes\n", autores[i], palavra, ocorrencias[i]);
    }

    return total_ocorrencias;
}

int main(int argc, char *argv[])
{
    char palavra[100];

    printf("Insira a palavra a procurar: ");
    scanf("%s", palavra);

    if(strcmp("exe1", argv[1]) == 0)
    {
        char ficheiro[100];

        printf("Insira o nome do ficheiro: ");
        scanf("%s", ficheiro);

        int ocorrencias = mensagens(palavra, ficheiro);
        printf("Número de mensagens: %d\n", ocorrencias);
    }
    else{
        int n;
        printf("Insira o número de autores: ");
        scanf("%d", &n);

        char* autores[n];
        for(int i=0; i<n; i++)
        {
            autores[i] = malloc(100*sizeof(char));
            printf("Insira o nome do autor %d: ", i+1);
            scanf("%s", autores[i]);
        }
        
        int total_ocorrencias = autores_que_usaram(palavra, n, autores);
        printf("Número total de mensagens: %d\n", total_ocorrencias);

        for(int i=0; i<n; i++)
        {
            free(autores[i]);
        }
    }

    return 0;
}