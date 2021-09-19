#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

int main(int argc, char **argv)
{
                printf("é o escreves");
    int sock;
    struct sockaddr_in dest;
    char msg_write[1024], msg_read[100];
    char *msgteste[4];
    sock = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(8888);
    inet_aton("127.0.0.1", &dest.sin_addr);  // converte o endereço á esquerda e armazena na struct á direita Endereço de destino
    
    msgteste[0] = "GET /teste.html HTTP/1.0";
    msgteste[1] = "Host: localhost:8888";
    msgteste[2] = "User-Agent: curl/7.68.0";
            printf("é o escreves");
    connect(sock, (struct sockaddr *)&dest, sizeof(dest));
    do
    {
        //scanf("%s", msg_write);
        strcat(msg_write,"GET /index.html\t HTTP/1.1\t\n");
        printf("é o escreves");
        write(sock, msg_write, strlen(msg_write) + 1);

        //read(sock, msg_read, 1024);
        printf("%s\n",msg_read);

    } while (1);
    printf("FECHEI A CONNECTION sou o clienti hihi \n");
    close(sock);
}
