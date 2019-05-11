CC = gcc
CFLAGS = -Wall -g

OBJSMA = $(patsubst %.c,%.o, ma.c readline.c qsort.c compactador.c)
MA = ma

OBJSSV = $(patsubst %.c,%.o, Cache.c Utility.c Servidor.c)
SV = sv

OBJSCL = $(patsubst %.c,%.o, Utility.c Cliente.c)
CL = cv

OBJSAG = $(patsubst %.c,%.o, Utility.c Agregador.c)
AG = ag

ALL = $(patsubst %.c,%.o,$(wildcard *.c))

all:$(ALL)
	$(CC) $(CFLAGS) $(OBJSMA) -o $(MA)
	$(CC) $(CFLAGS) $(OBJSSV) -o $(SV)
	$(CC) $(CFLAGS) $(OBJSCL) -o $(CL)
	$(CC) $(CFLAGS) $(OBJSAG) -o $(AG) -lm

$(MA): $(OBJSMA)
	$(CC) $(CFLAGS) $(OBJSMA) -o $(MA)

$(SV): $(OBJSSV)
	$(CC) $(CFLAGS) $(OBJSSV) -o $(SV)

$(CL): $(OBJSCL)
	$(CC) $(CFLAGS) $(OBJSCL) -o $(CL)

$(AG): $(OBJSAG)
	$(CC) $(CFLAGS) $(OBJSAG) -o $(AG) -lm

cat:
	cat artigos strings

delete:
	rm -rf artigos strings deprecated log.txt

clean:
	@echo "Cleaning..."
	@echo ""
	@cat .art/train
	@rm -rf *.o *.out ma sv cv
	@echo ""
	@echo "Done!"
