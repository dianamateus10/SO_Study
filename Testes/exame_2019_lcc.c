/**
 * 1. Na estratégia de escalonameto SRT, um processo poderá ser desafetado
 * se chegar outro com duração esperada menor. Explique como podem ser estimadas
 * tais durações e, supondo um núnmero fixo de processos em execução, descreva
 * poderá chegar a tal processo.
 * RESPOSTA: No SRT, as durações esperados dos processos podem ser estimadas
 * através do tempo restante - cada processo possui um campo que indica o tempo
 * restante necessário para a sua conclusão, sendo que com base nesse valor, o 
 * escalonador pode fazer uma estimativa da duração esperada; atrabés do tempo de
 * execução anterior; através de informações do processo - alguns SO permitem que
 * os processos forneçam estimativas da duração esperada. Quanto à chegada de um
 * processo, isso ocorre quando um novo processo é criado e está pronto a ser
 * executado. O novo processo é comparado com os processos em execução e se o tempo
 * de duração esperado for menor do que o tempo restante em execução mais curto, 
 * o processo em execução é desafetado e substituído pelo novo processo.
 *
*/

/**
 * 3. Assuma a existẽncia de um programa geturls, que recebe uma URL como argumento,
 * descarrega a página correspondente e produz no stdout as URLs nela contidos, uma
 * por linha, cada uma escrita anatomicamente num único write; que cada URL não 
 * excede os 100 bytes, a existência de uma função readline como o mesmo protótipo de
 * read.
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>

#define MAX_URL_LENGTH 100
#define MAX_PROCESSES 10
#define TIMEOUT 60

int mains(int argc, char* argv[])
{
    const char* input_file = "input.txt";
    const char* output_file = argv[1];

    int input_fd = open(input_file, O_RDONLY);
    if (input_fd == -1)
    {
        perror("Error opening input file");
        return 1;
    }

    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_fd == -1)
    {
        perror("Error opening output file");
        return 1;
    }

    char url[MAX_URL_LENGTH];
    int processes_count = 0;
    struct timeval start, end;
    gettimeofday(&start, NULL);

    while (readline(input_fd, url, sizeof(url)) != 0)
    {
        while(processes_count >= MAX_PROCESSES)
        {
            gettimeofday(&end, NULL);
            double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;

            if(elapsed >= TIMEOUT)
            {
                printf("Timeout\n");
                return 1;
            }

            int status;
            pid_t pid = wait(&status);
            while((pid = waitpid(-1, &status, WNOHANG))>0)
            {
                if(WIFEXITED(status) || WIFSIGNALED(status))
                {
                    printf("Process %d finished\n", pid);
                    processes_count--;
                }
            }
        }
        usleep(100000);
    }

    if(processes_count < MAX_PROCESSES)
    {
        int pid = fork();
        if(pid == -1)
        {
            perror("Error forking");
            return 1;
        }
        else if(pid == 0)
        {
            execlp("./geturls", "./geturls", url, NULL);
            _exit(1);
        }
        else
        {
            processes_count++;
        }
    }

    while(processes_count > 0)
    {
        int status;
        pid_t pid = wait(&status);
        if(WIFEXITED(status) || WIFSIGNALED(status))
        {
            printf("Process %d finished\n", pid);
            processes_count--;
        }
    }
}