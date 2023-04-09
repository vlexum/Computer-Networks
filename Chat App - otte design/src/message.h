#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>

#define NUM_CONNECTIONS 20       // number of pending connections in the connection queue

char *SERVER_IP;
int SERVER_PORT;
char *CLIENT_IP;
int CLIENT_PORT;
char *NAME;

typedef enum {
    JOIN,
    LEAVE,
    SHUTDOWN,
    SHUTDOWN_ALL,
    NOTE
} Commands;

typedef struct {
    uint32_t type; // Command Type 
    char sender[20]; // name of sender
    char content[80]; // message content if NOTE
} Message;

Commands createMessage(char *input, Message *msg);
void printMessage(Message msg);
void messageToChat(Message msg);
bool readMessage(int socket, Message *msg);
bool sendMessage(int socket, Message msg);