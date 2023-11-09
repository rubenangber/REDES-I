/*
** Fichero: clientetcp.c
** Autores:
** Rubén Angoso Berrocal. DNI 70958754M
** Óscar Hernández Hernández. DNI 70918137Y
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

#define PUERTO 17278

int main(int argc, char const *argv[])
{
	struct sockaddr_in myADDR;		 // Direccion del cliente
	struct sockaddr_in serverADDR; // Direccion del servidor
	int s, errcode;								 // Descriptor del socket
	struct addrinfo hints, *res;

	if (argc != 2)
	{
		fprintf(stderr, "Usage:  %s <remote host>\n", argv[0]);
		exit(1);
	}

	int s = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM porque es TCP
	if (s == -1)
	{
		perror("Error al crear el socket");
		fprintf(stderr, "%s:unable to create socket TPC\n", argv[0]);
		exit(1);
	}

	memset((char *)&myADDR, 0, sizeof(struct sockaddr_in));
	memset((char *)&serverADDR, 0, sizeof(struct sockaddr_in));

	serverADDR.sin_family = AF_INET;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;

	errcode = getaddrinfo(argv[1], NULL, &hints, &res);
	if (errcode != 0)
	{
		fprintf(stderr, "%s: No es posible resolver la IP de %s\n", argv[0], argv[1]);
		exit(1);
	}
	else
	{
		serverADDR.sin_addr.s_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr.s_addr;
	}
	freeaddrinfo(res);

	servaddr_in.sin_port = htons(PUERTO);

	if (connect(s, (const struct sockaddr *)&servaddr_in, sizeof(struct sockaddr_in)) == -1)
	{
		perror(argv[0]);
		fprintf(stderr, "%s: unable to connect to remote\n", argv[0]);
		exit(1);
	}

	return 0;
}
