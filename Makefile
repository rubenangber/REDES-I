CC = gcc

all: clienteudp servidorudp
#all: clienteudp clientetcp servidor

clienteudp: clienteudp.c
	$(CC) -o clienteudp clienteudp.c

#clientetcp: clientetcp.c
#	$(CC) -o clientetcp clientetcp.c

#servidor: servidor.c
#	$(CC) -o servidor servidor.c

servidorudp: servidorudp.c
	$(CC) -o servidorudp servidorudp.c

clean:
	rm -f clienteudp servidorudp
#	rm -f clienteudp clientetcp servidor
