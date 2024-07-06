#include "basic_header.h"
#include "storageservers.h"
struct global_operation
{
    int operation;
    char path1[MAX_PATH_LENGTH];
    char path2[MAX_PATH_LENGTH];
};

sem_t argumentsetter;
// void listFilesAndDirs(const char *basePath, const char *relativePath, struct entries *entry)
// {
//     DIR *dir = opendir(basePath);
//     if (!dir)
//     {
//         perror("opendir");
//         return;
//     }

//     struct dirent *dp;
//     while ((dp = readdir(dir)) != NULL)
//     {
//         if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0 || strstr(dp->d_name, ".c") != NULL || strstr(dp->d_name, ".h") != NULL || strcmp(dp->d_name, "Makefile") == 0 || strstr(dp->d_name, ".o") != NULL || strstr(dp->d_name, ".json") != NULL )
//             continue;

//         char fullPath[MAX_PATH_LENGTH + 1];
//         snprintf(fullPath, sizeof(fullPath), "%s/%s", basePath, dp->d_name);

//         char relativeFullPath[MAX_PATH_LENGTH + 1];
//         snprintf(relativeFullPath, sizeof(relativeFullPath), "%s/%s", relativePath, dp->d_name);

//         struct stat st;
//         if (stat(fullPath, &st) == 0)
//         {
//             if (S_ISDIR(st.st_mode))
//             {
//                 if (entry->numberofpaths < MAX_PATHS)
//                 {
// strcpy(entry->pathsaccessible[entry->numberofpaths], relativeFullPath);
//                     // printf("%s\n", entry->pathsaccessible[entry->numberofpaths]);

//                     entry->numberofpaths++;
//                     listFilesAndDirs(fullPath, relativeFullPath, entry);
//                 }
//                 else
//                 {
//                     fprintf(stderr, "Max paths exceeded\n");
//                 }
//             }
//             else if (S_ISREG(st.st_mode))
//             {
//                 if (entry->numberofpaths < MAX_PATHS)
//                 {
//                     strcpy(entry->pathsaccessible[entry->numberofpaths], relativeFullPath);
//                     // printf("%s\n", entry->pathsaccessible[entry->numberofpaths]);

//                     entry->numberofpaths++;
//                 }
//                 else
//                 {
//                     fprintf(stderr, "Max paths exceeded\n");
//                 }
//             }
//         }
//         else
//         {
//             perror("stat");
//         }
//     }
//     closedir(dir);
// }
void storageserverinitiailise(int port, char *ip, int portforclientcommunication, struct entries *entry, char *path)
{
    entry->port = port;
    entry->portforclient = portforclientcommunication;
    entry->ip = strdup(ip);
    entry->numberofpaths = 0;
    // printf("%s\n", entry->pathsaccessible[entry->numberofpaths]);

    int t = 0;
    while (1)
    {
        char a[MAX_PATH_LENGTH];
        scanf("%s", a);
        if (strcmp(a, "-1") == 0)
        {
            break;
        }
        char b[MAX_PATH_LENGTH];
        b[0] = '.';
        int c = 1;
        for (int i = 0; i < strlen(a); i++)
        {
            b[c++] = a[i];
        }
        b[c++] = '\0';
        strcpy(entry->pathsaccessible[t++], b);
        (entry->numberofpaths)++;
    }
}
void *performfunctions(void *arg)
{
    char response[5];
    strcpy(response, "OK");
    int res = 0;
    int s1 = 0;
    struct com h = *(struct com *)arg;
    if (h.h == DELETE_DIR)
    {
        int a = delete1(h.arr, 0);
        if (a == 0)
        {
            res = -1;

            strcpy(response, "NOT");
        }
        s1 = send(h.socket, &res, 4, 0);
        if (s1 < 0)
        {
            perror("Error sending data");
        }
    }
    else if (h.h == DELETE_FILE)
    {
        printf("hello from delete\n");
        int a = delete1(h.arr, 1);
        if (a == 0)
        {
            res = -1;
            strcpy(response, "NOT");
        }
        s1 = send(h.socket, &res, 4, 0);
        if (s1 < 0)
        {
            perror("Error sending data");
        }
    }
    else if (h.h == CREATE_DIR)
    {
        int a = createfile(h.arr, 0);
        if (a == -1)
        {
            res = -1;

            strcpy(response, "NOT");
        }
        s1 = send(h.socket, &res, 4, 0);
        if (s1 < 0)
        {
            perror("Error sending data");
        }
    }
    else if (h.h == CREATE_FILE)
    {
        int a = createfile(h.arr, 1);
        if (a == -1)
        {
            res = -1;
            strcpy(response, "NOT");
        }
        printf("%s\n", response);
        s1 = send(h.socket, &res, 4, 0);
        if (s1 < 0)
        {
            perror("Error sending data");
        }
    }
    else if (h.h == COPY_FILE)
    {
        int number = 5;
        send(h.socket, &number, 4, 0);
        recv(h.socket, &number, 4, 0);
        printf("%d\n", number);
        copy1(h.arr, h.socket, h.arr1, 1);
    }
    else if (h.h == COPY_DIR)
    {
        int number = 5;
        send(h.socket, &number, 4, 0);
        recv(h.socket, &number, 4, 0);
        printf("%d\n", number);
        copy1(h.arr, h.socket, h.arr1, 1);
    }
    else if (h.h == PASTE)
    {
        // printf("hello\n");
        paste(h.arr, h.socket);
    }
    else if (h.h == SELFCOPY)
    {
        copy1(h.arr, h.socket, h.arr1, 2);
    }
    else if (h.h == -5)
    {
        int number = 5;
        send(h.socket, &number, 4, 0);
        recv(h.socket, &number, 4, 0);
        printf("redundancy%d\n", number);
        copy1(h.arr, h.socket, h.arr1, 0);
    }
    sem_post(&argumentsetter);
    return NULL;
}
void *namingserver(void *arg)
{
    // printf("here\n");
    int sock = *(int *)arg;
    struct global_operation *name = (struct global_operation *)malloc(sizeof(struct global_operation));
    struct com h;
    sem_init(&argumentsetter, 0, 0);
    while (1)
    {

        pthread_t threadid;

        int hh = recv(sock, name, sizeof(struct global_operation), 0);
        strcpy(h.arr, name->path1);
        strcpy(h.arr1, name->path2);
        h.h = name->operation;
        h.socket = sock;
        printf("hiiii%s %s %d %d\n", h.arr, h.arr1, h.h, hh);
        if (hh < 0)
        {
            perror("Error in recieving");
        }
        else if (hh == 0)
            continue;
        // printf("here\n");
        pthread_create(&threadid, NULL, performfunctions, &h);
        sem_wait(&argumentsetter);
        // char a[5];
        // strcpy(a,"OK");
        // send(sock,a,5,0);
        // printf("%d\n",hh);

        // printf("LOOP\n");
    }
    free(name);
    return NULL;
}
