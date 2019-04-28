#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "defines.h"

static int verificaID(int id){
  int fd = open("STOCKS",O_RDONLY, 0666);

  off_t max = lseek(fd,0,SEEK_END);
  // calcula numero total / 11 (da o ID)
  double atual = ((double) max) / (STK_LEN_TOT);
  int at = (int) atual;

  close(fd);

  return (id>=at?0:1);
}

void getPreco_Stock_Of(int id) {

  if(!verificaID(id)) {
    // nao e possivel ir buscar este id
    puts("not available");
  }
  else {
    int fd = open("STOCKS",O_RDONLY, 0666);
    char stock[STK_LEN + 1];

    char ints[32];
    char floats[64];

    int stockNum;
    float precoNum;

    char preco[PRICE_LEN_I + 1];
    int fd2 = open("artigos",O_RDONLY,0666);

    //onde esta o preço relativo ao ficheiro
    off_t offsetStock = id*(STK_LEN_TOT);
    off_t offsetPreco = id*(ARTIGO_LENG+1) + 2 + NUMBER_LEN_I + POINTER_LEN_I;

    // ler desse offset 11 bytes (conto com o \n)
    lseek(fd2,offsetPreco,SEEK_SET);
    read(fd2,&preco,PRICE_LEN_I);

    //tratamento das strings
    precoNum = atof(preco);
    sprintf(floats,"%.2f",precoNum);
    write(1,"PRECO: ", 7);
    write(1,floats,strlen(floats));

    write(1," ",1);

    // ler desse offset 9 bytes (conto com o \n)
    lseek(fd,offsetStock,SEEK_SET);
    read(fd,&stock,STK_LEN);
    //tratamento das strings
    stockNum = atoi(stock);
    sprintf(ints,"%d\n",stockNum);
    write(1,"STOCK: ",7);
    write(1,ints,strlen(ints));

    close(fd);
    close(fd2);
  }
}

int is_Overflow(int x, int y){
  //if(x - 1 > (MAX_INT_NUM) || x - 1 < (MIN_INT_NUM)) return 1;
  //printf("Debug %d\n", sizeof(*MEMx));
//  if(sizeof(*MEMx)>4) return 1;
  if ((y > 0 && x > MAX_INT_NUM - y) || (y < 0 && x < MIN_INT_NUM - y)){
    return 1;
  }
  else{
    return 0;
  }
/*
  // Checking if addition will cause overflow
    if (num1 > MAX_INT_NUM - num2)
        return -1;

   // No overflow occured
    else
        return num1 + num2;
*/
}


void updateStock_Of(int id, int quantidade) {
  if(!verificaID(id)){ //|| is_Overflow(quantidade,1)){
    puts("not available");
  }
  else{
    char ints[15];
    int fd = open("STOCKS",O_WRONLY,0666);

    sprintf(ints, STK_SIZE , (long int) quantidade);

    off_t offsetStock = id*(STK_LEN_TOT);
    lseek(fd,offsetStock,SEEK_SET);
    write(fd,ints,strlen(ints));
    close(fd);

  }
}

void startServer(){
  char buffer[1024];
  int n;
  int flag = 0;
  char space = ' ';
  mkfifo("server", 0666); //fifo para trocar mensagens
  int server = open("server",O_RDONLY,0666);

  while((n = read(server,buffer,1024))>0 || 1) {
    if(n>0) {
      for(int i = 0; i < n; i++) {
        if(buffer[i] == space) {
          updateStock_Of(atoi(buffer),atoi(buffer + i));
          flag = 1;
          break;
        }
      }
      if(!flag) {
        //write(1,"ola\n",4);
        getPreco_Stock_Of(atoi(buffer));
      }
    }
    flag = 0;
  }
  close(server);
}

int main() {

  //char *string = strdup("Hello there, peasants!");
  //off_t s = 5;
/*
  for(int i = 0; i<21; i++) {
    updateStock_Of(i,MAX_INT_NUM);
    getPreco_Stock_Of(i);
  }
  */
  startServer();


/*
  char buffer[1024];
	int fd,n;

	mkfifo("server", 0666); //fifo para trocar mensagens

	//int stocks = open("STOCKS",O_CREAT | O_WRONLY, 0666);
	fd = open("server",O_RDONLY);

	while((n = read(fd,buffer,1024))>0 || 1) {
		if(n>0) {
      //opcao2Gestao(string);
			//write(stocks,buffer,n);
			write(1,buffer,n);
		}
	}
  close(fd);
  */
}
