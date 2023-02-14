#include "client.h"

/************************************************************************
 * MAIN
 ************************************************************************/
int main() {
    int client_socket;                  // client side socket
    struct sockaddr_in client_address;  // client socket naming struct
    int num;

    int32_t signed_result;
    uint32_t unsigned_result;
    
    printf("3A + 1 Client\n");

    // create an unnamed socket, and then name it
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    // create addr struct
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = inet_addr(SERVER_ADDR);
    client_address.sin_port = htons(PORT);
    
    
    // connect to server socket
    if (connect(client_socket, (struct sockaddr *)&client_address, sizeof(client_address)) == -1) {
        perror("Error connecting to server!\n");
        exit(EXIT_FAILURE);
    }

    // get num from user
    printf("Enter an integer: ");

    // make sure we got a valid number
    if (scanf("%d", &num) != 1) {
        perror("Error getting number!\n");
        exit(EXIT_FAILURE);
    }

    // Talk to server
    
    int32_t endianNum = (int32_t)htonl((uint32_t)num);
    
    // send the int to the server
    write(client_socket, &endianNum, sizeof(int32_t));

    // Read the result 
    read(client_socket, &signed_result, sizeof(int32_t));
    
    // close the socket
    close(client_socket);

    unsigned_result = ntohl((uint32_t)signed_result);

    // return the result to user
    printf("Number of Cycles: %lu\n", (unsigned long) unsigned_result);
    
    return EXIT_SUCCESS;
}

