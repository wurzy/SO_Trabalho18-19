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

Cache cache;

void cache_handler(int signum) {
  int id;
  int fd = open("artigos",O_RDONLY);
  off_t offset;
  char precos[12];
  int preco;
  for(int i = 0; i<cache->ocupados; i++) {
    id = cache->cached[i]->id;
    offset = id*29+19;
    lseek(fd,offset,SEEK_SET);
    read(fd,precos,12);
    preco = atoi(precos);
    cache->cached[i]->preco = preco;
  }
  close(fd);
}

void agr_handler(int signum) {

  int fd = open("vendas",O_RDWR,0666);
  int to_ag = open("para_agregar", O_CREAT | O_TRUNC | O_RDWR, 0666);
  char buf[12];
  char buf2[1024];
  int fds[2];
  pipe(fds);
  int n = read(fd,buf,12);
  off_t offset = atol(strndup(buf,11));
  lseek(fd,0,SEEK_SET);
  lseek(fd,offset + 12,SEEK_SET);
  while((n = read(fd,buf2,1024)) > 0) {
    write(to_ag,buf2,n);
  }
  off_t vendasNovo = lseek(fd,0,SEEK_END) -12;
  lseek(fd,0,SEEK_SET);
  snprintf(buf2,12,"%011ld",vendasNovo);
  write(fd,buf2,11);
  if (!fork()){
    char time[1024];
    timeString(time);
    int data = open(time, O_CREAT | O_WRONLY, 0666);
    dup2(data, STDOUT_FILENO);
    close(data);
    execlp("./agregador.sh", "agregador.sh", NULL);
    _exit(0);
  }
  else{
    wait(NULL);
  }
  close(to_ag);
  unlink("para_agregar");
  close(fd);
}

static int verificaID(int id){
  int fd = open("stocks",O_RDONLY, 0666);

  off_t max = lseek(fd,0,SEEK_END);
  double atual = ((double) max) / (STK_LEN_TOT);
  int at = (int) atual;

  close(fd);

  return (id>=at?0:1);
}

int getPreco_Stock_Of(int id, int returned[]) {
  if(!verificaID(id)) {
    return -1;
  }
  else {
    int fd = open("stocks",O_RDONLY, 0666);
    char stock[STK_LEN + 1];


    char preco[PRICE_LEN_I + 1];
    int fd2 = open("artigos",O_RDONLY,0666);

    off_t offsetStock = id*(STK_LEN_TOT);
    off_t offsetPreco = id*(ARTIGO_LENG+1) + 2 + NUMBER_LEN_I + POINTER_LEN_I;

    lseek(fd2,offsetPreco,SEEK_SET);
    read(fd2,&preco,PRICE_LEN_I);

    returned[0] = atof(preco)*100;

    lseek(fd,offsetStock,SEEK_SET);
    read(fd,&stock,STK_LEN);

    returned[1]=atoi(stock);

    close(fd);
    close(fd2);
  }
  return 0;
}

int is_Overflow(int x, int y){
  if ((y > 0 && x > MAX_INT_NUM - y) || (y < 0 && x < MIN_INT_NUM - y)){
    return 1;
  }
  else{
    return 0;
  }
}

void manageVendas(int id, int quantidade){

  int fd = open("vendas",O_RDWR | O_APPEND,0666);
  int modqtd = abs(quantidade);

    char all[64];
    char preco[PRICE_LEN_I + 1];
    int fetched = fetchPreco(cache,id);
    int montante;
    if(fetched==-1) {
      int fd2 = open("artigos", O_RDONLY, 0666);
      off_t offsetPreco = id*(ARTIGO_LENG+1) + 2 + NUMBER_LEN_I + POINTER_LEN_I;
      lseek(fd2,offsetPreco,SEEK_SET);
      read(fd2,preco,PRICE_LEN_I);
      montante = atoi(preco)*modqtd;
      close(fd2);
    }
    else {
      montante = fetched*modqtd;
    }
    cache = addToCache(cache, id);
    sprintf(all,"%010ld %011ld %011ld\n",(long int) id, (long int) modqtd, (long int) montante);
    write(fd,all,strlen(all));
  close(fd);
}

int updateStock_Of(int id, int quantidade) {
  if(!verificaID(id)){
    return (-1);
  }
  else{
    int fdven = open("vendas",O_CREAT | O_RDWR,0666);
    off_t seeker = lseek(fdven,0,SEEK_END);


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
    if((stockfrom + quantidade) < 0) {
      return -1;
    }
    close(fdven);
    if(quantidade<0) {
      manageVendas(id,quantidade);
    }
    int stock = stockfrom + quantidade;
    sprintf(ints, STK_SIZE , (long int) stock);

    lseek(fd,offsetStock,SEEK_SET);
    write(fd,ints,strlen(ints));

    close(fd);
    return stock;
  }
}

void startServer(){
  char sender[100];
  int client_to_server, n;
   const char *myfifo = "server";

   int server_to_client;
   char myfifo2[100];


   mkfifo(myfifo, 0666);

   client_to_server = open(myfifo, O_RDWR);


   while(1) {
     char* buf = (char*)malloc(sizeof(char*) * 1024);
     char* found;
     int i = 0;
     char** new = (char**)malloc(sizeof(char*) * 3);

     if((n = read(client_to_server,buf,28)) > 0) {


       while((found = strsep(&buf," ")) != NULL) {
         new[i++] = strdup(found);
       }
       int pid = atoi(new[0]);
       sprintf(myfifo2,"%d",pid);
       server_to_client=open(myfifo2,O_WRONLY);

       if(i==2) {
         int returned[2];
         int res = getPreco_Stock_Of(atoi(new[1]),returned);
         sprintf(sender,"%d %.2f\n",returned[1],((float) returned[0])/100);
         if(res!=(-1)) {
           write(server_to_client,sender,strlen(sender));
         }
         else {
           write(server_to_client,"\n",1);
         }
       }
       else if (i==3) {

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
}

void createPid(){
  int pid = getpid();
  char buf[10];
  int fd = open("pid_sv",O_CREAT | O_WRONLY | O_TRUNC, 0666);
  sprintf(buf,"%d\n",pid);
  write(fd,buf,strlen(buf));
  close(fd);
}

int main() {
  createPid();
  signal(SIGUSR1,agr_handler);
  signal(SIGUSR2,cache_handler);
  cache=initCache(cache);
  startServer();
  freeCache(cache);
  return EXIT_SUCCESS;
}
