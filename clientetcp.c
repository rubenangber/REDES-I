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
    printf("Cerrando los socket y saliendo\n");
    close(s);
    exit(0);
}

int main(int argc, char const *argv[]) {
    struct sockaddr_in dirLocal;
    struct addrinfo hints, * resultados;
    char solicitud[MAX], respuesta[MAX];
    signal(SIGINT, handler);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if(s == -1) {
        perror("Error al crear el socket");
        return 1;
    }

    bzero(&dirLocal, sizeof(dirLocal));

    dirLocal.sin_family = AF_INET;
    dirLocal.sin_addr.s_addr = htonl(INADDR_ANY);
    dirLocal.sin_port = htons(2050);

    if(connect(s, (struct sockaddr *)&dirLocal, sizeof(dirLocal)) == -1) {
        perror("Error al conectar");
        close(s);
        return 1;
    }

    while(1) {
        printf("Introduce una solicitud: ");
        fgets(solicitud, MAX, stdin);
        solicitud[strcspn(solicitud, "\n")] = '\0';
        strcat(solicitud, "\r\n");

        if(send(s, solicitud, sizeof(char) * MAX, 0) == -1) {
            perror("Error al enviar la solicitud");
            close(s);
            return 1;
        }

        for (int i = 0; i < MAX; i++) {
            respuesta[i] = '\0';
        }
        if(recv(s, respuesta, sizeof(char) * MAX, 0) == -1) {
            perror("Error al recibir la respuesta");
            close(s);
            return 1;
        }

        printf("Respuesta: %s\n", respuesta);
        if(strcmp(respuesta, "221 Cerrando el servicio") == 0 || strcmp(respuesta, "350 ACIERTO") == 0) {
            return 0;
        }
    }

    return 0;
}
