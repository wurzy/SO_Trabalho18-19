CC = gcc
CFLAGS = -Wall -g

##############   ma   ##################
OBJSMA = $(patsubst %.c,%.o, ma.c readline.c qsort.c compactador.c)
MA = ma
########################################

SV = Servidor.c
CL = Cliente.c

$(MA): $(OBJSMA)
	$(CC) $(CFLAGS) $(OBJSMA) -o $(MA)

sv:
	$(CC) $(CFLAGS) $(SV) -o sv

cl:
	$(CC) $(CFLAGS) $(CL) -o cl

cat:
	cat artigos && cat strings

delete:
	rm -rf artigos strings deprecated log.txt

clean:
	@echo "Cleaning..."
	@echo ""
	@cat .art/train
	@rm -rf *.o *.out ma sv cl
	@echo ""
	@echo "Done!"
