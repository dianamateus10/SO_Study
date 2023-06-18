/**1. Mesmo com um número de processadores limitado, um sistema operativo
deve ser capaz de fornecer aos seus utilizadores a ilusão de suportar a
execução simultânea de vários processos.

a) Descreva sucintamente o mecanismo de comutação de processos e a sua
contribuição para a percepção de execução simultânea referida acima.
Justifique a sua resposta.
RESPOSTA: O mecanismo de comutação de processos é uma técnica utilizada
pelos sistemas operativos para gerir a execução de vários processos concorrentes
num único processador. Permite que múltiplos processos sejam executados
aparentemente ao mesmo tempo, proporcionando uma perceção de execução simultânea.
Na comutação de processos, o SO aloca fatias de tempo para cada processo, no qual
o processo utiliza o CPU para realizar as suas tarefas. Após acabar a sua
fatia de tempo, o SO interrompe a execução desse processo e passa o controlo
para outro. Esta troca rápida entre processos cria a ilusão de que múltiplos
processos estão a ser executados simultaneamente, mesmo que o processador esteja,
na verdade, a executar apenas um processo de cada vez. A comutação acontece
a uma velocidade tão alta que os intervalos de tempo são imperceptíveis para o
user, criando a sensação de que vários processos estão a ser executados simultaneamente.

b) Com estratégias de escalonamento em fatias de tempo, o intervalo de tempo
escolhido até forçar a comutação de processos tem impacto no desempenho de
aplicações. Discuta a importância de escolher intervalos de tempo adequados 
à carga do sistema. Justifique a sua resposta.
RESPOSTA: O intervalo de tempo escolhido, ou quantum de tempo determina por
quanto tempo cada processo poderá executar antes de ser interrompido e dar 
lugar a outro processo. Existem duas situações que ilustram a importância de
escolher intervalos de tempo adequados: intervalos muito curtos - a comutação
ocorrerá com muita frequência, o que pode resultar num alto custo de comutação
de contexto (tempo gasto pelo SO para guardar o estado de um processo e restaurar
o estado de outro) que poderá prejudicar o desempenho geral do sistema, consumindo
recursos e poderá causar efeitos negativos na eficiência da cache do processador, 
resultando em mais falhas e menor aproveitamento de hardware - e ainda, intervalos
muito longos - um único processo pode monopolizar o processador por um longo período
de tempo, causando atrasos perceptíveis em outros processos que aguardam a sua
vez. Assim, é importante escolher intervalos de tempo adequados para que o sistema
seja eficiente e não prejudique o desempenho das aplicações.
*/

/** 2. Imagine que está a implementar uma aplicação que precisa de armazenar e
ler informação de um conjunto de ficheiros de forma eficiente. Na escrita do código
da aplicação deparou-se com diferentes abordagens. Com base no seu conhecimento
sobre o funcionamento dos sistemas operativos, sistemas de ficheiros e dispositivos de
armazenamento, indique quais as que acha mais apropriadas.

a) Para cada ficheiro pode efetuar várias escritas (através da system call write) com
um tamanho reduzido (10bytes) ou realizar escritas com um tamanho maior (4096 bytes).
Qual das opções acha melhor para otimizar o desempenho da aplicação. Justifique a resposta.
RESPOSTA: Para determinar qual a melhor opção, é necessário considerar vários fatores
como o sistema de ficheiros utilizado, a latência de acesso ao disco, o tamanho da cache,
entre outros. No entanto, consideranodo o cenário típico: escritas de tamanho reduzido
pode resultar num menor tempo de transferência de dados para o disco, com um menor impacto
de ocupação da cache do sistema. No entanto, o custo de comutação de contexto e as operações
de I/O podem ter um peso maior, pois há uma sobrecarga associada à comunicação entre a 
aplicação e o SO para cada escrita. Por outro lado, escritas de tamanho maior podem resultar
num tempo de transferência de dados relativamente maior para o disco. NO entanto, como as
operações de I/O geralmente ocorrem em blocos de tamanho fixo, o custo de comutação de
contexto e a sobrecarga associada são amortizados ao lifar com blocos maiores, e por isso,
operações menos frequentes. Portanto, em termos de otimização de desempenho, a opção de
escritas com tamanho grande tende a ser mais eficiente na maioria dos casos. Isto ocorre
porque a redução no custo de comutação de contexto e as operações I/O amortizadas em blocos
maiores superam o tempo de transferência de dados maiores para o disco.

b) Ao ler cada ficheiro leituras de forma sequencial ou aleatória sobre os blocos de dados 
contidos no mesmo. Qual o padrãoo de acesso, sequencial ou aleatório, mais eficiente em termos
de desempenho. Justifique a sua resposta.
RESPOSTA: No acesso sequencial, os blocos de dados são lidos ou escritos em ordem contínua, sem
saltos ou retornos. Este padrão de acesso é especialmente eficiente quando se trata de leitura
ou escrita de grandes volumes de dados. Desta forma, o SO pode otimizar o acesso ao disco, lendo
ou escrevendo blocos em sequência, sem a necessidade de movimentos de cabeça de leitura/escrita.
Isto minimiza o tempo de acesso ao disco e latência do disco. Por outro lado, no acesso aleatório,
os blocos de dados são lidos ou escritos em ordem não contínua, com saltos ou retornos. Este padrão
de acesso é útil quando é necessário acessar blocos de dados específicos, em posições diferentes ao
longo do arquivo. No entanto, pode ser menos eficiente em termos de desempenho da 
aplicação, pois pode exigir movimentos frequentes de cabeça de escrita/leitura, aumentando o tempo 
de busca e latência do disco. Além disso, pode levar a um maior número de operações de I/O,
impactando o desempenho geral da aplicação. Em resumo, o acesso sequencial é mais eficiente.
*/

