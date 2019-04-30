#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
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
  int fd = open("artigos", O_CREAT | O_RDWR, 0666);
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

int main(int argc, char** argv){
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
        /* escrever para o stderr */
        return -1;
      }
  return EXIT_SUCCESS;
}
