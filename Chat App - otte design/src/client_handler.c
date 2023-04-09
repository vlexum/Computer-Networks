#include "client_handler.h"

/* ************************************************************************* */
/* handle client                                                             */
/* ************************************************************************* */

void* handle_client(void* arg) { // xxxxx
    int client_socket = *((int*)arg);   // the socket connected to the client
    Message msg;

    // unlock the mutex 
    pthread_mutex_unlock(&lock);

    // read the message
    readMessage(client_socket, &msg);

    // log message
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
