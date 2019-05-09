CC = gcc
CFLAGS = -Wall -lm -g

##############   ma   ##################
OBJSMA = $(patsubst %.c,%.o, ma.c readline.c qsort.c compactador.c)
MA = ma
########################################

SV = Utility.c Servidor.c
CL = Utility.c Cliente.c
AG = Utility.c Cliente.c

$(MA): $(OBJSMA)
	$(CC) $(CFLAGS) $(OBJSMA) -o $(MA)

sv:
	$(CC) $(CFLAGS) $(SV) -o sv

cv:
	$(CC) $(CFLAGS) $(CL) -o cv

ag:
	$(CC) $(CFLAGS) $(AG) -o ag

cat:
	cat artigos && cat strings

delete:
	rm -rf artigos strings deprecated log.txt

clean:
	@echo "Cleaning..."
	@echo ""
	@cat .art/train
	@rm -rf *.o *.out ma sv cv
	@echo ""
	@echo "Done!"
