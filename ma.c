#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define NUMBER_SIZE   "%08ld"
#define POINTER_SIZE  "%08ld"
#define PRICE_SIZE    "%010.2f"
#define ARTIGO_LENG   28

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
  int fd = open("artigos", O_RDONLY, 0777);
  lseek(fd, offset, SEEK_SET);

  char buf[100]; // Perceber o pq de só dar com 100
  read(fd, &buf, ARTIGO_LENG);
  char *field = strndup(buf + 9, 8); // 9 = size(numero) + " "
  close(fd);

  char name[100];
  sprintf(name, "%s\n",nome);
  fd = open("strings", O_RDWR, 0777);
  offset = atol(field);
  lseek(fd, offset, SEEK_SET);
  write(fd, name, strlen(name));
  close(fd);
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
