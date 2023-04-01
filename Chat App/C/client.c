#include "chat.h"

/************************************************************************
 * MAINs
 ************************************************************************/

// GLOBAL MUTEX - unsure if this is the best approach?
pthread_mutex_t lock;
char serverAddress[46];

int main() {
    bool joined = false;
    bool shutdown = false;

    char message[64];

    printf("Chat Client\n");

    // init mutex and lock it 
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_lock(&lock);

    // get input from user
    printf("Commands:\n");
    printf("JOIN 'ip' - join specified IP chat server or if ommitted join ip from config file.\n");
    printf("LEAVE - leaves chat server\n");
    printf("SHUTDOWN - leaves chat if necessary then closes client\n");
    printf("SHUTDOWN ALL - same as SHUTDOWN but server and all clients closed\n\n");
    printf("Any other message will be treated as chat message - must be connected to chat server\n\n");
    printf("Enter an command/message: ");

    pthread_t receiveThread;
    if (pthread_create(&receiveThread, NULL, getMessages, NULL) != 0) {
        perror("Error creating thread");
        exit(EXIT_FAILURE);
    }

    while(!shutdown) {
        Message newMsg;

        
        // get user input
        fgets(message, 64, stdin);
        //flush();

        // parse input
        Commands cmd = createMessage(message, &newMsg);


        // check if a join
        if (cmd == JOIN) {
            // check if already joined
            if (joined) {
                continue;
            }

            joined = true;
        }

        // send to server
        if (joined) {
            // make thread
            pthread_t sendThread;
            if (pthread_create(&sendThread, NULL, handleMessage, (void*)&newMsg) != 0) {
                perror("Error creating thread");
                exit(EXIT_FAILURE);
            }
            pthread_mutex_lock(&lock);
        }

        // check if leave and not joined
        if (!joined && cmd == LEAVE) {
            printf("No server to leave!\n");
        }

        // set flags - after message has been sent
        joined = (joined && !(cmd == LEAVE || cmd == SHUTDOWN || cmd == SHUTDOWN_ALL));

        shutdown = (cmd == SHUTDOWN || cmd == SHUTDOWN_ALL);
    }

    sleep(5);
    return EXIT_SUCCESS;
}

void *handleMessage(void *arg) {
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

    // get message
    Message msg = *((Message *)arg);

    // unlock the mutex 
    pthread_mutex_unlock(&lock);;

    sendMessage(server_socket, msg);

    // close the socket
    close(server_socket);

    pthread_exit(NULL);
}

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
    server_address.sin_family      = AF_INET;           // accept IP addresses
    server_address.sin_addr.s_addr = htonl(INADDR_ANY); // accept clients on any interface
    server_address.sin_port        = htons(MY_PORT);       // port to listen on
    
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

        printMessage(inMsg);
    }
}


Commands createMessage(char *input, Message *msg) {
    char command[12];
    int totalLen = 0;

    // get command type
    sscanf(input, "%s%n", command, &totalLen);

    // set name - same regardles of msg type
    strcpy(msg->sender, MY_NAME);

    // check for JOIN command
    if (strncmp(command, "JOIN", 4) == 0) {
        // set type 
        msg->type = JOIN;

        // put client ip and port into string
        char address[30];
        sprintf(address, "%s:%d", MY_IP, MY_PORT);

        // address info as content
        strcpy(msg->content, address);

        // get ip from user
        if (sscanf(input + totalLen, "%s", serverAddress) != 1) {
            // no IP address provided
            strcpy(serverAddress, SERVER_ADDR);  // set serverAddress to config ip
        }
    }
    // check for LEAVE command
    else if (strncmp(command, "LEAVE", 5) == 0) {
        // set type 
        msg->type = LEAVE;

        // no content for LEAVE message
        msg->content[0] = '\0';
    }
    // check for SHUTDOWN command
    else if (strncmp(command, "SHUTDOWN", 8) == 0) {
        // set type 
        msg->type = SHUTDOWN;

        // no content for SHUTDOWN message
        msg->content[0] = '\0';
    }
    // check for SHUTDOWN ALL command
    else if (strncmp(command, "SHUTDOWN ALL", 12) == 0) {
        // set type 
        msg->type = SHUTDOWN_ALL;

        // no content for SHUTDOWN ALL message
        msg->content[0] = '\0';
    }
    // no command -> note
    else {
        // set type 
        msg->type = NOTE;

        strcpy(msg->content, input);
    }

    // return the msg type
    return msg->type;
}

void flush() {
    // Flush the input buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}
