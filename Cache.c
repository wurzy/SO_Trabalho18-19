#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "defines.h"
#include "Utility.h"
#include "Cache.h"

Cache initCache(Cache cache){
  cache = malloc(sizeof(struct cache));
  for(int i = 0; i < CACHE_SIZE; i++) {
    cache->cached[i] = malloc(sizeof(struct data));
  }
  cache->ocupados = 0;
  cache->full = 0;
  return cache;
}

void freeCache(Cache cache){
  if(cache) {
    for(int i= 0; i < cache->ocupados; i++) {
      free(cache->cached[i]);
    }
    free(cache);
  }
}

int indexOf(Cache cache,int id,int *mins) {
  int min = *mins;
  if(cache) {
    int qtd_ven = cache->cached[0]->vendas;
    for(int i = 0; i < cache->ocupados; i++) {
      if (cache->cached[i]->vendas < qtd_ven){
        qtd_ven = cache->cached[i]->vendas;
        min = i;
      }
      if(cache->cached[i]->id == id) {
        *mins = 1;
        return i;
      }
    }
  }
  return min;
}

Cache addToCache(Cache cache,int id){
  int zero = 0;
  int existe = indexOf(cache,id,&zero);
  if(zero) {
    cache->cached[existe]->vendas++;
  }
  else {
    int fd = open("artigos",O_RDONLY);
    off_t offset = id*29 + 19;
    char buffer[12];
    lseek(fd,offset,SEEK_SET);
    read(fd,buffer,12);
    int prc = atoi(buffer);
    if(cache->full) {
      cache->cached[existe]->id = id;
      cache->cached[existe]->vendas = 1;
      cache->cached[existe]->preco = prc;
    }
    else {
        cache->cached[cache->ocupados]->id = id;
        cache->cached[cache->ocupados]->preco = prc;
        cache->cached[cache->ocupados]->vendas = 1;
        cache->ocupados++;
        if(cache->ocupados >= CACHE_SIZE) {
          cache->full = 1;
        }
    }
    close(fd);
  }
  return cache;
}

int fetchPreco(Cache cache,int id){
  if(cache) {
    for(int i = 0; i < cache->ocupados;i++) {
      if(id==cache->cached[i]->id) {
        return cache->cached[i]->preco;
      }
    }
  }
  return -1;
}
/*
void printCache(Cache cache){
  if(cache) {
    printf("Ocupados: %d / %d\n",cache->ocupados,CACHE_SIZE );
    if(cache->ocupados) {
      printf("Está cheia?: %s\n",cache->full?"sim":"não");
    }
    else {
      printf("Empty Cache.\n");
    }
    for(int i = 0; i < cache->ocupados; i++) {
      printf("DATA: ID [%d] Vendas [%d] Preco [%d]:\n",cache->cached[i]->id,cache->cached[i]->vendas,cache->cached[i]->preco);
    }
    printf("Done.\n\n");
  }
  else {
    printf("There is no Cache.\n");
  }
}
*/
