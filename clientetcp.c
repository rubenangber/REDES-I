/*
** Fichero: clientetcp.c
** Autores:
** Rubén Angoso Berrocal DNI 70958754M
** Óscar Hernández Hernández DNI 70918137Y
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
	struct sockaddr_in myADDR;				 // Direccion del cliente
	struct sockaddr_in serverADDR;		 // Direccion del servidor
	struct addrinfo hints, *res;			 // Descriptor del socket ??
	int socketcltpc, errcode, addrlen; // Socket del cliente, codigo de error, longitud de la direccion
	long tvar;												 // Variable para guardar el tiempo

	if (argc != 2)
	{
		fprintf(stderr, "Usage:  %s <remote host>\n", argv[0]);
		exit(1);
	}

	socketcltpc = socket(AF_INET, SOCK_STREAM, 0); // SOCK_STREAM porque es TCP
	if (socketcltpc == -1)
	{
		perror("Error al crear el socket");
		fprintf(stderr, "%s:unable to create socket TPC\n", argv[0]);
		exit(1);
	}
	// memorias a 0
	memset((char *)&myADDR, 0, sizeof(struct sockaddr_in));
	memset((char *)&serverADDR, 0, sizeof(struct sockaddr_in));

	// Rellenamos la estructura de la direccion del cliente
	serverADDR.sin_family = AF_INET;

	// Rellenamos la estructura de la direccion del servidor
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;

	// Obtenemos la direccion IP del servidor
	errcode = getaddrinfo(argv[1], NULL, &hints, &res);
	// Comprobamos que no haya habido error
	if (errcode != 0)
	{
		fprintf(stderr, "%s: No es posible resolver la IP de %s\n", argv[0], argv[1]);
		exit(1);
	}
	else
	{
		// Copiamos la direccion IP
		serverADDR.sin_addr.s_addr = ((struct sockaddr_in *)res->ai_addr)->sin_addr.s_addr;
	}
	freeaddrinfo(res); // Liberamos la memoria?? (no estoy seguro) @rubenangber dime que hacemos aqui porfis

	// Rellenamos el puerto del servidor
	serverADDR.sin_port = htons(PUERTO);

	// Conectamos con el servidor
	if (connect(socketcltpc, (const struct sockaddr *)&serverADDR, sizeof(struct sockaddr_in)) == -1)
	{
		perror(argv[0]);
		fprintf(stderr, "%s: unable to connect to remote\n", argv[0]);
		exit(1);
	}
	// Obtenemos el nombre del socket
	addrlen = sizeof(struct sockaddr_in);
	if (getsockname(socketcltpc, (struct sockaddr *)&myADDR, &addrlen) == -1)
	{
		perror(argv[0]);
		fprintf(stderr, "%s: unable to get socket name\n", argv[0]);
		exit(1);
	}
	// Obtenemos el tiempo
	time(&tvar);
	// Imprimimos la informacion
	printf("Local socket is: %s\n", inet_ntoa(myADDR.sin_addr));
	printf("Local port is: %d\n", ntohs(myADDR.sin_port));
	printf("Remote socket is: %s\n", inet_ntoa(serverADDR.sin_addr));
	printf("Remote port is: %d\n", ntohs(serverADDR.sin_port));
	printf("Time is: %s\n", ctime(&tvar));

	return 0;
}