/** 3. Considere um sistema de deteção de anomalias numa linha de produção baseado em algoritmos
 * de IA. As anomalias são detetadas com base em fotografias do produto. Por exemplo, no caso
 * do produto ser um chip, os modelos de IA irão tentar perceber se o chip tem algum defeito
 * visível. Este algoritmos estão implementados no ficheiro executável detectAnom que recebe
 * como argumento o caminho para um ficheiro de imagem e escreve para o stdout uma linha por
 * cada defeito encontrado.
 * 
 * a) Escreva a função void defeitos(char *imagens[n], int n, int max) que, para um conjunto
 * de caminhos para imagens passados num array de n elementos, escreve para o stdout todos os
 * defeitos encontrados. A função deve desencadear uma procura concorrente, recorrendo ao
 * ficheiro executável detectAnom, nunca excedendo um total de max processos a executar
 * simultaneamente.
*/

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

void defeitos(char *imagens[], int n, int max)
{
    int status;
    int nrexec = 0;

    //o código itera sobre o array de imagens e, para cada imagem,
    //verifica se o número de execuções ativas é menor que o máximo
    //permitido. Caso seja, cria um novo processo filho através do fork()
    for(int i = 0; i < n; i++)
    {
        if(nrexec < max)
        {
            if(fork() == 0)
            {
                //no processo filho, a imagem é substituida pelo executavel
                //passado como argumento e encerra o processo filho com exit(0)
                execlp("detectAnom", "detectAnom", imagens[i], NULL);
                exit(0);
            }
            else
            {
                //no processo pai, incrementa o número de processos ativos
                nrexec++;
            }
        }
    }

    //após a criação de todos os processos, o processo pai aguarda a conclusão
    //de cada processo filho através da função wait(). A cada processo filho
    //a variavel nrexec é decrementada.
    for(int i = 0; i < max; i++)
    {
        wait(&status);
        nrexec--;
    }
}

/**
 * b) Escreva a função void conta(char *imagens[], int n) que para um
 * conjunto de caminhos para imagens passados num array de n elementos,
 * devolve para o stdout o número total de defeitos que foram encontrados.
 * Para resolver este exercício, pode tirar proveito da função defeitos
 * desenvolvida anteriormente e do comando unix wc -l, o qual lê as linhas
 * do stdin e retorna o número total de linhas para o stdout.
*/
void conta(char *imagens[], int n)
{
    int fd[2];
    pipe(fd);

    
    if(fork()==0)
    {
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        close(fd[1]);

        defeitos(imagens, n, 10);
        _exit(0);
    }
    close(fd[1]);

    if(fork() == 0)
    {
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);

        execlp("wc", "wc", "-l", NULL);
        _exit(0);
    }
    close(fd[0]);

    wait(NULL);
    wait(NULL);
}
