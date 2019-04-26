#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int myCompare(const void* a, const void* b){

  return strcmp(*(const char**)a, *(const char**)b);
}

void sort(char** arr, int n){
  qsort(arr, n, sizeof(char*), myCompare);
}
