#include "client-step3.complete-reads.h"

/************************************************************************
 * MAIN
 ************************************************************************/

int main() {
    int numThreads = 50;    
    uint32_t num = 7; 
    
    printf("3A + 1 Client\n");

    // send int over and over to server
    for (int i = 0; i < numThreads; i++) {
        // make thread
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_server, (void*)&num) != 0) {
            perror("Error creating thread");
            exit(EXIT_FAILURE);
        }
    }
    // dont bail on me!
    sleep(5);

    return EXIT_SUCCESS;
}

void *handle_server(void *arg) {
    int server_socket;                  // client side socket
    struct sockaddr_in server_address;  // client socket naming struct

    // create an unnamed socket, and then name it
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // create addr struct
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    server_address.sin_port = htons(PORT);

    // connect to server socket
    if (connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("Error connecting to server!\n");
        exit(EXIT_FAILURE);
    }

    uint32_t num = htonl(*((uint32_t *)arg));
    uint32_t cycles; 
    int readResult;

    // send number to server
    write(server_socket, &num, sizeof(uint32_t));

    // read back result
    readResult = read_int(server_socket, &cycles);

    // return the result to user
    if (readResult != -1) {
        printf("Number of Cycles: %lu\n", (unsigned long)cycles);
    }
    else {
        printf("Error retrieving result from server\n");
    }

    // close the socket
    close(server_socket);

    pthread_exit(NULL);
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