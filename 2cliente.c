#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Escreva um programa “servidor”, que fique a correr em background, e acrescente a um ficheiro de “log”
todas as mensagens que sejam enviadas por “clientes”. Escreva um programa cliente que envia para o
servidor o seu argumento. Cliente e servidor devem comunicar via pipes com nome. */
/*
int main(int argc, char const *argv[]){

	int fd;

	fd = open("cliente",O_WRONLY);

	write(fd,argv[1],strlen(argv[1])+1);
	printf("Sent to server: %s \n",argv[1]);

	close(fd);

}
*/
int main(int argc, char** argv)
{
    int server = open("cliente", O_WRONLY);
    if (server < 0) {
        printf("Server offline\n");
        return 1;
    }
    for (int i = 1; i < argc; i++)
        write(server, argv[i], strlen(argv[i])+1);
    close(server);

    return 0;
}
