#ifndef READLINE_H
#define READLINE_H

#include <unistd.h>

ssize_t readln(int fildes, void* buf, ssize_t nbyte);

#endif
