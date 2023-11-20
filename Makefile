CC = gcc

all: clienteudp clientetcp servidor

clienteudp: clienteudp.c
	$(CC) -o clienteudp clienteudp.c

clientetcp: clientetcp.c
	$(CC) -o clientetcp clientetcp.c

servidor: servidor.c
	$(CC) -o servidor servidor.c

clean:
	rm -f clienteudp clientetcp servidor
