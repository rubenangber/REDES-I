CC = gcc

all: clienteudp servidorudp

clienteudp: clienteudp.c
	$(CC) -o clienteudp clienteudp.c

servidorudp: servidorudp.c
	$(CC) -o servidorudp servidorudp.c

clean:
	rm -f clienteudp servidorudp
