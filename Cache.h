#ifndef CACHE_H
#define CACHE_H

struct data {
  int id;
  int vendas;
  int preco;
};

typedef struct cache {
  struct data* cached[CACHE_SIZE];
  int ocupados;
  int full; // 0 1
}*Cache;

Cache initCache(Cache x);
void freeCache(Cache x);
int indexOf(Cache x,int id,int *mins);
Cache addToCache(Cache x,int id);
int fetchPreco(Cache x,int id);
void fillCache(Cache x);

#endif
