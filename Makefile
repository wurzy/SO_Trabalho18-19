CC = gcc
CFLAGS = -Wall -g
MA = ma.c readline.c qsort.c
SV = Servidor.c
CL = Cliente.c

ma:
	$(CC) $(CFLAGS) $(MA) -o ma

sv:
	$(CC) $(CFLAGS) $(SV) -o sv

cl:
	$(CC) $(CFLAGS) $(CL) -o cl

cat:
	cat artigos && cat strings

delete:
	rm -rf artigos strings deprecated log.txt

clean:
	rm -rf *.out ma sv cl
