#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "readline.h"
#include "defines.h"

off_t insert_strings(char *nome){
  int fd = open("strings", O_CREAT | O_RDWR, 0777);
  off_t offset = lseek(fd, 0, SEEK_END);
  int size = strlen(nome) +1;
  char buf[size];
  sprintf(buf, "%s\n", nome);
  write(fd, buf, strlen(buf));
  return offset;
}

void insert_artigo(char *nome, char *preco){
  char buf[ARTIGO_LENG +1];
  int fd = open("artigos", O_CREAT | O_RDWR, 0777);
  off_t s = lseek(fd, 0, SEEK_END);
  off_t offset = insert_strings(nome);
  float price = atof(preco);

  sprintf(buf, NUMBER_SIZE" "POINTER_SIZE" "PRICE_SIZE"\n",s/ARTIGO_LENG,offset,price);
  write(fd, buf, strlen(buf));
  close(fd);
}

void alteraNome(char *codigo, char *nome){
  long int num = atol(codigo);
  off_t offset = num * (ARTIGO_LENG +1);
  int fd1 = open("artigos", O_RDWR, 0777);
  int fd2 = open("strings", O_RDWR, 0777);
  int fd3 = open("deprecated", O_CREAT | O_RDWR, 0777);
  lseek(fd1, offset, SEEK_SET);

  char buf[100]; // Perceber o pq de só dar com 100
  read(fd1, &buf, ARTIGO_LENG);
  char *field = strndup(buf + 9, 8); // 9 = size(numero) + " "

  char name[100];
  sprintf(name, "%s\n",nome);
  int offset2 = atol(field);
  lseek(fd2, offset2, SEEK_SET);

  char lin[100];
  ssize_t size;
  size = readln(fd2, lin, 100);
  sprintf(buf, "%s %s\n", field, lin);
  lseek(fd3, 0, SEEK_END);
  write(fd3, buf, 9 + size - 1);

  offset2 = lseek(fd2, 0, SEEK_END);
  printf("%ld\n", offset);
  write(fd2, name, strlen(name));

  sprintf(buf, POINTER_SIZE, offset2);
  lseek(fd1, offset + NUMBER_LEN_I + 1, SEEK_SET);
  write(fd1, buf, POINTER_LEN_I);

  close(fd1);
  close(fd2);
  close(fd3);
}

int main(int argc, char** argv){
  if (argc < 4){
    perror("Número de elementos do input inferior ao que é necessário");
    exit(-1);
  }

  if(strcmp("i", argv[1]) == 0)
    insert_artigo(argv[2], argv[3]);
  else
    if(strcmp("n", argv[1]) == 0)
      alteraNome(argv[2], argv[3]);
  /*
    else
      if(strcmp("p", argv[1]) == 0)
        alteraPreco(argv[2], argv[3]);
        */
  return 0;
}
