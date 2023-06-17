//Implemente uma versão simplificada da função system(). Ao contrário
//da função original, não tem de suportar qualquer tipo de redirecionamento
//ou composição/encadeamento de programas executáveis. O único argumento
//deverá ser uma string que especifica um programa executável e uma eventual
//lista de argumentos. Procure que o comportamento e valor de retorno da sua
//função sejam compatíveis com a função original.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int my_system(const char *command) {
    pid_t child_pid = fork();

    if (child_pid == -1) {
        // Error creating child process
        return -1;
    } else if (child_pid == 0) {
        // Child process code
        char *args[64];
        char *token;
        int i = 0;

        // Tokenize the command string
        token = strtok((char *)command, " ");
        while (token != NULL) {
            args[i] = token;
            token = strtok(NULL, " ");
            i++;
        }
        args[i] = NULL;

        // Execute the executable program with arguments
        execvp(args[0], args);

        // If execvp returns, there was an error in execution
        perror("Error executing program");
        _exit(1);
    } else {
        // Parent process code
        int status;
        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            // Child process terminated normally
            return WEXITSTATUS(status);
        } else if (WIFSIGNALED(status)) {
            // Child process terminated due to a signal
            return WTERMSIG(status);
        } else {
            // Other type of abnormal termination
            return -1;
        }
    }
}

int main() {
    int ret = my_system("ls -l");
    printf("Return code: %d\n", ret);
    return 0;
}
