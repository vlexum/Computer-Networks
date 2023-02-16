#include "client.h"

/************************************************************************
 * MAIN
 ************************************************************************/

int main() {
    int client_socket;                  // client side socket
    struct sockaddr_in client_address;  // client socket naming struct
    int num;      
    
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

    int scanned = scanf("%d", &num);

    // make sure we got a valid number
    if (scanned != 1 || num < 0) {
        perror("Error getting number!\n");
        exit(EXIT_FAILURE);
    }
    
    // java is signed - values returned should be positive but to be safe will use int32_t
    // make sure bits are in correct order
    int32_t endianNum = (int32_t)htonl((uint32_t)num);
    
    // send the int to the server
    write(client_socket, &endianNum, sizeof(int32_t));

    // java int is 4 bytes
    ssize_t totalBytes = 4;
    ssize_t receivedBytes = 0;

    // buffer for the java int
    int8_t buffer[totalBytes];
    
    // loop until we receive all 4 bytes - necessary for the bit flipping
    while (receivedBytes < totalBytes) {
        // read byte(s) from socket
    	ssize_t bytesRead = read(client_socket, buffer + receivedBytes, totalBytes - receivedBytes);
    	receivedBytes += bytesRead;
    	
        // if we get 0 bytes and we dont have 4 - error
    	if (receivedBytes == 0) {
    	   perror("Error retreiving result from server\n");
    	   exit(EXIT_FAILURE);
    	}
    }
    
    // convert recieved int to host byte order 
    uint32_t convertedResult = ntohl(*(uint32_t *)buffer);

    // return the result to user
    printf("Number of Cycles: %lu\n", (unsigned long)convertedResult);

    // close the socket
    close(client_socket);
     
    return EXIT_SUCCESS;
}