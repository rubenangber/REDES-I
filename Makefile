CC = gcc

all: cliente servidor

cliente: cliente.c
	$(CC) -o cliente cliente.c

servidor: servidor.c
	$(CC) -o servidor servidor.c

clean:
	rm -f cliente servidor
