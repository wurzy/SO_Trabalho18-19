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

int getPreco_Stock_Of(int id, int returned[]) {

  if(!verificaID(id)) {
    // nao e possivel ir buscar este id
    return -1;
  }
  else {
    int fd = open("stocks",O_RDONLY, 0666);
    char stock[STK_LEN + 1];

    //char toPrint[128];

    char preco[PRICE_LEN_I + 1];
    int fd2 = open("artigos",O_RDONLY,0666);

    //onde esta o preço relativo ao ficheiro
    off_t offsetStock = id*(STK_LEN_TOT);
    off_t offsetPreco = id*(ARTIGO_LENG+1) + 2 + NUMBER_LEN_I + POINTER_LEN_I;

    // ler desse offset 11 bytes (conto com o \n)
    lseek(fd2,offsetPreco,SEEK_SET);
    read(fd2,&preco,PRICE_LEN_I);

    //tratamento das strings
    //sprintf(toPrint,"PRECO: %f ",atof(preco));
    returned[0] = atof(preco)*100;

    // ler desse offset 9 bytes (conto com o \n)
    lseek(fd,offsetStock,SEEK_SET);
    read(fd,&stock,STK_LEN);

    //sprintf(toPrint,"%sSTOCK: %d\n",toPrint,atoi(stock));
    returned[1]=atoi(stock);

    //write(STDOUT_FILENO,toPrint,strlen(toPrint));
    close(fd);
    close(fd2);
    //printOut(toPrint);
  }
  return 0;
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
    int montante = atoi(preco)*modqtd;
    sprintf(all,"%010ld %011ld %011ld\n",(long int) id, (long int) modqtd, (long int) montante);
    lseek(fd,seeker,SEEK_SET);
    write(fd,all,strlen(all));
    close(fd2);
//  }
  close(fd);
}

//dar fix a isto por causa do return
int updateStock_Of(int id, int quantidade) {
  if(!verificaID(id)){ //|| is_Overflow(quantidade,1)){
    return (-1);
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
      close(fdven);
    }

    char ints[32];
    char stockArr[STK_LEN+1];
    int fd = open("stocks",O_RDWR,0666);

    off_t offsetStock = id*(STK_LEN_TOT);

    lseek(fd,offsetStock,SEEK_SET);
    read(fd,&stockArr,STK_LEN);
    int stockfrom = atoi(stockArr);
    printf("STOCKFROM: %d\n", stockfrom);
    printf("qtdade %d\n",quantidade );
    if(quantidade < 0 && stockfrom<abs(quantidade)) {
      printf("N quero atualizar\n" );
      return -1;
    }
    // serve para quando stock = 1 e qtd = -23, ele escrever no vendas 1
/*
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
    */
    manageVendas(id,quantidade);

    int stock = stockfrom + quantidade;
    sprintf(ints, STK_SIZE , (long int) stock);

    lseek(fd,offsetStock,SEEK_SET);
    write(fd,ints,strlen(ints));

    //sprintf(printer,"OK. ID [%d]: updated STOCK from [%d] to [%d]\n",id,stockfrom,stock);
    close(fd);
    //printOut(printer);
    return stock;
  }
}

void startServer(){
  char sender[100];
  int client_to_server, n;
   const char *myfifo = "server"; // const

   int server_to_client;
   char myfifo2[100];


   mkfifo(myfifo, 0666);

   printf("Server ON.\n");
   client_to_server = open(myfifo, O_RDWR);


   while(1) {
     char* buf = (char*)malloc(sizeof(char*) * 1024);
     char* found;
     int i = 0;
     //char eu[1024];
     char** new = (char**)malloc(sizeof(char*) * 3);

     puts("tou aqui");
     if((n = read(client_to_server,buf,27)) > 0) {
       puts("passei");

       printf("READ from cl: %s\n", buf);

       /* pid id *qtd* */
       while((found = strsep(&buf," ")) != NULL) {
         new[i++] = strdup(found);
         //printf("I = %d\n",i );
       }
       printf("SOU O I= %d\n", i);
       for(int j=0; j<3; j++)
         printf("%d - %s , ", j, new[j]);
       puts("");

       int pid = atoi(new[0]);
       sprintf(myfifo2,"%d",pid);
       server_to_client=open(myfifo2,O_WRONLY);
       // decidir
       if(i==2) {
         //snprintf(eu, 18, "%05d %010d\n", atoi(new[0]), atoi(new[1]));
         printf("DECIDI I = 2\n");
         int returned[2];
         int res = getPreco_Stock_Of(atoi(new[1]),returned);
         //printf("WRITING to cl: %s\n",eu );
         sprintf(sender,"%d %.2f\n",returned[1],((float) returned[0])/100);
         if(res!=(-1)) {
           write(server_to_client,sender,strlen(sender));
         }
         else {
           write(server_to_client,"\n",1);
         }
       }
       else if (i==3) {
        // snprintf(eu, 29, "%05d %010d %010d\n", atoi(new[0]), atoi(new[1]), atoi(new[2]));
         //printf("WRITING to cl: %s\n",eu );
         printf("DECIDI I = 3\n");
         int stock = updateStock_Of(atoi(new[1]),atoi(new[2]));
         sprintf(sender,"%d\n",stock);
         if(stock!=(-1)){
           write(server_to_client,sender,strlen(sender));
         }
         else {
           write(server_to_client,"\n",1);
         }
       }

       close(server_to_client);
     }
     free(found);
     free(buf);
     free(new);
   }

   close(client_to_server);
   unlink(myfifo);
   unlink(myfifo2);
//unlink(myfifo);
//unlink(myfifo2);

  /*
  mkfifo("server", 0666);
  int server = open("server",O_RDONLY,0666);
  if(!fork()) {
    char buffer[1024];
    int n = 1;
    int flag = 0;
    char space = ' ';
    while(n > 0 || 1) {
       n = myreadln(server,buffer,1024);
       if(n>0) {
         for(int i = 0; i<n; i++) {
           if(buffer[i]==space) {
             flag = 1;
             updateStock_Of(atoi(buffer),atoi(buffer+i));
             break;
           }
         }
         if (!flag) {
           getPreco_Stock_Of(atoi(buffer));
         }
       }
       flag = 0;
     }
  }
  else {
    wait(NULL);
    close(server);
  }
  */
}

/*
void teste(){
  int n = 1;
  char buffer[1024];
  int fd = open("clSend",O_RDONLY, 0666);
  while(n>0) {
     n = myreadln(fd, buffer, 1024);
     write(1,buffer,n);
  }
}
*/
int main() {
  startServer();
  /*
  char returned[100];
  char str[100] = "1234 6 3\n";
  char **strings = malloc(sizeof(char*)*3);
  decideInstruction(str,returned,strings);
  printf("%s\n",strings[0]);
  */


  return EXIT_SUCCESS;
}
