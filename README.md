# SOCKETS

struct sockaddr --> struct sockaddr_in<br>
genérica --> IPv4
- sin_family = AF_INET;
- sin_port = htons(número de puerto); --> si se pone 0, estamos hablando de un puerto efímero
- sin_addr -> sin_addr.s_addr = INADDR_ANY; --> significa que es una dirección local
- sin_addr -> otras funciones

```
struct sockaddr_in dir;
dir.sin_addr.s_addr = INADDR_ANY;
```

### Limpiar la estructura cada vez que se declara
#### void * memset(void *ptr, int c, size_t n)
- ptr -> direccion que quiero leer (&dir)
- c -> caracter que quiero escribir (0)
- size_t -> numero de bytes que quiero escribir (sizeof(struct sockaddr_in))

#### void bzero(void *ptr, size_t n)
Hace lo mismo solo que no se especifica que caracter hay que poner ya que lo pone a "0" automaticamente

#### int socket(int familia, int tipo, int protocolo)
Crea el fichero especial<br>
Si se retorna -1 es un error (HAY QUE COMPROBARLO), si no, retorna el socket creado
- familia --> AF_INET
- tipo --> SOCK_STREAM --> TCP
- tipo --> SOCK_DGRAM --> UDP
- protocolo --> 0

```
int s = socket(AF_INET, SOCK_STREAM, 0);
if(s == -1) {
    perror("Error al crear el socket");
    return 1;
}
```

#### int bind(int s, struct sockaddr * dirLocal, socklen_t tamDirLocal)
Asocia un socket a una dirección local, esta asociando un socket a un numero de puerto local que será el que se use para emitir<br>
Si se retorna -1 es un error (HAY QUE COMPROBARLO)
- s --> socket que quiero asociar (s)
- dirLocal --> (&dirLocal)
- socklen_t --> (sizeof(struct sockaddr_in))

```
if(bind(s, &dirLocal, sizeof(struct sockaddr_in)) == -1) {
    //error
}
```

#### int connect(int s, struct sockaddr_in * dirRemota, socklen_t * tamDirRemota)
Realiza una conexión TCP de un cliente a un servidor<br>
Si se retorna -1 es un error (HAY QUE COMPROBARLO)
- s --> socket que quiero que establezca la conexion (s)
- dirRemota --> direccion del servidor (&dirRemota)
- tamDirRemota --> (sizeof(struct sockaddr_in))

```
if(connect(s, &dirLocal, sizeof(struct sockaddr_in)) == -1) {
    //error
}
```

#### ssize_t send(int s, void * datos, size_t tamDatos, int flags)
Envia en TCP<br>
Retorna el valor del tamaño de los datos que haya podido enviar <= tamDatos
- s --> socket (s)
- datos --> datos a enviar (&datos)
- tamDatos --> numeros de bytes de los datos a enviar (sizeof(char) * numero de caracteres)
- flags --> (0)

#### ssize_t recv(int s, void * datos, size_t tamDatos, int flags)
Recibe en TCP<br>
Retorna el valor del tamaño de los datos que haya podido recibir <= tamDatos, puede recibir 0 si se corta la conexión TCP
- s --> socket (s)
- datos --> datos a enviar (&datos)
- tamDatos --> numeros de bytes de los datos a enviar (sizeof(char) * numero de caracteres)
- flags --> (0)

#### int close(int s)
Cierra un fichero<br>
No importa el valor de retorno
- s --> socket que hemos querido cerrar

#### ssize_t sendto(int s, void * datos, size_t tamDatos, int flags, struct sockaddr * dirRemota, socklen_t tamDir)
Envía en UDP<br>
- s --> socket (s)
- datos --> datos a enviar (&datos)
- tamDatos --> numeros de bytes de los datos a enviar (sizeof(char) * numero de caracteres)
- flags --> (0)
- dirRemota --> ((struct sockaddr *)&dirRemota) hacer conversion ya que dirRemota es sockaddr_in
- tamDir --> (sizeof(struct sockaddr_in))

#### ssize_t recvfrom(int s, void * datos, size_t tamDatos, int flags, struct sockaddr * dirRemota, socklen_t * tamDir)
Recibe en UDP<br>
- s --> socket (s)
- datos --> datos a enviar (&datos)
- tamDatos --> numeros de bytes de los datos a enviar (sizeof(char) * numero de caracteres)
- flags --> (0)
- dirRemota --> ((struct sockaddr *)&dirRemota) hacer conversion ya que dirRemota es sockaddr_in
- tamDir --> socklen_t tamDir = sizeof(struct sockaddr_in) --> (&tamDir)
```
socklen_t tamDir = sizeof(struct sockaddr_in)
if(ssize_t sendto(s, datos, tamDatos, 0, &dirRemota, &tamDir) == -1) {
    //error
}
```

```
struct sockaddr_in direccion;
direccion.sin_addr.s_addr = INADDR_ANY; //Direccion local
REMOTA
A traves de una IP decimal punto --> inet_aton
A traves de un nombre --> getaddrinfo
```

#### int inet_aton(char * cadenaDir, struct in_addr * campoDir)
Convierte el array de carracteres y lo convierte a net(internet)<br>
Retorna 0 si hay error
```
struct sockaddr_in dirRemota;
char cadena[] = "192.168.0.17";
if(! inet_aton(cadena, &dirRemota.sin_addr)){
    //error
}
```

