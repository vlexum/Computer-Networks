#include "chat_node.h"

ClientInfo *head = NULL;

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