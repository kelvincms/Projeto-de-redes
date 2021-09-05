//Bibliotecas

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
//Defines

#define PATH "/home/montoli/Downloads/Multi-threaded-HTTP-Server-master/arquivosserv" // Caminho para os arquivos do servidor
#define PORT_NO 8888                                                                  // Numero da porta
#define BUFFER_SIZE 1024                                                              // Tamanho do Buffer
#define CONNECTION_NUMBER 2                                                           // Quantidade máxima de requisições simultâneas
#define PENDING_CONNECTIONS 10                                                        // Tamanho da fila

//Funcoes,variaveis globais:

void html_handler(int socket, char *file_name);
void *connection_handler(void *socket_desc);

int thread_count = 0; // Contagem de threads ativas.
sem_t mutex;          // Semáforo para controlar o mutex em momento que variáveis "atômicas" precisem ser alteradas

int main(int argc, char *argv[])
{
    //inicialização de semaforos
    sem_init(&mutex, 0, 1); // Inicializa o mutex.
    int socket_desc, new_socket, len, *new_sock;
    struct sockaddr_in server, client;             // Struct referente ao socket do servidor e usuário
    len = sizeof(struct sockaddr_in);              // Tamanho da struct
    socket_desc = socket(AF_INET, SOCK_STREAM, 0); //retorna -1 caso ocorra algum erro

    if (socket_desc == -1)
    {
        puts("Nao foi possivel criar o socket\n");
        return 1;
    }
    // Inicializa a struct server
    server.sin_family = AF_INET;                     // IPV4 /AF = Address Family
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Qualquer endereço que esteja disponível // INADDR_ANY, fornece todos os endereços disponíveis para o servidor
    server.sin_port = htons(PORT_NO);                // Porta do servidor (8888), qualquer uma acima de 1000, preferencialmente 5000

    int bindReturn = bind(socket_desc, (struct sockaddr *)&server, sizeof(server)); // Caso -1, bind falha, liga o socket criado á struct server, envia o tamanho da struct servidor, "server" faz cast da struct_addrin para struct_addr

    if (bindReturn < 0)
    {
        puts("Ligacao falhou");
        return 1;
    }

    // Inicializa o servidor, enviando o socket que o servidor vai ouvir e o máximo de conexões aceitas na fila
    listen(socket_desc, PENDING_CONNECTIONS);

    puts("Esperando por conexcoes");
    //Abrir uma thread nova para cada accept realizado
    // na struct client vai ter o IP e porta do cliente, assim como o tamanho dela com len, new_socket é o socket do cliente
    while (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&len)) // Caso new_socket = 1, aceita a conexção
    {
        puts("conexcao aceita \n");
        pthread_t sniffer_thread;
        new_sock = malloc(1);                                                                // Aloca o socket do cliente
        *new_sock = new_socket;                                                              // armazena o socket do cliente no local de memória previamente alocado
        if (pthread_create(&sniffer_thread, NULL, connection_handler, (void *)new_sock) < 0) // Cria uma thread para cada requisição, dentro da função "Connection_handler" checa se o número de requisições ultrapassa ou não o que foi previamente estabelecido
        {
            puts("Nao foi possivel criar a thread\n");
            return 1;
        }
    }
    return 0;
}

