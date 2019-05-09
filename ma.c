#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include "readline.h"
#include "defines.h"
#include "compactador.h"

off_t insert_strings(char *nome){
  int fd = open("strings", O_CREAT | O_RDWR, 0666);
  off_t offset = lseek(fd, 0, SEEK_END);
  int size = strlen(nome) +1;
  char buf[size];
  sprintf(buf, "%s\n", nome);
  write(fd, buf, strlen(buf));
  return offset;
}

void insert_artigo(char *nome, char *preco){
  char buf[50];
  char buf2[13];
  int fd2 = open("stocks", O_CREAT | O_WRONLY | O_APPEND,0666);
  int fd = open("artigos", O_CREAT | O_RDWR, 0666);
  off_t s = lseek(fd, 0, SEEK_END);
  off_t offset = insert_strings(nome);
  int aux = s/ARTIGO_LENG;
  char auxArray[20];
  sprintf(auxArray,"%d\n",aux);
  write(STDOUT_FILENO,auxArray,strlen(auxArray));
  float price = atof(preco);
  snprintf(buf2,13,STK_SIZE,(long int) 0);
  write(fd2,buf2,12);
  sprintf(buf, NUMBER_SIZE" "POINTER_SIZE" "PRICE_SIZE"\n",s/ARTIGO_LENG,offset,price);
  write(fd, buf, strlen(buf));
  close(fd);
}

void alteraNome(char *codigo, char *nome){
  long int num = atol(codigo);
  off_t offset = num * (ARTIGO_LENG +1);

  int fd1 = open("artigos", O_RDWR, 0666);
  int fd2 = open("strings", O_RDWR, 0666);
  int fd3 = open("deprecated", O_CREAT | O_RDWR, 0666);

  lseek(fd1, offset, SEEK_SET);

  char buf[DEFAULT_SIZE]; // Perceber o pq de só dar com 100
  read(fd1, &buf, ARTIGO_LENG);

  char *identifier = strndup(buf, NUMBER_LEN_I);
  char *field = strndup(buf + NUMBER_LEN_I + 1, POINTER_LEN_I);

  char name[DEFAULT_SIZE];
  long offset2 = atol(field);
  sprintf(name, "%s\n",nome);
  lseek(fd2, offset2, SEEK_SET);

  char lin[DEFAULT_SIZE], buffer[DEFAULT_SIZE];
  ssize_t size = readln(fd2, lin, DEFAULT_SIZE);
  snprintf(buffer, DEPREC_LEN + 2, "%s %s %010ld\n", field, identifier, size);
  lseek(fd3, 0, SEEK_END);
  write(fd3, buffer, DEPREC_LEN + 1);

  offset2 = lseek(fd2, 0, SEEK_END);
  write(fd2, name, strlen(name));

  sprintf(buf, POINTER_SIZE, offset2);
  lseek(fd1, offset + NUMBER_LEN_I + 1, SEEK_SET);
  write(fd1, buf, POINTER_LEN_I);

  close(fd1);
  close(fd2);
  close(fd3);
}

void alteraPreco(char *codigo, char *preco){
  long int num = atol(codigo);
  double price = atof(preco);
  off_t offset = num * (ARTIGO_LENG +1);

  int fd = open("artigos", O_RDWR, 0666);
  lseek(fd, offset + NUMBER_LEN_I +1 + POINTER_LEN_I +1, SEEK_SET);

  char newPrice[DEFAULT_SIZE];
  snprintf(newPrice, PRICE_LEN_I +1, PRICE_SIZE, price); /* Porque +1 ? */
  write(fd, newPrice, PRICE_LEN_I);
}

void agrega(){
  int fd = open("pid_sv",O_RDONLY);
  char buf[10];
  read(fd,buf,10);
  //printf("pid %s\n",buf );
  int pid = atoi(buf);
  printf("pid %d\n",pid );
  kill(pid,SIGUSR1);
  close(fd);
}

int main(int argc, char** argv){

  ssize_t size;
  do{
    char* buffer = (char*)malloc(sizeof(char) * 1024);
    char** buf = (char**)malloc(sizeof(char*) * 3);
    char* found;
    int i=0;
    size = readln(STDIN_FILENO, buffer, 1024);
    while((found = strsep(&buffer," ")) != NULL)
      buf[i++] = strdup(found);

    buf[2] = strndup(buf[2], strlen(buf[2]) - 1); // tirar o \n

    switch(buf[0][0]){
      case 'i':
        insert_artigo(buf[1], buf[2]);
        break;
      case 'n':
        alteraNome(buf[1], buf[2]);
        verify_deprecated();
        break;
      case 'p':
        alteraPreco(buf[1], buf[2]);
        break;
      case 'a':
        agrega();
      default:
        break;
    }
    free(buffer);
    free(buf);
    free(found);
  } while(size > 0);

  /*
  if (argc < 4){
    perror("Número de elementos do input inferior ao que é necessário");
    exit(-1);
  }

  if(strcmp("i", argv[1]) == 0)
    insert_artigo(argv[2], argv[3]);
  else
    if(strcmp("n", argv[1]) == 0){
      alteraNome(argv[2], argv[3]);
      verify_deprecated();
    }
    else
      if(strcmp("p", argv[1]) == 0)
        alteraPreco(argv[2], argv[3]);
      else{
        return -1;
      }
  */
  return EXIT_SUCCESS;
}
