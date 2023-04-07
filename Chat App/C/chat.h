#ifndef CHAT_H
#define CHAT_H

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
#include "properties.h"

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

typedef struct  {
    char ip[16];
    int port;
    struct ClientInfo *nextClient;
} ClientInfo;

/* Function prototypes */
void* handle_client(void* arg);
void *handleMessage();
void printMessage(Message msg);
void addUser(Message msg);
void removeUser(Message msg);
void getNetDetails(char *info, char *ip, int *port);
void forwardMsg(Message msg);
void *getMessages();


// forward declaration
bool readInt32(int socket, uint32_t *result);
bool writeInt32(int socket, uint32_t num);
bool readMessage(int socket, Message *msg);
bool sendMessage(int socket, Message msg);
Commands createMessage(char *input, Message *msg);
void printClientList();
void messageToChat(Message msg);


#define NUM_CONNECTIONS 20       // number of pending connections in the connection queue
#define SERVER_ADDR "142.11.217.88" // loopback ip address
#define PORT 23657              // port the server will listen on

#endif