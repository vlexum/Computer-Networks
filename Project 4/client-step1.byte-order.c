#include "client-step1.byte-order.h"

/************************************************************************
 * MAIN
 ************************************************************************/

int main() {
    int server_socket;                  // client side socket
    struct sockaddr_in server_address;  // client socket naming struct
    uint32_t num; 
    uint32_t cycles;       
    
    printf("3A + 1 Client\n");

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

    // get num from user
    printf("Enter an integer: ");

    // read in 32 bit int gotta use macros from inttypes
    int scanned = scanf("%" SCNu32, &num);

    // make sure we got a valid number
    if (scanned != 1 || num < 0) {
        perror("Error getting number!\n");
        exit(EXIT_FAILURE);
    }
    
    // send number to server
    num = htonl(num);
    write(server_socket, &num, sizeof(uint32_t));

    // read back result
    read(server_socket, &cycles, sizeof(uint32_t));
    cycles = ntohl(cycles);

    // return the result to user
    printf("Number of Cycles: %lu\n", (unsigned long)cycles);

    // close the socket
    close(server_socket);
     
    return EXIT_SUCCESS;
}