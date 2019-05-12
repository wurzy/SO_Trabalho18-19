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
    const char *myfifo = "server";

    int server_to_client;
    char myfifo2[100];

    char* str = (char*)malloc(sizeof(char*) * 1024);
    char* oi[2];
    char str2[1024];
    char str3[1024];
    char* found;
    int i = 0;
    pid_t pid = getpid();
    sprintf(myfifo2,"%d",pid);
    mkfifo(myfifo2, 0666);

    while((n = myreadln(0,str,1024))>0) {
      if(!fork()) {
        client_to_server = open(myfifo, O_WRONLY);

          while((found = strsep(&str," ")) != NULL)
            oi[i++] = strdup(found);

          if(i==1) {
            snprintf(str2, 17, "%05d %010d",pid, atoi(oi[0]));
          }
          else if (i==2) {
            snprintf(str2, 28, "%05d %010d %010d",pid, atoi(oi[0]), atoi(oi[1]));
          }
          write(client_to_server,str2,strlen(str2));
          close(client_to_server);
          _exit(0);
        }
        else {
          wait(NULL);
          server_to_client = open(myfifo2, O_RDONLY);
          while (1) {
            n = myreadln(server_to_client,str3,20);
            str3[n]='\0';
            if (n>0) {
              write(1,str3,n);
              break;
            }
          }
          close(server_to_client);
      }
    }
    unlink(myfifo2);
}

int main(int argc, char* argv[]){
  startClient();
  return EXIT_SUCCESS;
}
