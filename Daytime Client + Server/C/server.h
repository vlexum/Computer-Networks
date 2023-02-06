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
#include <string.h>
#include <time.h>

/* Function prototypes */
void* handle_client(void* arg);

/* Preprocessor directives */
#define SERVER_ADDR "142.11.217.88" // loopback ip address
#define PORT 23657              // port the server will listen on

// Constants for DAYTIME CLIENT connecting to time.nist.gov:13
#define DAYTIME_ADDR "time.nist.gov"
#define DAYTIME_PORT 13

#define FALSE 0
#define TRUE !FALSE

#define NUM_CONNECTIONS 1       // number of pending connections in the connection queue

