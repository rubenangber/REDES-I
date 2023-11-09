# REDES-I

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
Retorna el valor del tamaño de los datos que haya podido recibir <= tamDatos
- s --> socket (s)
- datos --> datos a enviar (&datos)
- tamDatos --> numeros de bytes de los datos a enviar (sizeof(char) * numero de caracteres)
- flags --> (0)

#### int close(int s)
Cierra un fichero<br>
No importa el valor de retorno
- s --> socket que hemos querido cerrar