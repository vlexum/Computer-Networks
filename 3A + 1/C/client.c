#include "client.h"

/************************************************************************
 * MAIN
 ************************************************************************/

int main() {
    int client_socket;                  // client side socket
    struct sockaddr_in client_address;  // client socket naming struct
    int num;
    int32_t serverResult;
    
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
    
    // java is signed
    // make sure bits are in correct order
    int32_t endianNum = (int32_t)htonl((uint32_t)num);
    
    // send the int to the server
    write(client_socket, &endianNum, sizeof(int32_t));

    // Read the result 
    int bytesReceived = read(client_socket, &serverResult, sizeof(int32_t));
    
    if (bytesReceived == 0) {
    	perror("Error retreiving result from server\n");
    	exit(EXIT_FAILURE);
    }
   
    // close the socket
    close(client_socket);
    
    // convert recieved into to host byte order 
    uint32_t convertedResult = ntohl((uint32_t)serverResult);

    // return the result to user
    printf("Number of Cycles: %lu\n", (unsigned long)serverResult);
    
    return EXIT_SUCCESS;