#include "misc.h"

/* ======================================== */
/*                  Structs                 */
/* ======================================== */
typedef struct  {
    char ip[16];
    int port;
    struct ClientInfo *nextClient;
} ClientInfo;

/* ======================================== */
/*             Function definitions         */
/* ======================================== */
void addUser(Message msg);
void forwardMsg(Message msg);
void removeUser(Message msg);
void printClientList();