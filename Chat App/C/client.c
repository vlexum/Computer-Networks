#include "chat.h"

/************************************************************************
 * MAIN
 ************************************************************************/

int main() {
    int server_socket;                  // client side socket
    struct sockaddr_in server_address;  // client socket naming struct
    char message[100]; 
    
    printf("Chat Client\n");

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

    // get input from user
    printf("Commands:\n");
    printf("JOIN 'ip' - join specified IP chat server or if ommitted join ip from config file.\n");
    printf("LEAVE - leaves chat server\n");
    printf("SHUTDOWN - leaves chat if necessary then closes client\n");
    printf("SHUTDOWN ALL - same as SHUTDOWN but server and all clients closed\n\n");
    printf("Any other message will be treated as chat message - must be connected to chat server\n\n");
    printf("Enter an command/message: ");

    // read in 32 bit int gotta use macros from inttypes
    // int scanned = scanf("%s", message);
    
    Message newMsg;
    newMsg.type = JOIN;
    strcpy(newMsg.sender, "John"); // set sender name
    strcpy(newMsg.content, "The quick brown fox jumps over the lazy dog, who barks loudly in response."); // set message content

    sendMessage(server_socket, newMsg);

    // close the socket
    close(server_socket);
     
    return EXIT_SUCCESS;
}