#include "client.h"
#define Max_SS_no 1024
#define MAX_PATHS 256
#define MAX_PATH_LENGTH 256
#define MAX_BUFFER_AT_ONE_TIME 1024
struct com
{
    int h;
    char arr[1024];
    int socket;
    char arr1[1024];
};
void handle_flags(char *host_ip, int port, struct storageserver *SS, struct namingserver *NS, int nssock)
{
    if (SS->flag == 0)
    {
        char *ip = "127.0.0.1";
        printf("hello\n");
        printf("Connecting to another server on port %d...\n", SS->port);
        book("Connecting to storage server");
        // send_sock(host_ip, SS->port, NS);
        int sock;
        struct sockaddr_in addr;
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            perror("[-]Socket error");
            exit(1);
        }
        printf("[+]TCP server socket created.\n");

        memset(&addr, '\0', sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(SS->port);
        addr.sin_addr.s_addr = inet_addr(ip);

        int a = connect(sock, (struct sockaddr *)&addr, sizeof(addr));
        if (a == -1)
        {
            perror("connection unsuccessful");
            return;
        }
        printf("Connected to the storage server.\n");
        book("Connected to the storage server");

        char dummy[10];
        strcpy(dummy, "HELLO");
        int s1 = send(sock, dummy, 10, 0);
        if (s1 < 0)
        {
            perror("Error in sending data");
        }
        int r1 = recv(sock, dummy, 10, 0);
        if (r1 < 0)
        {
            perror("Error in receiving data");
        }
        // printf("%s\n", dummy);
        struct com *h = (struct com *)malloc(sizeof(struct com));
        strcpy(h->arr, NS->path1);
        h->h = NS->number;
        // printf("%s %d\n", h->arr, h->h);
        int s2 = send(sock, h, sizeof(struct com), 0);
        if (s2 < 0)
        {
            perror("Error in sending data");
        }
        char data[MAX_BUFFER_AT_ONE_TIME];
        char response[5];
        strcpy(response, "OK");
        if (NS->number == READ)
        {
            int a = 0;
            while (1)
            {
                int r2 = recv(sock, data, MAX_BUFFER_AT_ONE_TIME, 0);
                if (r2 < 0)
                {
                    perror("Error in receiving data");
                }

                if (strcmp(data, "STOP") == 0)
                {
                    book("Data given\n");
                    printf("\n");
                    int s3 = send(sock, response, 5, 0);
                    if (s3 < 0)
                    {
                        perror("Error in sending data data");
                    }
                    break;
                }
                else if (strcmp(data, "STOP1") == 0)
                {
                    printf("\n");
                    int s3 = send(sock, response, 5, 0);
                    if (s3 < 0)
                    {
                        perror("Error in sending data data");
                    }
                    printf("Error in reading\n");
                    book("Error in reading\n");
                    break;
                }
                printf("%s", data);
                int s4 = send(sock, response, 5, 0);
                if (s4 < 0)
                {
                    perror("Error in sending data");
                }
                data[0] = '\0';
                a++;
            }
            // printf("\n");
            // printf("%d\n", a);
        }
        else if (NS->number == WRITE)
        {

            char data[MAX_BUFFER_AT_ONE_TIME];
            char response[5];

            int r3 = recv(sock, response, 5, 0);
            if (r3 < 0)
            {
                perror("Error in receiving data");
            }
            int packetNumber = 1;
            size_t bytesRead;
            int fl = 0;

            while (1)
            {
                if (fgets(data, MAX_BUFFER_AT_ONE_TIME, stdin) != NULL)
                {
                    char *newline_pos = strchr(data, '\n');
                    if (newline_pos != NULL)
                    {
                        *newline_pos = '\0';
                        bytesRead = newline_pos - data + 1;
                        int s5 = send(sock, data, bytesRead, 0);
                        if (s5 < 0)
                        {
                            perror("Error in sending data");
                        }
                        int r4 = recv(sock, response, 5, 0);
                        if (r4 < 0)
                        {
                            perror("Error in recieving data");
                        }

                        break;
                    }
                    else
                    {
                        bytesRead = strlen(data) + 1;
                        int s6 = send(sock, data, bytesRead, 0);
                        if (s6 < 0)
                        {
                            perror("Error in sending data");
                        }
                    }
                }
                else
                {
                    fl = 1;
                    break;
                }
                int r5 = recv(sock, response, 5, 0);
                if (r5 < 0)
                {
                    perror("Error in recieving data");
                }
                if (strcmp(response, "ERROR") == 0)
                {
                    fl = 2;
                    break;
                }
            }
            if (fl != 2)
            {
                strcpy(data, "STOP");
                int s7 = send(sock, data, strlen("STOP") + 1, 0);
                if (s7 < 0)
                {
                    perror("Error in sending data");
                }
                book("Write done successfully\n");
            }
            if (fl == 1)
            {
                printf("error in fgets\n");
                book("error in fgets\n");
            }
            if (fl == 2)
            {
                printf("Error in writing\n");
                book("Error in writing\n");
            }
        }
        else if (NS->number == ADD_INFO)
        {
            int r6 = recv(sock, data, MAX_BUFFER_AT_ONE_TIME, 0);
            if (r6 < 0)
            {
                perror("Error in recieving data");
            }
            if (strcmp(data, "INVALID") == 0)
            {
                printf("Invalid path or stat doesn't exist\n");
                book("Invalid path or stat doesn't exist\n");
            }
            else
                printf("%s\n", data);
            int s8 = send(sock, response, 5, 0);
            if (s8 < 0)
            {
                perror("Error in sending data");
            }
        }
    }
    else if (SS->flag == 2)
    {
        char buffer[256];
        int r7 = recv(nssock, buffer, 256, 0);
        if (r7 < 0)
        {
            perror("Error in recieving data");
        }
        printf("%s\n", buffer);
        book(buffer);
    }
    else
    {
        printf("%d\n", SS->flag);
        printf("Waiting for more data from the initial server...\n");
        {
            char buffer[256];
            int r8 = recv(nssock, buffer, 256, 0);
            if (r8 < 0)
            {
                perror("Error in recieving data");
            }

            printf("%s\n", buffer);
            book(buffer);
        }
    }
}