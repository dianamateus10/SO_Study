//Escreva um programa que emule o funcionamento do interpretador de comandos na execucao encadeada de 
//grep -v ^# /etc/passwd | cut -f7 -d: | uniq | wc -l

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int pipefd[4][2];
    pid_t grep_pid, cut_pid, uniq_pid, wc_pid;

    // Cria os pipes
    for (int i = 0; i < 4; i++) {
        if (pipe(pipefd[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Cria o processo filho para executar o comando "grep -v ^# /etc/passwd"
    grep_pid = fork();

    if (grep_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (grep_pid == 0) {
        // Filho - executa o comando "grep -v ^# /etc/passwd"

        // Redireciona a saída padrão para o descritor de escrita do primeiro pipe
        close(pipefd[0][0]);
        dup2(pipefd[0][1], STDOUT_FILENO);
        close(pipefd[0][1]);

        // Executa o comando "grep -v ^# /etc/passwd"
        execlp("grep", "grep", "-v", "^#", "/etc/passwd", NULL);

        // Se ocorrer um erro, exibe a mensagem de erro
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    // Cria o processo filho para executar o comando "cut -f7 -d:"
    cut_pid = fork();

    if (cut_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cut_pid == 0) {
        // Filho - executa o comando "cut -f7 -d:"

        // Redireciona a entrada padrão para o descritor de leitura do primeiro pipe
        close(pipefd[0][1]);
        dup2(pipefd[0][0], STDIN_FILENO);
        close(pipefd[0][0]);

        // Redireciona a saída padrão para o descritor de escrita do segundo pipe
        close(pipefd[1][0]);
        dup2(pipefd[1][1], STDOUT_FILENO);
        close(pipefd[1][1]);

        // Executa o comando "cut -f7 -d:"
        execlp("cut", "cut", "-f7", "-d:", NULL);

        // Se ocorrer um erro, exibe a mensagem de erro
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    // Cria o processo filho para executar o comando "uniq"
    uniq_pid = fork();

    if (uniq_pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (uniq_pid == 0) {
        // Filho - executa o comando "uniq"

        // Redireciona a entrada padrão para o descritor de leitura do segundo pipe
        close(pipefd[1][1]);
        dup2(pipefd[1][0], STDIN_FILENO);
        close(pipefd[1][0]);

        // Redireciona a saída padrão para o descritor de escrita do terceiro pipe
        close(pipefd[2][0]);
        dup2(pipefd[2][1], STDOUT_FILENO);
        close(pipefd[2][1]);

        // Executa o comando "uniq"
        execlp("uniq", "uniq", NULL);

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

        // Redireciona a entrada padrão para o descritor de leitura do terceiro pipe
        close(pipefd[2][1]);
        dup2(pipefd[2][0], STDIN_FILENO);
        close(pipefd[2][0]);

        // Executa o comando "wc -l"
        execlp("wc", "wc", "-l", NULL);

        // Se ocorrer um erro, exibe a mensagem de erro
        perror("execlp");
        exit(EXIT_FAILURE);
    }

    // Fecha os descritores de arquivo dos pipes no processo pai
    for (int i = 0; i < 4; i++) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
    }

    // Espera pela conclusão dos processos filhos
    wait(NULL);
    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}
