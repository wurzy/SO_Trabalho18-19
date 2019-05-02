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

static int verificaID(int id){
  int fd = open("STOCKS",O_RDONLY, 0666);

  off_t max = lseek(fd,0,SEEK_END);
  // calcula numero total / 11 (da o ID)
  double atual = ((double) max) / (STK_LEN_TOT);
  int at = (int) atual;

  close(fd);

  return (id>=at?0:1);
}

// qAtual = qtd ficheiro, qInput = qtd input
static int manageStock(int qAtual, int qInput) {
  int novaQ;
   novaQ = qAtual + qInput;
  if(qInput < 0 && novaQ < 0) { // 200 e -300 -> 0
  	return 0;
  }

  return novaQ;
}

void getPreco_Stock_Of(int id) {

  if(!verificaID(id)) {
    // nao e possivel ir buscar este id
    printOut("Invalid ID\n");
  }
  else {
    int fd = open("STOCKS",O_RDONLY, 0666);
    char stock[STK_LEN + 1];

    char toPrint[100];

    char preco[PRICE_LEN_I + 1];
    int fd2 = open("artigos",O_RDONLY,0666);

    //onde esta o preço relativo ao ficheiro
    off_t offsetStock = id*(STK_LEN_TOT);
    off_t offsetPreco = id*(ARTIGO_LENG+1) + 2 + NUMBER_LEN_I + POINTER_LEN_I;

    // ler desse offset 11 bytes (conto com o \n)
    lseek(fd2,offsetPreco,SEEK_SET);
    read(fd2,&preco,PRICE_LEN_I);

    //tratamento das strings
    sprintf(toPrint,"PRECO: %f ",atof(preco));

    // ler desse offset 9 bytes (conto com o \n)
    lseek(fd,offsetStock,SEEK_SET);
    read(fd,&stock,STK_LEN);

    sprintf(toPrint,"%sSTOCK: %d\n",toPrint,atoi(stock));

    //write(STDOUT_FILENO,toPrint,strlen(toPrint));
    printOut(toPrint);
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
    printOut("Invalid ID\n");
  }
  else{
    char print[100];
    char ints[15];
    char stockArr[STK_LEN];
    int fd = open("STOCKS",O_RDWR,0666);

    off_t offsetStock = id*(STK_LEN_TOT);

    lseek(fd,offsetStock,SEEK_SET);
    read(fd,&stockArr,STK_LEN);
    int stock = manageStock(atoi(stockArr),quantidade);

    sprintf(ints, STK_SIZE , (long int) stock);

    lseek(fd,offsetStock,SEEK_SET);
    write(fd,ints,strlen(ints));

    //sprintf(print,"OK. ID [%d]: updated STOCK from [%d] to [%d]\n",id,atoi(stockArr),stock);
  //  printOut(print);
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
