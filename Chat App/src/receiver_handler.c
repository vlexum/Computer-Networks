#include "receiver_handler.h"

void *getMessages() {
    int receiveSocket;                 // descriptor of server socket
    struct sockaddr_in server_address; // for naming the server's listening socket

    // sent when client disconnected
    signal(SIGPIPE, SIG_IGN);
    
    // create unnamed network socket for server to listen on
    if ((receiveSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating socket\n");
        exit(EXIT_FAILURE);
    }
    
    // name the socket (making sure the correct network byte ordering is observed)
    server_address.sin_family      = AF_INET;            // accept IP addresses
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);  // accept clients on any interface
    server_address.sin_port        = htons(CLIENT_PORT); // port to listen on
    
    // binding unnamed socket to a particular port
    if (bind(receiveSocket, (struct sockaddr *)&server_address, sizeof(server_address)) != 0) {
        perror("Error binding socket\n");
        exit(EXIT_FAILURE);
    }
    
    // listen for client connections (pending connections get put into a queue)
    if (listen(receiveSocket, NUM_CONNECTIONS) != 0) {
        perror("Error listening on socket\n");
        exit(EXIT_FAILURE);
    }

    // server loop
    while (true) {
        // accept connection to client
        int socket = accept(receiveSocket, NULL, NULL);
        
        Message inMsg;

        readMessage(socket, &inMsg);

        messageToChat(inMsg);
    }
}