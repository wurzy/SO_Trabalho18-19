#include <sys/types.h>
#include <sys/stat.h>

void createFifo(char *name) {
    if(mkfifo(name,0666)==-1) {
      puts("Pipe error");
    }
}
