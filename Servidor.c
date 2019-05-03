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
  int fd = open("stocks",O_RDONLY, 0666);

  off_t max = lseek(fd,0,SEEK_END);
  // calcula numero total / 11 (da o ID)
  double atual = ((double) max) / (STK_LEN_TOT);
  int at = (int) atual;

  close(fd);

  return (id>=at?0:1);
}

// qAtual = qtd ficheiro, qInput = qtd input
static int manageStock(int qAtual, int qInput) {
  //if(qInput < 0 && novaQ < 0) { // 200 e -300 -> 0
  	//return -1;
  //}
  return qAtual + qInput;
}

void getPreco_Stock_Of(int id) {

  if(!verificaID(id)) {
    // nao e possivel ir buscar este id
    printOut("Invalid ID\n");
  }
  else {
    int fd = open("stocks",O_RDONLY, 0666);
    char stock[STK_LEN + 1];

    char toPrint[128];

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
    close(fd);
    close(fd2);
    printOut(toPrint);
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

void manageVendas(int id, int quantidade){

  int fd = open("vendas",O_RDWR,0666);
  off_t seeker = lseek(fd,0,SEEK_END);
  int modqtd = abs(quantidade);
  /*
  //se so for um EOF
  if(seeker<1) {
    char vendas[16];
    sprintf(vendas,VENDAS_S, (long int) 0);
    write(fd,vendas,VENDAS_INT + 1);
  }
  else{
  */
    char all[64];
    int fd2 = open("artigos", O_RDONLY, 0666);
    char preco[PRICE_LEN_I + 1];
    off_t offsetPreco = id*(ARTIGO_LENG+1) + 2 + NUMBER_LEN_I + POINTER_LEN_I;
    //char id[NUMBER_LEN_I + 1];
    //char quantidade[STK_LEN + 1]
    lseek(fd2,offsetPreco,SEEK_SET);
    read(fd2,preco,PRICE_LEN_I);

    sprintf(all,"%010ld %011ld %s\n",(long int) id, (long int) modqtd, preco);
    lseek(fd,seeker,SEEK_SET);
    write(fd,all,strlen(all));
    close(fd2);
//  }
  close(fd);
}

void updateStock_Of(int id, int quantidade) {
  if(!verificaID(id)){ //|| is_Overflow(quantidade,1)){
    printOut("Invalid ID\n");
  }
  else{
    //char *printer = malloc(sizeof(char)*100);
    int fdven = open("vendas",O_CREAT | O_RDWR,0666);
    off_t seeker = lseek(fdven,0,SEEK_END);

    //se so for um EOF
    if(seeker<1) {
      char vendas[16];
      sprintf(vendas,VENDAS_S, (long int) 0);
      write(fdven,vendas,VENDAS_INT + 1);
    }

    char printer[128];
    char ints[32];
    char stockArr[STK_LEN+1];
    int fd = open("stocks",O_RDWR,0666);

    off_t offsetStock = id*(STK_LEN_TOT);

    lseek(fd,offsetStock,SEEK_SET);
    read(fd,&stockArr,STK_LEN);
    int stockfrom = atoi(stockArr);
    int stock = manageStock(stockfrom,quantidade);

    // serve para quando stock = 1 e qtd = -23, ele escrever no vendas 1
    if(stock<0) {
      stock = 0;
      if(stockfrom>0) {
        quantidade = stockfrom;
        manageVendas(id,quantidade);
      }
    }
    else if (quantidade<0 ) {
      manageVendas(id,quantidade);
    }

    sprintf(ints, STK_SIZE , (long int) stock);

    lseek(fd,offsetStock,SEEK_SET);
    write(fd,ints,strlen(ints));

    sprintf(printer,"OK. ID [%d]: updated STOCK from [%d] to [%d]\n",id,stockfrom,stock);
    close(fd);
    close(fdven);
    printOut(printer);

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
