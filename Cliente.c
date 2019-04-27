#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void cliente(char *string, int n){
  int server = open("server", O_WRONLY);
  if (server==-1) {
      perror("Server OFFLINE");
  }

  write(server, string, n);
  close(server);
}

int main(){
  char buffer[1024];
  int n;
  while((n = read(0,buffer,1024))>0 || 1) {
      //write(1,buffer,n);
      cliente(buffer,n);
	}

  return 0;
}