void *connection_handler(void *socket_desc)
{
    int request;
    int i = 1;
    char client_reply[BUFFER_SIZE] = "", *request_lines[3];
    char *file_name;
    char *extension;

    // Socket do cliente
    int sock = *((int *)socket_desc);

    sem_wait(&mutex);                     // Começa uma seção "atômica" para checar o número de conexões e incrementar a quantidade de threads
    thread_count++;                       // Incrementa a contagem de threads
    if (thread_count > CONNECTION_NUMBER) // Checa se existem mais de 10 conexcão, caso sim, esta conexção precisa ser recusada
    {
        //Realiza os procedimentos para recusar a conexcão
        char *message = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>System is busy right now.</body></html>\n";
        write(sock, message, strlen(message));
        puts("Tentativa de conexao, porem: numero de conexcoes simultaneas excedido \n");
        thread_count--;
        sem_post(&mutex);
        free(socket_desc);
        shutdown(sock, SHUT_RDWR);
        close(sock);
        sock = -1;
        pthread_exit(NULL);
    }
    sem_post(&mutex); // Termina a seção.
    // Recebe a requisição, request é o número de bytes recebidos.
    while (request != -1)
    {
        printf("Travei na requisição\n");
        request = recv(sock, client_reply, BUFFER_SIZE, 0); // Socket do cliente, lugar onde armazenar a resposta, tamanho do lugar, flag
        printf("Recebi a requisição do amigao do sock:%d\n", sock);
        if (request > 0) // Caso request <= 0, então a requisição falhou (não foi possível receber um numero X de bytes)
        {
            //printf("Client Reply do amigao do sock%d: %s\n", sock, client_reply);
            //char *mensagemparaoCliente = "ALOU PRA TI TAMBEM";
            //write(sock, mensagemparaoCliente, strlen(mensagemparaoCliente));
            //request_lines[0] = strtok(client_reply, "\t\n");
            //i = strncmp(request_lines[0], "cabou\n", 5);

            request_lines[0] = strtok(client_reply, " \t\n");
            if (strncmp(request_lines[0], "GET\0", 4) == 0)
            {
                // COnverte o header da requisição
                request_lines[1] = strtok(NULL, " \t");
                request_lines[2] = strtok(NULL, " \t\n");
                if (strncmp(request_lines[2], "HTTP/1.0", 8) == 0 || strncmp(request_lines[2], "HTTP/1.1", 8) == 0) // Checa se a conexão é uma conexão com protocolo HTTP 1.0 ou HTTP 1.1
                {
                    char *tokens[1];
                    file_name = (char *)malloc(strlen(request_lines[1]) * sizeof(char));
                    strcpy(file_name, request_lines[1]);

                    //Separando o nome e extensão
                    tokens[0] = strtok(file_name, ".");                      //Nome
                    tokens[1] = strtok(NULL, ".");                           //Extensão
                    if (tokens[0] != NULL && strcmp(tokens[1], "html") == 0) // Checa o filename e a extensão do arquivo pedido
                    {
                        sem_wait(&mutex); // Liga o semáforo (realiza o lock)
                        html_handler(sock, request_lines[1]);
                        sem_post(&mutex); // Libera o semáforo
                    }
                    else // Formato não suportado
                    {
                        char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\n\r\n<!doctype html><html><body>400 Bad Request. Not Supported File Type (Supported File Types: html)</body></html>\n";
                        puts("houve uma tentativa de acesso, porem: Formato nao suportado\n");
                        write(sock, message, strlen(message));
                    }
                    free(extension);
                    free(file_name);
                    printf("Liberei Extension e FIle Name\n");
                }
                else // Não é uma conexão com protocolo HTTP 1.0 ou HTTP 1.1
                {
                    char *message = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>400 Bad Request</body></html>\n";
                    puts("Houve uma tentativa de conexcao, porem: Protocolo nao suportado\n");
                    write(sock, message, strlen(message));
                }
            }
            else
            {
                puts("finalera?\n");
                char *message = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>System is busy right now.</body></html>\n";
                write(sock, message, strlen(message));
                puts("Tentativa de conexao, porem: numero de conexcoes simultaneas excedido \n");
                thread_count--;
                sem_post(&mutex);
                free(socket_desc);
                shutdown(sock, SHUT_RDWR);
                close(sock);
                sock = -1;
                pthread_exit(NULL);
            }
        }
        else
        {
            puts("Requisicao falhou ou o cliente se desconectou de forma abrupta");
            char *message = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>System is busy right now.</body></html>\n";
            write(sock, message, strlen(message));
            puts("Tentativa de conexao, porem: numero de conexcoes simultaneas excedido \n");
            thread_count--;
            sem_post(&mutex);
            free(socket_desc);
            shutdown(sock, SHUT_RDWR);
            close(sock);
            sock = -1;
            pthread_exit(NULL);
        }
        printf("vou recomeçar pode andar de skeiti\n");
    }

    //Encerra a conexão
    //sleep(50);  // Caso queira simular conexões simultâneas, descomentar esta linha
    free(socket_desc);
    shutdown(sock, SHUT_RDWR);
    close(sock);
    sock = -1;
    sem_wait(&mutex);
    thread_count--;
    sem_post(&mutex);
    pthread_exit(NULL);
}

void html_handler(int socket, char *file_name)
{
    char *buffer;
    char *full_path = (char *)malloc((strlen(PATH) + strlen(file_name)) * sizeof(char));
    FILE *fp;

    strcpy(full_path, PATH);
    strcat(full_path, file_name);

    fp = fopen(full_path, "r");
    if (fp != NULL) //Caso seja diferente de nulo, significa que encotnrou o arquivo
    {
        puts("Arquivo encontrado\n");
        fseek(fp, 0, SEEK_END);
        long bytes_read = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        send(socket, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n", 44, 0);
        buffer = (char *)malloc(bytes_read * sizeof(char));

        fread(buffer, bytes_read, 1, fp);
        write(socket, buffer, bytes_read);
        free(buffer);

        fclose(fp);
    }
    else // Não encontrou o arquivo.
    {
        write(socket, "HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
        puts("Arquivo nao encontrado\n");
    }
    printf("sai do htmlhandler\n");
    free(full_path);
}
