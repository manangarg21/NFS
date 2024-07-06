#define  MAX_PATHS 200
#define  IP_LENGTH 32
#define MAXPATHLENGTH 256

struct request_header
{
    char ip_address[IP_LENGTH];
    int nm_port;
    int client_port;
    char accessible_paths[MAX_PATHS][256];
    int num_paths;
    int status;
    int dupli1;
    int dupli2;
    int dupyflag;
    int flag_RW[MAX_PATHS];
};

struct global_operation
{
    int operation;
    char path1[MAXPATHLENGTH];
    char path2[MAXPATHLENGTH];
};

struct entries {
    char *ip;
    int port;
    int portforclient;
    int numberofpaths;
    char pathsaccessible[MAX_PATHS][MAXPATHLENGTH];
};

extern int spinlock;