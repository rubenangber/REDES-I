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
    if (argc != 2) {
        printf("Uso: ./cliente TCP/UDP\n");
        return 1;
    } else if (strcmp(argv[1], "TCP") == 0) {
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
            if(strcmp(respuesta, "221 Cerrando el servicio\r\n") == 0) {
                return 0;
            }
        }
    } else if (strcmp(argv[1], "UDP") == 0) {
        struct sockaddr_in dirLocal;
        struct sockaddr_in dirRemota;
        struct addrinfo hints, * resultados;
        char solicitud[MAX], respuesta[MAX];
        signal(SIGINT, handler);

        s = socket(AF_INET, SOCK_DGRAM, 0);
        if(s == -1) {
            perror("Error al crear el socket");
            return 1;
        }

        bzero(&dirLocal, sizeof(dirLocal));
        bzero(&dirRemota, sizeof(dirRemota));

        dirLocal.sin_family = AF_INET;
        dirLocal.sin_addr.s_addr = htonl(INADDR_ANY);
        dirLocal.sin_port = htons(0);

        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;

        char hostname[1024];
        gethostname(hostname, sizeof(hostname));

        if(getaddrinfo(hostname, NULL, &hints, &resultados) != 0) {
            perror("Error al obtener la direccion");
            close(s);
            return 1;
        }

        dirRemota.sin_family = AF_INET;
        dirRemota.sin_addr = ((struct sockaddr_in *) resultados->ai_addr)->sin_addr;
        dirRemota.sin_port = htons(2050);

        freeaddrinfo(resultados);

        if(bind(s, (struct sockaddr *)&dirLocal, sizeof(struct sockaddr_in)) == -1) {
            perror("Error al hacer bind");
            close(s);
            return 1;
        }

        socklen_t tamDir = sizeof(struct sockaddr_in);

        while(1) {
            printf("Introduce una solicitud: ");
            fgets(solicitud, MAX, stdin);
            solicitud[strcspn(solicitud, "\n")] = '\0';
            strcat(solicitud, "\r\n");

            if(sendto(s, solicitud, sizeof(char) * MAX, 0, (struct sockaddr *)&dirRemota, sizeof(dirRemota)) == -1) {
                perror("Error al enviar la solicitud");
                close(s);
                return 1;
            }

             if(recvfrom(s, respuesta, (sizeof(char) * MAX), 0, (struct sockaddr *)&dirRemota, &tamDir) == -1) {
                perror("Error al recibir la respuesta");
                close(s);
                return 1;
            }

            printf("Respuesta recibida: %s\n", respuesta);
            if(strcmp(respuesta, "221 Cerrando el servicio\r\n") == 0) {
                return 0;
            }
        }
    }
}
