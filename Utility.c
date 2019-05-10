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
  sprintf(buffer,"%02d-%02d-%02dT%02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
}

size_t myreadln(int fildes, void* buf, ssize_t nbyte){
  ssize_t size = 0;
  int res;
  char byte;
  char *buffer = (char*)buf; // casting void*
  while ((res = read(fildes,&byte,1) > 0) && size < nbyte){
    if (byte == '\0')
      return size;
    buffer[size++] = byte;
    if (byte == '\n')
      return size;
  }
  return size;
}



/*
int main(){
  char buffer[25];
  timeString(buffer);
  printf("%s", buffer);
  printOut(buffer);
}
*/