#### char * inet_ntoa(struct in_addr campoDir)
A partir de una direccion IP, lo convierte en una cadena de caracteres de formato decimal punto
```
char * cad;
cad = inet_ntoa(dirRemota.sin_addr);
```

#### GETADDRINFO:
Obtiene direcciones con unos requesitos específicos<br>
```
struct sockaddr_in dirRemota;
struct addrinfo hints, * resultados;

memset(&hints, 0, sizeof(struct addrinfo));
hints.ai_family = AF_INET;

if(getaddrinfo(nombre, NULL, &hints, &resultados) != 0) {
    //error
}
dirRemota.sin_addr = ((struct sockaddr_in *) resultados->ai_addr)->sin_addr;

freeaddrinfo(resultados);
```
- nombre --> nombre dns (olivo, encina...)
- hints --> conjunto de requesitos
- resultados --> lista enlazada con todos los resultados que cumplen las condiciones

#### int listen(int s, int tamCola) --> lo usa el servidor TCP
Retorna -1 si hay error
- s --> socket
- tamCola --> mensajes que puede tener esperando (nines pone de ejemplo 5)

#### int accept(int s, struct sockaddr * dirCliente, socklen_t * tamDir) --> lo usa el servidor TCP
Es una llamada bloqueante que espera la solicitud del conexiones<br>
Retorna -1 si se produce un error, el socket de la conexion si todo ha ido bien
- s --> socket
- dirCliente --> guarda la dirrecion del cliente que se ha conectado
- tamDir --> guarda el tamaño de la direccion del cliente que se ha conectado
```
int s, sNuevo;
struct sockaddr_in dirCliente;
socklen_t tamDir = sizeof(struct sockaddr_in);
//...
sNuevo = accept(s, &dirCliente, &tamDir);
```

### CLIENTE UDP
1) Crear el socket
2) Configurar una dirLocal para el puerto efímero
3) Configurar una dirRemota --> getaddrinfo
4) Asociar la dirLocal al puerto --> bind
Manejadora para SIGINT
5) Bucle
    - pedir solicitud al unirme
    - enviar --> sendto
    - recibir --> recvfrom
### SERVIDOR UDP
1) Crear socket
2) Configurar dirLocal
3) Asociar dirLocal al puerto --> bind
Manejadora
4) Bucle
    - recibir --> recvfrom
    - enviar --> sendto

#### int select(int mayorFD, fd_set * entrada, fd_set * salida, fd_set * error, struct timeval * timeout)
´´´
fg_set conjunto;
FD_ZERO(&conjunto);
FD_SET(descriptor, &conjunto);
if(FD_ISSET(descriptor, &conjunto)) {
    //descriptor está conjunto
}
´´´
Solo usaremos "entrada", "salida" y "error" les pasamos NULL<br>
struct timeval le pasaremos tambien NULL, asi estepera de forma indefinida<br>
mayorFD, mayor fd de conjuntos + 1<br>
Retorna -1 si hay error<br>
Como la llamada select modifica los conjuntos, es necesario recontruirlos cada vez que se llame

### SERVIDOR
```
int sTCP, sUDP, sNuevo;
fd_set readmusk;
int s_mayor;
struct sockaddr_in servaddr, clientnaddr;
socklent_t tamDir = sizeof(struct sockaddr_in);
//Dar valor al servaddr
//Crear socket sTCP --> socket
//Asociar sTCP a la dirLocal --> bind
//Poner el sTCP en modo escucha --> listen

//Crear socket sUDP --> socket
//Asociar sUDP a la dirLocal --> bind

//Configurar señales

s_mayor = sTCP;
if(sUDP > s_mayor) {
    s_mayor = sUDP;
}

for(;;) {
    FD_ZERO(&readmusk);
    FD_set(sTCP, &readmusk);
    FD_set(sUDP, &readmusk);
    if(select(s_mayor + 1, &readmusk, NULL, NULL, NULL) == -1) {
        //Error
    }
    if(FD_ISSET(sTCP, &readmusk)) {
        //Socket TCP activo
        sNuevo = accept(sTCP, &clientnaddr, &tamDir);
        switch(fork()) {
            case -1: 
                //Error 
            break;
            case 0: 
                close(sTCP); 
                //Recibir --> recv
                //Enviar --> send
            break;
            default: 
                close(sNuevo);
        }
    }
    if(FD_ISSET(sUDP, &readmusk)) {
        //Recibir --> recvfrom
        //Enviar --> sendto
    }
}
```

### Funciones para tarbajar con cadena de caracteres
strcpy(cad1, cad2); //Copia cad2 en cad1
strcat(cad1, cad2); //Añade cad2 al final de cad1
sprintf(cad1,"...",...) //Igual que printf pero en lugar de sacarlo por pantalla lo guarda en cad1
strcmp(cad1,cad2); == 0 //Si son iguales
´´´
char datos[] = "Felipe Lopez 22";
char * campo;
campo = strtok(datos, " "); //campo = "Felipe"
campo = strtok(NULL, " "); //campo = "Lopez"
campo = strtok(NULL, " "); //campo = "22"
´´´
atoi(cad); //Devuelve el int que haya en cad