#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "readline.h"
#include "defines.h"
#include "qsort.h"

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
  char buf[50];
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

  int fd = open("artigos", O_RDWR, 0777);
  lseek(fd, offset + NUMBER_LEN_I +1 + POINTER_LEN_I +1, SEEK_SET);

  char newPrice[DEFAULT_SIZE];
  snprintf(newPrice, PRICE_LEN_I +1, PRICE_SIZE, price); /* Porque +1 ? */
  write(fd, newPrice, PRICE_LEN_I);
}

char** sort_deprecated(int deprecated, int size){
  int res = 1;
  char** dep_buf = (char**)malloc(sizeof(char*) * size);

  for(int i=0; i<size; i++)
    dep_buf[i] = (char*)malloc(sizeof(char) * 50);

  for(int i=0; i<size && res > 0; i++){
    res = read(deprecated, dep_buf[i], DEPREC_LEN);
    lseek(deprecated, 1, SEEK_CUR);
  }

  sort(dep_buf, size);

  return dep_buf;
}

void adjust_artigos(int artigos, int y, int number_i, int leng_i, int total){
  lseek(artigos, 0, SEEK_SET);
  int size = y / (ARTIGO_LENG + 1);
  int offset = number_i * (ARTIGO_LENG + 1);
  leng_i -= total;
  lseek(artigos, offset + NUMBER_LEN_I + 1, SEEK_CUR);
  char lin[20], str[20];
  long int value;
  for (int i = number_i; i <= size; i++){
    read(artigos, lin, POINTER_LEN_I);
    value = atol(lin);
    value -= leng_i;
    snprintf(str, POINTER_LEN_I + 1, POINTER_SIZE, value);
    write(artigos, str, POINTER_LEN_I);
    lseek(artigos, ARTIGO_LENG + 1, SEEK_CUR);
  }
}

void verify_deprecated(){
  int artigos = open("artigos", O_RDWR, 0777);
  int strings = open("strings", O_RDWR, 0777);
  int deprecated = open("deprecated", O_RDWR, 0777);

  off_t offset_artigos = lseek(artigos, 0, SEEK_END);
  off_t offset_deprecated = lseek(deprecated, 0, SEEK_END);
  int total = 0;

  if (offset_deprecated >= (0.2 * offset_artigos)){
    int strings2 = open("temp", O_CREAT | O_RDWR, 0777);
    int x = offset_deprecated / (DEPREC_LEN + 1);
    lseek(strings, 0, SEEK_SET);
    lseek(deprecated, 0, SEEK_SET);
    char** dep_buf = sort_deprecated(deprecated, x);
    int pointer_i, number_i, leng_i;
    for(int i=0; i<x; i++){
      lseek(strings2, 0, SEEK_END);
      pointer_i = atoi(strndup(dep_buf[i], POINTER_LEN_I));
      number_i  = atoi(strndup(dep_buf[i] + POINTER_LEN_I + 1, NUMBER_LEN_I));
      leng_i    = atoi(strndup(dep_buf[i] + POINTER_LEN_I + 1 + NUMBER_LEN_I + 1 + MAX_INT_LEN, MAX_INT_LEN));
      char buf[pointer_i + 1];
      read(strings, buf, pointer_i);
      write(strings2, buf, pointer_i);
      lseek(strings, leng_i, SEEK_CUR);
      adjust_artigos(artigos, offset_artigos, number_i, leng_i, total);
      total += leng_i;
    }
    off_t offset_atual = lseek(strings, 0, SEEK_CUR);
    off_t offset_total = lseek(strings, 0, SEEK_END);
    char buf[offset_total - offset_atual + 1];
    read(strings, buf, offset_total - offset_atual);
    write(strings2, buf, offset_total - offset_atual);
  }
  close(artigos);
  close(strings);
  close(deprecated);
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
      verify_deprecated();
      alteraNome(argv[2], argv[3]);
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
