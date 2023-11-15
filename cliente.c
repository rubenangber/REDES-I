/*
** Fichero: cliente.c
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

void tcp(char **, int);
void udp(char **, int);
int main(int argc, char *argv[])
{
	if (argc >= 3)
	{
		if (strcmp(argv[2], "TCP") == 0)
		{
			tcp(argv, argc);
			end();
		}
		else if (strcmp(argv[2], "UDP") == 0)
		{
			udp(argv, argc);
			end();
		}
		else
		{
			printf("Error en el protocolo\n");
			end();
		}
	}
	return 0;
}
