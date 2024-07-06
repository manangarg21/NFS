#include "basic_header.h"
#include "storageservers.h"
sem_t argsetter;
int sock;
entries entry;

void *client(void *arg)
{
    ssize_t s1, r1;
    struct com h = *(struct com *)arg;
    sem_post(&argsetter);
    char response[5];
    strcpy(response, "OK");
    if (h.h == READ)
    {
        printf("here wefrgfth\n");
        char *data = (char *)malloc(sizeof(char) * MAX_BUFFER_AT_ONE_TIME);
        printf("hehe\n");
        int a = 11;
        int fd = open(h.arr, O_RDONLY);
        if (fd == -1)
        {
            perror("Error in opening file");
        }

        while (1)
        {
            printf("hi\n");
            a = readdata(data, fd);
            if (a != 0 && a != -1)
            {
                s1 = send(h.socket, data, MAX_BUFFER_AT_ONE_TIME, 0);
                if (s1 < 0)
                {
                    perror("Error in sending data");
                }

                r1 = recv(h.socket, response, 5, 0);
            }
            else
            {
                strcpy(data, "STOP");
                s1 = send(h.socket, data, MAX_BUFFER_AT_ONE_TIME, 0);
                r1 = recv(h.socket, response, 5, 0);
                break;
            }
        }

        close(fd);
    }
    else if (h.h == WRITE)
    {
        s1 = send(h.socket, response, 5, 0);

        char *data = (char *)malloc(sizeof(char) * MAX_BUFFER_AT_ONE_TIME);
        int fd = open(h.arr, O_WRONLY | O_TRUNC);
        if (fd == -1)
        {
            perror("Error in opening file");
        }
        printf("here\n");

        while (1)
        {
            r1 = recv(h.socket, data, MAX_BUFFER_AT_ONE_TIME, 0);
            if (r1 < 0)
            {
                perror("Error in receiving data");
            }
            if (strcmp(data, "STOP") == 0)
            {
                break;
            }
            writedata(data, fd);

            s1 = send(h.socket, response, 5, 0);
            if (s1 < 0)
            {
                perror("Error in sending data");
            }
        }
        close(fd);
    }
    else if (h.h == ADD_INFO)
    {
        char *data = (char *)malloc(sizeof(char) * MAX_BUFFER_AT_ONE_TIME);
        additionalinfo(h.arr, data);
        s1 = send(h.socket, data, 1024, 0);
        if (s1 < 0)
        {
            perror("Error in sending data");
        }
        r1 = recv(h.socket, response, 5, 0);
    }
    strcpy(response, "OK");
    send(sock, response, 5, 0);
    return NULL;
}
int serverSocket11;
void doo(int port)
{
    int serverSocket11 = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket11 == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    printf("dfghjk\n");

    if (bind(serverSocket11, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("bind");
        exit(1);
    }
    listen(serverSocket11, 3);

    return;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("REWRITE PORT\n");
        return 0;
    }
    int pp = atoi(argv[2]);
    char *ip = "127.0.0.1";
    int port = 8080;
    int port1 = atoi(argv[1]);
    struct sockaddr_in addr;
    doo(pp);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
    {
        perror("[-]Socket error");
        exit(1);
    }
    printf("[+]TCP server socket created.\n");

    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    int a = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
    if (a == -1)
    {
        perror("connection unsuccessful");
        return -1;
    }
    printf("Connected to the naming server.\n");
    int number = 8;
    send(sock, &number, 4, 0);
    recv(sock, &number, 4, 0);
    printf("%d\n", number);
    char cwd[1024];
    storageserverinitiailise(pp, ip, port1, &entry, getcwd(cwd, sizeof(cwd)));
    ssize_t s2 = send(sock, &entry, sizeof(entry), 0);
    if (s2 < 0)
    {
        perror("Error in sending data");
    }
    printf("%d %d\n", s2, sizeof(entry));
    // ptrtofirstdata datafirst = malloc(sizeof(struct firstdata));
    // recv(sock,datafirst,sizeof(firstdata),0);
    pthread_t fornamingserver;

    // printf("df");
    pthread_create(&fornamingserver, NULL, namingserver, &sock);
    // printf("FGhj\n");
    // recieve a struct of four integers two port numbers and one new or old
    // to naming server till here
    // pthread_join(fornamingserver,NULL);
    printf("hu\n");
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port1);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // printf("dfghjk\n");

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        perror("bind");
        exit(1);
    }
    sem_init(&argsetter, 0, 1);

    while (1)
    {
        sem_wait(&argsetter);
        pthread_t threadid;

        printf("hi\n");
        listen(serverSocket, 5);
        printf("heid\n");
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        printf("hello\n");

        if (clientSocket == -1)
        {
            perror("accept");
            continue;
        }
        char dummy[10];
        recv(clientSocket, dummy, 10, 0);
        printf("%s\n", dummy);
        strcpy(dummy, "HI");
        send(clientSocket, dummy, 10, 0);
        struct com *h = malloc(sizeof(struct com));

        ssize_t r1 = recv(clientSocket, h, sizeof(struct com), 0);
        printf("%s %d\n", h->arr, h->h);
        h->socket = clientSocket;
        printf("hey\n");
        if (r1 < 0)
        {
            perror("Error in receiving data");
        }
        pthread_create(&threadid, NULL, client, h);
    }
}