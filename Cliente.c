#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include "defines.h"
#include "Utility.h"
/*
void opcao1(char *string){
  int n = strlen(string);
  char buffer[n+2];
  snprintf(buffer,n+2,"%s\n",string);
  int server = open("server", O_WRONLY);

  if (server==-1) {
      printOut("Server OFFLINE");
  }
  //write(1,buffer,n+1);
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
      printOut("Server OFFLINE");
  }

  write(server, buffer, n1+n2+2);
  close(server);
}

void sendToServer(char *string) {
  int server = open("server", O_WRONLY);
  //write(1,string,1);
  //if(isValidString(string, n)) {
    int n = strlen(string);
    char buffer[n+2];
    snprintf(buffer,n+2,"%s\n",string);

    if (server==-1) {
        printOut("Server OFFLINE");
    }
    //write(1,buffer,n+1);
    write(server,buffer, n+1);
  //}
  //else {
    //char aux[2] = "\n";
    //write(server,aux,1);
  //}
  close(server);
}
*/

int confirm(char *string,int len) {
  int spaces = 0,where;
  for(int i = 0; i < len; i++) {
    if (string[i]==' '){
      spaces++;
      where = i;
    }
  }

  if(spaces==1) {
    if (where==0 || where==(len-2)) return 0;
    for(int i = 0; i < where; i++) {
      if(!isdigit(string[i])){
        return 0;
      }
    }
    for(int i = where + 1; i< len -1; i++) {
      if(!isdigit(string[i])){
        if(string[i]!='-') {
          return 0;
        }
      }
    }
  }
  else if(spaces>1) {
    return 0;
  }
  else {
    if (len == 1) {
      return 0;
    }
    for(int i = 0; i < len - 1; i++) {
      if(!isdigit(string[i])){
        return 0;
      }
    }
  }
  return 1;
}

void startClient(){
  int client_to_server,n = 1;
const char *myfifo = "server"; //fifo unico

int server_to_client; // useless
char myfifo2[100]; //diretoria primitiva

char* str = (char*)malloc(sizeof(char*) * 1024);
char* oi[2];
char str2[1024];
char str3[1024];
char* found;
int i = 0;

client_to_server = open(myfifo, O_WRONLY);

pid_t pid = getpid(); // pid processo
sprintf(myfifo2,"%d",pid);
mkfifo(myfifo2, 0666);

while((n = myreadln(0,str,1024))>0) {
  if(!fork()) {
      while((found = strsep(&str," ")) != NULL)
        oi[i++] = strdup(found);
      printf("SOU I = %d\n",i );
      if(i==1) {
        printf("DECIDI I = 1\n" );
        snprintf(str2, 17, "%05d %010d",pid, atoi(oi[0]));
      }
      else if (i==2) {
        printf("DECIDI I = 2\n" );
        snprintf(str2, 28, "%05d %010d %010d",pid, atoi(oi[0]), atoi(oi[1]));

      }
      //printf("campo 1 : %s\n", oi[0]);
      //printf("campo 2 : %s", oi[1]);
      printf("WRITTEN: %s\n",str2);
      write(client_to_server,str2,strlen(str2));
      close(client_to_server);
      _exit(0);
  }
  else {
    wait(NULL);
    server_to_client = open(myfifo2, O_RDONLY);
    while (1) {
      n = myreadln(server_to_client,str3,30);
      str3[n]='\0';
      //str3[n+1]='\0';

      if (n>0) {
        //printf("foi maior ZERO!\n");
        printf("resultado de ler do sv --> %s", str3);
        break;
      }
      //else {printf("FOI ZERO\n");}
    }
    /*
    char log[40];
    sprintf(log,"%d.txt",pid);
    int fd = open(log,O_CREAT | O_WRONLY,0666);
    lseek(fd,0,SEEK_END);
    write(fd,str3,n);
    printf("READ: %s", str3);
    close(fd);
    */
    close(server_to_client);
  }
}

unlink(myfifo2);
//printf("%s\n", myfifo2);
//close(server_to_client);
//close(client_to_server);


//printf("%s\n", myfifo2);
//close(server_to_client);
//close(client_to_server);

  /*char buffer[1024];
  int n = 1;//x=0;
  int sv = open("server",O_WRONLY,0666);
  while(n>0) {
    //write(1,buffer,n);
    //sendToServer(buffer);
    n = myreadln(0,buffer,1024);
    if(confirm(buffer,n)) {
      write(sv,buffer,n);
    }
}
  close(sv);
  */
}

int main(int argc, char* argv[]){
  startClient();
  return EXIT_SUCCESS;
}
