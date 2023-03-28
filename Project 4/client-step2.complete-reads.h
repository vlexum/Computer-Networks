#ifndef CLIENT_STEP2_COMPLETE_READS_H
#define CLIENT_STEP2_COMPLETE_READS_H

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

// function declarations
void *handle_server(void *arg);
int read_int(int socket, uint32_t *int_value_ptr);

/* Preprocessor directives */
#define SERVER_ADDR "142.11.217.88" // loopback ip address
#define PORT 23657              // port the server will listen on

#define FALSE 0
#define TRUE !FALSE

#endif