#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "Utility.h"

#define SIZE 35


int getLinhas(int fd){
  off_t fim = lseek(fd, 0, SEEK_END);
  lseek(fd, 0, SEEK_SET);
  return fim / SIZE;
}


void agrega(){
  int fd;
  if ((fd = open("temp_agregador", O_RDONLY)) < 0) return ;

  int nmrTotalLinhas = getLinhas(fd);
  int nmrForks = (int)ceil((double) 0.01 * nmrTotalLinhas);
  int nmrMedioInst = (int)ceil((double) nmrTotalLinhas / nmrForks);
  int offset = 0;
  for (int i=0; i<nmrForks; i++){
    if (!fork()){
      char lin[SIZE];
      char* id;
      int res, total=0;
      int newfd;

      fd = open("temp_agregador", O_RDONLY);
      lseek(fd, offset, SEEK_SET);
      while((res = read(fd, lin, SIZE)) > 0 && total < nmrMedioInst){
        id = strndup(lin, 10);
        newfd = open(id, O_CREAT | O_APPEND | O_RDWR, 0666);
        write(newfd, lin, 35);
        close(newfd);
        total++;
      }
      _exit(0);
    }
    else{
      offset += SIZE * nmrMedioInst;
    }
  }

  for(int i=0; i<nmrForks; i++){
    wait(NULL);
  }
  int final = open("final", O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0666);
  int newfd;
  char id2[11], lin[35], lin2[35+1];
  int res;
  for(int i=0; i<10; i++){
    if(!fork()){
      int mont = 0,qtd = 0;
      char *monts, *qtds;
      snprintf(id2, 11, "%010d", i);
      if ((newfd = open(id2, O_RDONLY)) > 0){
        while((res = read(newfd, lin, 35)) > 0){
          qtds = strndup(lin + 11,11);
          qtd += atoi(qtds);
          monts = strndup(lin + 23,11);
          mont += atoi(monts);
        }
        snprintf(lin2, SIZE+1, "%s %011d %011d\n", id2, qtd, mont);
        write(final, lin2, SIZE);
      }
      unlink(id2);
      _exit(0);
    }
  }
  for(int i=0; i<10; i++)
    wait(NULL);

 unlink("temp_agregador");
 char time[1024];
 timeString(time);
 rename("final",time);

}

int main(){
  int fd = open("temp_agregador", O_CREAT | O_RDWR, 0666);
  int res;
  char buf[100];

  while((res = read(0, buf, 100)) > 0){
    write(fd, buf, res);
  }
  close(fd);

  agrega();

  return 0;
}
