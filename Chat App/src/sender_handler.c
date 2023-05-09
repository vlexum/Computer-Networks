#include "sender_handler.h"

void *sender_handler() {
    bool shutdown = false;
    bool joined = false;
    int server_socket;                  // client side socket
    struct sockaddr_in server_address;  // client socket naming struct
    char message[64]; // store user input

    // create addr struct - ip set later
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);

    // unlock the mutex 
    // pthread_mutex_unlock(&lock);

    while(!shutdown) {
        Message newMsg;

        // get user input
        fgets(message, 64, stdin);

        // will need to clear input buffer

        printf("\x1b[1F"); // Move to beginning of previous line
        printf("\x1b[2K"); // Clear entire line

        // parse input
        Commands cmd = createMessage(message, &newMsg);

        // check if a join
        if (cmd == JOIN) {
            // check if already joined
            if (joined) {
                printf("Already in server, please leave to join another\n");
                continue;
            }

            joined = true;

            // set ip in addr struct - allows for user to input diff ip than config
            server_address.sin_addr.s_addr = inet_addr(SERVER_IP);

            printf("Joining %s\n", SERVER_IP);
        }

        // send to server if joined
        if (joined) {
            // create an unnamed socket, and then name it
            server_socket = socket(AF_INET, SOCK_STREAM, 0);

           // connect to server socket
            if (connect(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
                perror("Error connecting to server!\n");
                exit(EXIT_FAILURE);
            }

            sendMessage(server_socket, newMsg);

            // close the socket
            close(server_socket);
        }

        // check if leave and not joined
        if (!joined && cmd == LEAVE) {
            printf("No server to leave!\n");
        }

        // set flags - after message has been sent
        joined = (joined && !(cmd == LEAVE || cmd == SHUTDOWN || cmd == SHUTDOWN_ALL));

        shutdown = (cmd == SHUTDOWN || cmd == SHUTDOWN_ALL);
    }

    printf("Shutting Down!\n");

    pthread_exit(NULL);
}