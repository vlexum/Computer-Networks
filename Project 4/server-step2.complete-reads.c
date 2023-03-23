#include "server-step2.complete-reads.h"


/* ************************************************************************* */
/* MAIN                                                                      */
/* ************************************************************************* */

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

    // server loop
    while (TRUE) {
        
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
    uint32_t clientNum;
    uint32_t cycles = -1;
    int readResult;

    // get int from client
    readResult = read_int(client_socket, &clientNum);
    
    if (readResult != -1) {
        // log num
        printf("Client Number: %lu\n", (unsigned long)clientNum);

        // do collatz algo
        cycles = collatz(clientNum);
    }
    else {
        printf("Error retrieving number from client, sending back -1\n");
    }

    // log 
    printf("Cycles: %lu\n", (unsigned long)cycles);

    // flip bits and return the cycles to client
    cycles = htonl(cycles);
    write(client_socket, &cycles, sizeof(uint32_t));
    
    // cleanup
    if (close(client_socket) == -1) {
        perror("Error closing socket\n");
        exit(EXIT_FAILURE);
    } else {
        printf("Closed socket to client, exit\n");
    }
    
    pthread_exit(NULL);
}

// 3A + 1 
uint32_t collatz(uint32_t num) {
    uint32_t cycles = 0;

    // keep going until 1
    while(num != 1) {
        // if even - halve it 
        if (num % 2 == 0) {
            num /= 2;
        }
        else { // Odd - 3n + 1 
            num = (3 * num) + 1;
        }

        cycles++;
    }

    return cycles;
}

int read_int(int socket, uint32_t *int_value_ptr) {
    // 32 bits
    ssize_t totalBytes = 4;
    ssize_t receivedBytes = 0;

    // buffer for the java int
    int8_t buffer[totalBytes];

    // loop until we receive all 4 bytes - necessary for the bit flipping
    while (receivedBytes < totalBytes) {
        // read byte(s) from socket
        ssize_t bytesRead = read(socket, buffer + receivedBytes, totalBytes - receivedBytes);
        receivedBytes += bytesRead;
        
        // if we get 0 bytes and we dont have 4 - error
        if (receivedBytes == 0) {
            return -1;
        }
    }

    // set ptr to read int
    *int_value_ptr = ntohl(*(uint32_t *)buffer);

    // return success
    return (int)totalBytes;
}

