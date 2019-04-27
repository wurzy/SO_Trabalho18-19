#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


/*Escreva um programa “servidor”, que fique a correr em background, e acrescente a um ficheiro de “log”
todas as mensagens que sejam enviadas por “clientes”. Escreva um programa cliente que envia para o
servidor o seu argumento. Cliente e servidor devem comunicar via pipes com nome.*/

int main(int argc, char const *argv[]){

	char buffer[2024];
	int fd,n;

	mkfifo("cliente", 0666); //fifo para trocar mensagens

	int log = open("log.txt",O_CREAT|O_WRONLY, 0666);
	fd = open("cliente",O_RDONLY);

	while((n = read(fd,buffer,1024))>0 || 1) {
		if(n>0) {
			write(log,buffer,n);
			write(log,"\n",2); //adds newline
			//write(1,buffer,n);
			printf("Client connected sent: %s \n",buffer);
		}
	}

return 0;


}
