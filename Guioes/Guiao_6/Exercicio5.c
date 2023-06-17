//Escreva um programa que emule o funcionamento do interpretador de comandos na 
//execução encadeada de ls /etc | wc -l

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pipefd[2];
    pid_t ls_pid, wc_pid;

    // Cria o pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Cria o processo filho para executar o comando "ls /etc"
    ls_pid = fork();

    if (ls_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (ls_pid == 0) {
        // Filho - executa o comando "ls /etc"

        // Redireciona a saída padrão para o descritor de escrita do pipe
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Executa o comando "ls /etc"
        execlp("ls", "ls", "/etc", NULL);

        // Se ocorrer um erro, exibe a mensagem de erro
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    // Cria o processo filho para executar o comando "wc -l"
    wc_pid = fork();

    if (wc_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (wc_pid == 0) {
        // Filho - executa o comando "wc -l"

        // Redireciona a entrada padrão para o descritor de leitura do pipe
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        // Executa o comando "wc -l"
        execlp("wc", "wc", "-l", NULL);

        // Se ocorrer um erro, exibe a mensagem de erro
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    // Pai - fecha os descritores de arquivo do pipe
    close(pipefd[0]);
    close(pipefd[1]);

    // Espera pela conclusão dos processos filhos
    wait(NULL);
    wait(NULL);

    return 0;
}
