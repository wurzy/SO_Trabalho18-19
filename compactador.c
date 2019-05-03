#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "qsort.h"
#include "defines.h"

char** read_deprecated(int deprecated, int size){
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


void adjust_strings(int deprecated, int temp, int strings, int point, int leng, int acc){
  char arr[point];
  int res = read(strings, arr, point - acc);
  write(temp, arr, res);
  lseek(strings, leng, SEEK_CUR);
}

void adjust_artigos(int artigos, int point, int numb, int leng, int size_artigos){
  int offset;
  char str[10];
  for(int j=numb; j<size_artigos; j++){
    offset = j * 29 + 9;
    lseek(artigos, offset, SEEK_SET);
    read(artigos, str, 8);
    lseek(artigos, offset, SEEK_SET);
    int atual = atoi(str);
    if (atual >= point){
      int novo = atual - leng;
      snprintf(str, 8 + 1, "%08d", novo);
      write(artigos, str, 8);
    }
  }
}

void paste(int strings, int temp){
  off_t of1, of2;
  of1 = lseek(strings, 0, SEEK_CUR);
  of2 = lseek(strings, 0, SEEK_END);
  char buf[of2-of1];
  lseek(strings, of1, SEEK_SET);
  int res = read(strings, buf, of2 - of1);
  write(temp, buf, res);
}

void verify_deprecated(){
  int deprecated = open("deprecated", O_RDWR, 0666);
  int strings = open("strings", O_RDWR, 0666);
  int artigos = open("artigos", O_RDWR, 0666);

  off_t x = lseek(deprecated, 0, SEEK_END);
  long int size = x / 29;
  lseek(deprecated, 0, SEEK_SET);

  off_t y = lseek(artigos, 0, SEEK_END);
  long int size_artigos = y / 29;
  lseek(artigos, 0, SEEK_SET);

  if (size < size_artigos * 0.2)
    return ;

  int temp = open("temp", O_CREAT | O_RDWR | O_TRUNC, 0666);

  char** buffer = read_deprecated(deprecated, size);
  int numb, point, leng;
  int acc = 0;
  for(int i=0; i<size; i++){
    point = atoi(strndup(buffer[i], 8));
    numb = atoi(strndup(buffer[i] + 9, 8));
    leng = atoi(strndup(buffer[i] + 18, 10));
    adjust_strings(deprecated, temp, strings, point, leng, acc);
    adjust_artigos(artigos, point, numb, leng, size_artigos);
    acc += point + leng;
  }
  paste(strings, temp);

  close(deprecated);
  close(strings);
  close(artigos);
  close(temp);

  /* tentar fazer com exec's: rm e mv */
  remove("strings");
  remove("deprecated");
  rename("temp", "strings");
}
