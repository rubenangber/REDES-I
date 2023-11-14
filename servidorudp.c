/*
** Fichero: clientetcp.c
** Autores:
** Rubén Angoso Berrocal. DNI 70958754M
** Óscar Hernández Hernández. DNI 70918137Y
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MAX 1024

int s;

void handler(int sigNum) {
    printf("Cerrando el socket y saliendo\n");
    close(s);
    exit(0);
}

int main(int argc, char const *argv[]) {
    struct sockaddr_in dirLocal;
    struct sockaddr_in dirRemota;
    char respuesta[MAX], solicitud[MAX];
    signal(SIGINT, handler);

    int s = socket(AF_INET, SOCK_DGRAM, 0);
    if(s == -1) {
        perror("Error al crear el socket");
        return 1;
    }

    bzero(&dirLocal, sizeof(dirLocal));
    bzero(&dirRemota, sizeof(dirRemota));

    dirLocal.sin_family = AF_INET;
    dirLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    dirLocal.sin_port = htons(2050);

    if(bind(s, (struct sockaddr *)&dirLocal, sizeof(struct sockaddr_in)) == -1) {
        perror("Error al hacer bind");
        close(s);
        return 1;
    }

    socklen_t tamDir = sizeof(struct sockaddr_in);

    while(1) {
        if(recvfrom(s, respuesta, (sizeof(char) * MAX), 0, (struct sockaddr *)&dirRemota, &tamDir) == -1) {
            perror("Error al recibir la respuesta");
            close(s);
            return 1;
        }
        printf("Respuesta: %s\n", respuesta);


        strcpy(solicitud, "OK");

        if(sendto(s, solicitud, (sizeof(char) * MAX), 0, (struct sockaddr *)&dirRemota, sizeof(dirRemota)) == -1) {
            perror("Error al enviar la solicitud");
            close(s);
            return 1;
        }
    }

    return 0;
}