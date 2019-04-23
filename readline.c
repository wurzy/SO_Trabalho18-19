#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

ssize_t readln(int fildes, void* buf, ssize_t nbyte){
  ssize_t size = 0;
  int res;
  char byte;
  char *buffer = (char*)buf; // casting void*
  while ((res = read(fildes,&byte,1) > 0) && size < nbyte){
    if (byte == '\0')
      return size;
    buffer[size++] = byte;
    if (byte == '\n')
      return size;
  }
  return size;
}
