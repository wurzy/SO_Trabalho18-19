#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
//#include "Cliente.h"

int main() {
  char buffer[1024];
	int fd,n;

	mkfifo("server", 0666); //fifo para trocar mensagens

	int log = open("log.txt",O_CREAT | O_WRONLY, 0666);
	fd = open("server",O_RDONLY);

	while((n = read(fd,buffer,1024))>0 || 1) {
		if(n>0) {
			write(log,buffer,n);
			//write(log,"\n",2); //adds newline
			//write(1,buffer,n);
			write(1,buffer,n);
		}
	}

}
