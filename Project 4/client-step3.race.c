#include "client-step2.race.h"

/************************************************************************
 * MAINs
 ************************************************************************/

int main() {
    int numThreads = 20;
    
    printf("3A + 1 Client\n");

    // send int over and over to server
    for (int taskCounter = 1; taskCounter <= numThreads; taskCounter++) {
        // make thread
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_server, (void*)&taskCounter) != 0) {
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

    uint32_t num = *((int*)arg);
    uint32_t cycles; 
    ssize_t bytesWanted = 4;

    // send number to server
    num = htonl(num);
    write(server_socket, &num, sizeof(uint32_t));

    // read back result
    ssize_t bytesRead = read(server_socket, &cycles, sizeof(uint32_t));

    // check how many bytes were read
    if (bytesRead < bytesWanted) {
        printf("Only read %ld bytes\n", bytesRead);
    }

    cycles = ntohl(cycles);

    // return the result to user
    printf("Number of Cycles: %lu\n", (unsigned long)cycles);

    // close the socket
    close(server_socket);

    pthread_exit(NULL);
}