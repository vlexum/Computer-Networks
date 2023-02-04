#include "server.h"

/************************************************************************
 * MAIN
 ************************************************************************/
int main() {
    char input[100];                    // buffer for user input
    int client_socket;                  // client side socket
    struct sockaddr_in client_address;  // client socket naming struct
    char c;
    
    printf("Daytime client\n");

    // get IP for hostname
    struct hostent *host = gethostbyname(DAYTIME_ADDR);
    
    // make sure we have an IP
    if (host == NULL){
        perror("Error gathering host info\n");
        exit(EXIT_FAILURE);
    }
    
    // create an unnamed socket, and then name it
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    // create addr struct
    client_address.sin_family = AF_INET;
    client_address.sin_port = htons(DAYTIME_PORT);
    memcpy(&client_address.sin_addr, host->h_addr_list[0], host->h_length);
    
    // connect to server socket
    if (connect(client_socket, (struct sockaddr *)&client_address, sizeof(client_address)) == -1) {
        perror("Error connecting to server!\n");
        exit(EXIT_FAILURE);
    }
    
    while (TRUE) {
        printf("Input: ");
        // read string
        fgets(input, sizeof(input), stdin);
        
        int i = 0;
        while (*(input + i)) {
            // make the request to the server
            write(client_socket, input + i, sizeof(char));
            // get the result
            read(client_socket, &c, sizeof(char));
            if (c == 'q') {
                close(client_socket);
                printf("\nDone!\n");
                exit(EXIT_SUCCESS);
            }
            printf("%c", c);
            i++;
        }
    }
    
    return EXIT_SUCCESS;
}

