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

/* Function prototypes */
void* handle_client(void* arg);

// forward declaration
void readInt32(int socket, uint32_t *result);
void writeInt32(int socket, uint32_t num);
void readMessage(int socket, Message *msg);
void sendMessage(int socket, Message msg);

/* Preprocessor directives */
#define SERVER_ADDR "142.11.217.88" // loopback ip address
#define PORT 23657              // port the server will listen on


#define FALSE 0
#define TRUE !FALSE

#define NUM_CONNECTIONS 20       // number of pending connections in the connection queue

#endif