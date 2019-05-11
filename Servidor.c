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
  /* printf("buf: %.11s\n", buf); */
  off_t offset = atol(strndup(buf,11));
  lseek(fd,0,SEEK_SET);
  /* printf("OFFSET %ld\n",offset ); */
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

/*
void agr_handler(int signum) {
  int status;
  int fd = open("vendas",O_RDWR,0666);
  int to_ag = open("para_agregar",O_CREAT | O_TRUNC | O_RDWR,0666);
  char buf[11];
  char buf2[1024];
  int fds[2];
  pipe(fds);
  int n = read(fd,buf,11);
  printf("buf: %.11s\n", buf);
  off_t offset = atol(strndup(buf,10));
  lseek(fd,0,SEEK_SET);
  printf("OFFSET %ld\n",offset );
  lseek(fd,offset + 12,SEEK_SET);
  while((n=read(fd,buf2,1024))>0) {
    write(to_ag,buf2,n);
  }
  off_t vendasNovo = lseek(fd,0,SEEK_END) -12;
  lseek(fd,0,SEEK_SET);
  snprintf(buf2,12,"%011ld",vendasNovo);
  write(fd,buf2,11);
  if (!fork())  {
   close(STDOUT_FILENO);
   dup(fds[1]);
   close(fds[0]);
   close(fds[1]);
   execlp("cat", "cat", "para_agregar", NULL);
   perror("cat");
  }
  if(!fork()) {
      close(STDIN_FILENO); //close write to pipe, in child
      dup(fds[0]); // Replace stdin with the read end of the pipe
      close(fds[1]); // Don't need another copy of the pipe read end hanging about
      close(fds[0]); // Don't need another copy of the pipe read end hanging about
      execlp("./ag", "./ag", NULL);
      perror("ag");
    }
    //unlink("para_agregar");
    //_exit(0);
  else {
    close(fds[0]);
    close(fds[1]);
    wait(&status);
    wait(&status);
  }
  unlink("para_agregar");

}
*/
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

  int fd = open("vendas",O_RDWR | O_APPEND,0666);
  //off_t seeker = lseek(fd,0,SEEK_END);
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
    char preco[PRICE_LEN_I + 1];
    int fetched = fetchPreco(cache,id);
    int montante;
    if(fetched==-1) {
      printf("aceitei fetched = -1\n" );
      int fd2 = open("artigos", O_RDONLY, 0666);
      off_t offsetPreco = id*(ARTIGO_LENG+1) + 2 + NUMBER_LEN_I + POINTER_LEN_I;
      lseek(fd2,offsetPreco,SEEK_SET);
      read(fd2,preco,PRICE_LEN_I);
      montante = atoi(preco)*modqtd;
      close(fd2);
    }
    else {
      printf("aceitei else fetched\n" );
      //sprintf(preco,"%d",fetched);
      montante = fetched*modqtd;
    }
    cache = addToCache(cache, id);
    printCache(cache);
    //char id[NUMBER_LEN_I + 1];
    //char quantidade[STK_LEN + 1]
    sprintf(all,"%010ld %011ld %011ld\n",(long int) id, (long int) modqtd, (long int) montante);
    //lseek(fd,seeker,SEEK_SET);
    write(fd,all,strlen(all));
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
    if((stockfrom + quantidade) < 0) {
      printf("N quero atualizar\n" );
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
     if((n = read(client_to_server,buf,28)) > 0) {
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
           printf("WRITING: %s\n",sender);
           write(server_to_client,sender,strlen(sender));
           //write(server_to_client,"\n",1);
         }
         else {
           //write(server_to_client,"\n",1);
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
           printf("WRITING: %s\n",sender);
          // write(server_to_client,"\n",1);
         }
         else {
           //write(server_to_client,"\n",1);
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
  /*
  char returned[100];
  char str[100] = "1234 6 3\n";
  char **strings = malloc(sizeof(char*)*3);
  decideInstruction(str,returned,strings);
  printf("%s\n",strings[0]);
  */


  return EXIT_SUCCESS;
}
