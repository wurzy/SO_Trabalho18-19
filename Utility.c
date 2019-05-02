#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "time.h"

void createFifo(char *name) {
    if(mkfifo(name,0666)==-1) {
      puts("Pipe error");
    }
}

void printOut(char *string) {
  int ln = strlen(string);
  write(1,string,ln);
}

//mete no buffer a string com o tempo
void timeString(char buffer[]){
  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  sprintf(buffer,"%02d-%02d-%02dT%02d:%02d:%02d\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}


/*
int main(){
  char buffer[25];
  timeString(buffer);
  printf("%s", buffer);
  printOut(buffer);
}
*/
