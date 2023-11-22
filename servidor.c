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

int sTCP, sUDP, sNuevo;
void handler(int sigNum) {
    printf("Cerrando los sockets y saliendo\n");
    close(sUDP);
    close(sTCP);
    close(sNuevo);
    return;
}
int flagHola = 0;
int i = 1;

void generarRespuesta(char *m, char *respuesta);

int main(int argc, char const *argv[]) {
    fd_set readmusk;
    int s_mayor;
    struct sockaddr_in servaddr, clientnaddr;
    socklen_t tamDir = sizeof(struct sockaddr_in);
    char respuesta[MAX], solicitud[MAX];

    //Dar valor al servaddr
    bzero(&servaddr, sizeof(servaddr));
    bzero(&clientnaddr, sizeof(clientnaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(2050);

    //Crear socket sTCP --> socket
    sTCP = socket(AF_INET, SOCK_STREAM, 0);
    if(sTCP == -1) {
        perror("Error al crear el socket TCP");
        return 1;
    }

    //Asociar sTCP a la dirLocal --> bind
    if(bind(sTCP, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) == -1) {
        perror("Error al hacer bind");
        close(sTCP);
        return 1;
    }

    //Poner el sTCP en modo escucha --> listen
    if(listen(sTCP, 5) == -1) {
        perror("Error al poner el socket TCP en modo escucha");
        close(sTCP);
        return 1;
    }

    //Crear socket sUDP --> socket
    sUDP = socket(AF_INET, SOCK_DGRAM, 0);
    if(sUDP == -1) {
        perror("Error al crear el socket UDP");
        return 1;
    }
    //Asociar sUDP a la dirLocal --> bind
    if(bind(sUDP, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in)) == -1) {
        perror("Error al hacer bind");
        close(sTCP);
        close(sUDP);
        return 1;
    }
    //Configurar señales
    signal(SIGINT, handler);

    s_mayor = sTCP;
    if(sUDP > s_mayor) {
        s_mayor = sUDP;
    }

    while(i) {
        FD_ZERO(&readmusk);
        FD_SET(sTCP, &readmusk);
        FD_SET(sUDP, &readmusk);
        if(select(s_mayor + 1, &readmusk, NULL, NULL, NULL) == -1) {
            //Error
            return 1;
        }
        if(FD_ISSET(sTCP, &readmusk)) { //TCP
            sNuevo = accept(sTCP,(struct sockaddr *) &clientnaddr, &tamDir);
            switch(fork()) {
                case -1: 
                    //Error
                    return 1;
                break;
                case 0: 
                    close(sTCP);
                    while (i) {
                        if(recv(sNuevo, respuesta, MAX, 0) == -1) {
                            perror("Error al recibir la respuesta");
                            close(sNuevo);
                            return 1;
                        }
                        printf("Respuesta: %s", respuesta);

                        generarRespuesta(respuesta, solicitud);

                        if(send(sNuevo, solicitud, strlen(solicitud), 0) == -1) {
                            perror("Error al enviar la respuesta");
                            close(sNuevo);
                            return 1;
                        }
                    }
                break;
                default: 
                    close(sNuevo);
            }
        }
        //UDP
        if(FD_ISSET(sUDP, &readmusk)) {
            while(i) {
                if(recvfrom(sUDP, respuesta, (sizeof(char) * MAX), 0, (struct sockaddr *)&clientnaddr, &tamDir) == -1) {
                    perror("Error al recibir la respuesta");
                    close(sUDP);
                    return 1;
                }
                printf("Respuesta: %s", respuesta);

                generarRespuesta(respuesta, solicitud);

                if(sendto(sUDP, solicitud, (sizeof(char) * MAX), 0, (struct sockaddr *)&clientnaddr, sizeof(clientnaddr)) == -1) {
                    perror("Error al enviar la solicitud");
                    close(sUDP);
                    return 1;
                }
            }
        }
    }
    return 0;
}

void generarRespuesta(char *m, char *respuesta) {
    if (strcmp(m, "HOLA\r\n") == 0) {
        strcpy(respuesta, "250 pregunta#");
        flagHola = 1;
        return;
    }
    if (flagHola == 0) {
        strcpy(respuesta, "500 Error de sintaxis");
        return;
    }

    if (strcmp(m, "ADIOS\r\n") == 0 && flagHola == 1) {
        strcpy(respuesta, "221 Cerrando el servicio");
        i = 0;
        return;
    }
    if (strcmp(m, "+\r\n") == 0 && flagHola == 1) {
        strcpy(respuesta, "250 pregunta2#");
        return;
    }
    char *campo = strtok(m, "\r\n");
    if (strncmp(campo, "RESPUESTA ", 10 && flagHola == 1) == 0) {
        campo = strtok(campo, " ");
        campo = strtok(NULL, " ");
        int num = atoi(campo);
        if (num < 50) {
            strcpy(respuesta, "354 MAYOR#");
            return;
        } else if (num > 50) {
            strcpy(respuesta, "354 MENOR#");
            return;
        } else if (num == 50) {
            strcpy(respuesta, "350 ACIERTO");
            i = 0;
            return;
        }
    }
    strcpy(respuesta, "500 Error de sintaxis");
    return;
}