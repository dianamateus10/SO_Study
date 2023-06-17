//Escreva um programa que execute o comando wc num processo filho. O processo
//pai deve enviar ao filho através de um pipe anónimo uma sequência de linhas
//de texto introduzidas pelo utilizador no seu standard input. Recorra à técnica
//de redireccionamento de modo a associar o standard input do processo filho ao
//descritor de leitura do pipe anónimo criado pelo pai. Recorde a necessidade de
//fechar os descritores de escrita no pipe de modo a verificar se a situação de EOF

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv)
{
   size_t i = 1;

   for(; *argv[i] == '-'; i++)
   {
        int fdr = 0, fdw = 1;
        
        switch(argv[i][1])
        {
            case 'i':
                fdr = open(argv[i+1], O_CREAT | O_RDONLY | O_TRUNC, 0640);
                if(fdr < 0)
                {
                    perror("open error");
                    _exit(1);
                }
                dup2(fdr, 0);
                close(fdr);
                break;
            case 'o':
                fdw = open(argv[i+1], O_CREAT | O_WRONLY | O_TRUNC, 0640);
                if(fdw < 0)
                {
                    perror("open error");
                    _exit(1);
                }
                dup2(fdw, 1);
                close(fdw);
                break;
            default:
                fputs("Invalid argument\n", stderr);
                return 1;
        }
   }

    execv(argv[i+1], &argv[i+2]);
}