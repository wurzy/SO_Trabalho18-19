#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

void opcao1(char *string){
  int n = strlen(string);
  char buffer[n+2];
  snprintf(buffer,n+2,"%s\n",string);
  int server = open("server", O_WRONLY);

  if (server==-1) {
      perror("Server OFFLINE");
  }

  write(server,buffer, n+1);
  close(server);
}

void opcao2(char *string1, char *string2) {
  int n1 = strlen(string1);
  int n2 = strlen(string2);
  char buffer[n1+n2+3];
  snprintf(buffer,n1+n2+3,"%s %s\n",string1,string2);
  int server = open("server", O_WRONLY);

  if (server==-1) {
      perror("Server OFFLINE");
  }

  write(server, buffer, n1+n2+2);
  close(server);
}

int main(int argc, char* argv[]){
  if(argc==2) {
    opcao1(argv[1]);
  }
  else if(argc==3) {
    opcao2(argv[1],argv[2]);
  }
  else {
    puts("invalid");
  }
  return 0;
}
