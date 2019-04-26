CC = gcc
CFLAGS = -Wall -g
MA = ma.c readline.c qsort.c

ma:
	$(CC) $(CFLAGS) $(MA) -o ma

cat:
	cat artigos && cat strings

delete:
	rm -rf artigos strings deprecated

clean:
	rm -rf *.out ma
