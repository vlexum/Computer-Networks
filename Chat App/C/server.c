#include "chat.h"


/* ************************************************************************* */
/* MAIN                                                                      */
/* ************************************************************************* */

// GLOBAL MUTEX - unsure if this is the best approach?
pthread_mutex_t lock;
ClientInfo *head = NULL;

int main(int argc, char** argv) {
    int server_socket;                 // descriptor of server socket
    struct sockaddr_in server_address; // for naming the server's listening socket

    // sent when client disconnected
    signal(SIGPIPE, SIG_IGN);
    
    // create unnamed network socket for server to listen on
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket\n");
        exit(EXIT_FAILURE);
    }
    
    // name the socket (making sure the correct network byte ordering is observed)
    server_address.sin_family      = AF_INET;           // accept IP addresses
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // accept clients on any interface
    server_address.sin_port        = htons(PORT);       // port to listen on
    
    // binding unnamed socket to a particular port
    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) != 0) {
        perror("Error binding socket\n");
        exit(EXIT_FAILURE);
    }
    
    // listen for client connections (pending connections get put into a queue)
    if (listen(server_socket, NUM_CONNECTIONS) != 0) {
        perror("Error listening on socket\n");
        exit(EXIT_FAILURE);
    }
    
     printf("\nServer Started!\n");

     // init mutex 
    pthread_mutex_init(&lock, NULL);

    // server loop
    while (true) {
        
        // accept connection to client
        int client_socket = accept(server_socket, NULL, NULL);
        printf("\nServer with PID %d: accepted client\n", getpid());

        // create thread to handle the client's request
        // note that this is a naive approach, i.e. there are race conditions
        // for now this is okay, assuming low load
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, (void*)&client_socket) != 0) {
            perror("Error creating thread\n");
            exit(EXIT_FAILURE);
        }

        // lock the mutex
        pthread_mutex_lock(&lock);
        
        // detach the thread so that we don't have to wait (join) with it to reclaim memory.
        // memory will be reclaimed when the thread finishes.
        if (pthread_detach(thread) != 0) {
            perror("Error detaching thread\n");
            exit(EXIT_FAILURE);
        }
    }
}


/* ************************************************************************* */
/* handle client                                                             */
/* ************************************************************************* */

void* handle_client(void* arg) { // xxxxx
    int client_socket = *((int*)arg);   // the socket connected to the client
    Message msg;

    // unlock the mutex 
    pthread_mutex_unlock(&lock);;

    // read the message
    readMessage(client_socket, &msg);
    printMessage(msg);

    // send message out to user list
    forwardMsg(msg);

    switch (msg.type) {
        case JOIN:
            addUser(msg);
            break;
        case SHUTDOWN:
        case LEAVE:
            removeUser(msg);
            break;
        case SHUTDOWN_ALL:
            exit(EXIT_SUCCESS);
            break;
    }
    
    // cleanup
    if (close(client_socket) == -1) {
        perror("Error closing socket\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Closed socket to client, exit\n");
    }
    
    pthread_exit(NULL);
}

void forwardMsg(Message msg) {
    // send message to all the clients
    ClientInfo *current = head;

    while (current != NULL) {
        // create socket
        int client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1) {
            perror("Error creating socket\n");
            exit(EXIT_FAILURE);
        }

        // set up client address structure
        struct sockaddr_in client_address;
        client_address.sin_family = AF_INET;
        client_address.sin_addr.s_addr = inet_addr(current->ip);
        client_address.sin_port = htons(current->port);

        // connect to client
        if (connect(client_socket, (struct sockaddr *)&client_address, sizeof(client_address)) != 0) {
            perror("Error connecting to client\n");
            exit(EXIT_FAILURE);
        }

        // send message to client
        sendMessage(client_socket, msg);

        // close socket to client
        if (close(client_socket) == -1) {
            perror("Error closing socket\n");
            exit(EXIT_FAILURE);
        }

        // update current
        current = current->nextClient;
    }
}


void addUser(Message msg) {
    ClientInfo *newClient = (ClientInfo *)malloc(sizeof(ClientInfo));
    char ip[16];
    int port;

    // retreive ip and port
    getNetDetails(msg.content, ip, &port);

    // set node values
    strcpy(newClient->ip, ip);
    newClient->port = port;
    newClient->nextClient = NULL;

    // place in LL
    if (head == NULL) {
        head = newClient;
    }
    // put at the end
    else { 
        ClientInfo *current = head;
        while (current->nextClient != NULL) {
            current = current->nextClient;
        }

        current->nextClient = newClient;
    }
}

void removeUser(Message msg) {
    ClientInfo *current = head;
    ClientInfo *prev = NULL;

    // find network info
    char ip[16];
    int port;

    getNetDetails(msg.content, ip, &port);

    while (current != NULL) {
        if (strcmp(current->ip, ip) == 0 && current->port == port) {
            if (prev == NULL) {
                head = current->nextClient;
            } 
            else {
                prev->nextClient = current->nextClient;
            }
            free(current);
            return;
        }

        prev = current;
        current = current->nextClient;
    }
}

void getNetDetails(char *info, char *ip, int *port) {
    // extract IP and Port info 
    // delimeter is ':'
    char *token = strtok(info, ":");

    if (token != NULL) {
        // get ip
        strcpy(ip, token);

        // look for port
        token = strtok(NULL, ":");

        if (token != NULL) {
            // get port
            *port = atoi(token);
        }
    }
}

// Debug Functions
void printClientList() {
    ClientInfo *current = head;
    int i = 1;

    // print all clients
    while (current != NULL) {
        printf("User #%d\n", i);
        printf(" - Ip: %s\n", current->ip);
        printf(" - Port: %d\n", current->port);

        // update current
        current = current->nextClient;
        i++;
    } 
}
