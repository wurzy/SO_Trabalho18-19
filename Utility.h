#ifndef UTILITY_H_
#define UTILITY_H_

void createFifo(char *name);
void printOut(char *string);
void timeString(char buffer[]);
ssize_t myreadln(int fildes, void *buff, ssize_t nbytes);

#endif
