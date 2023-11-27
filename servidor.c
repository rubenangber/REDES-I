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

#define MAX_LINE_LENGTH 256
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
const char *nombreArchivo = "preguntas.txt";
char *lineaAleatoria;
char *lineaCopia;
char *token;
char *pregunta;
char *numero;
int intentos;

void generarRespuesta(char *m, char *respuesta);
char *leerLineaAleatoria(const char *nombreArchivo);

int main(int argc, char const *argv[]) {
    srand(time(NULL));
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

    while(1) {
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
                    perror("Error al crear el proceso hijo");
                    return 1;
                break;
                case 0: 
                    close(sTCP);
                    while (1) {
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
            while(1) {
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
    if (strcmp(m, "HOLA\r\n") == 0 && flagHola == 0) {
        lineaAleatoria = leerLineaAleatoria(nombreArchivo);
        lineaCopia = strdup(lineaAleatoria);
        token = strtok(lineaCopia, "|");
        pregunta = token;
        token = strtok(NULL, "|");
        numero = token;
        intentos = rand() % 45 + 5; //Entre 5 y 50
        sprintf(respuesta, "250 %s#%d", pregunta, intentos);
        flagHola = 1;
        return;
    } else if (flagHola == 0) {
        strcpy(respuesta, "500 Error de sintaxis");
        return;
    } else if (strcmp(m, "ADIOS\r\n") == 0 && flagHola == 1) {
        flagHola = 0;
        strcpy(respuesta, "221 Cerrando el servicio");
        return;
    } else if (strcmp(m, "+\r\n") == 0 && flagHola == 1) {
        lineaAleatoria = leerLineaAleatoria(nombreArchivo);
        lineaAleatoria = leerLineaAleatoria(nombreArchivo);
        lineaCopia = strdup(lineaAleatoria);
        token = strtok(lineaCopia, "|");
        pregunta = token;
        token = strtok(NULL, "|");
        numero = token;
        intentos = rand() % 45 + 5;
        sprintf(respuesta, "250 %s#%d", pregunta, intentos);
        return;
    }
    char *campo = strtok(m, "\r\n");
    if ((strncmp(campo, "RESPUESTA ", 10) == 0) && flagHola == 1) {
        campo = strtok(campo, " ");
        campo = strtok(NULL, " ");
        int num = atoi(campo);
        if (num < atoi(numero) && intentos > 0) {
            intentos--;
            sprintf(respuesta, "354 MAYOR#%d", intentos);
            //strcpy(respuesta, "354 MAYOR#");
            return;
        } else if (num > atoi(numero) && intentos > 0) {
            //strcpy(respuesta, "354 MENOR#");
            intentos--;
            sprintf(respuesta, "354 MENOR#%d", intentos);
            return;
        } else if (num == atoi(numero) && intentos > 0) {
            strcpy(respuesta, "350 ACIERTO");
            return;
        }
    } else {
        strcpy(respuesta, "500 Error de sintaxis");
        return;
    }
    strcpy(respuesta, "500 Error de sintaxis");
    return;
}

char *leerLineaAleatoria(const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    // Contar las líneas en el archivo
    int numLineas = 0;
    char c;
    while ((c = fgetc(archivo)) != EOF) {
        if (c == '\n') {
            numLineas++;
        }
    }

    // Generar un número aleatorio entre 1 y el número de líneas
    int numAleatorio = rand() % numLineas + 1;

    // Volver al principio del archivo
    rewind(archivo);

    // Leer la línea aleatoria
    char *linea = NULL;
    size_t len = 0;
    ssize_t read;
    int lineaActual = 0;

    while ((read = getline(&linea, &len, archivo)) != -1) {
        lineaActual++;
        if (lineaActual == numAleatorio) {
            // Quitar el salto de línea al final de la línea
            linea[strcspn(linea, "\n")] = '\0';
            break;
        }
    }

    fclose(archivo);
    return linea;
}