#include "chat.h"

/************************************************************************
 * MAINs
 ************************************************************************/

// GLOBAL MUTEX - unsure if this is the best approach?
pthread_mutex_t lock;

int main() {
    /// initialize functions/variables
    pthread_t recv_thread;
    pthread_t send_thread;
    
    // read in config file
    Properties *properties_list = property_read_properties("user2.properties");

    // get config details
    SERVER_IP = property_get_property(properties_list, "Server_Address");
    SERVER_PORT = atoi(property_get_property(properties_list, "Server_Port"));
    CLIENT_IP = property_get_property(properties_list, "Client_Address");
    CLIENT_PORT = atoi(property_get_property(properties_list, "Client_Port"));
    NAME = property_get_property(properties_list, "Name");

    // init mutex and lock it 
    // pthread_mutex_init(&lock, NULL);
    // pthread_mutex_lock(&lock);

    // get input from user
    printf("Commands:\n");
    printf("JOIN 'ip' - join specified IP chat server or if ommitted join ip from config file.\n");
    printf("LEAVE - leaves chat server\n");
    printf("SHUTDOWN - leaves chat if necessary then closes client\n");
    printf("SHUTDOWN ALL - same as SHUTDOWN but server and all clients closed\n\n");
    printf("Any other message will be treated as chat message - must be connected to chat server\n\n");
    printf("Enter any command/message:\n");

    // Start up receiver thread
        // Function: getMessages
    pthread_create(&recv_thread, NULL, getMessages, NULL);

    // Start up sender thread, construct and send in initial JOIN message
        // Funtion: sender_handler
    pthread_create(&send_thread, NULL, handleMessage, NULL);
    
    // wait for the send thread to close (SHUTDOWN)
    pthread_join(send_thread, NULL);
}

void *handleMessage() {
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
    server_address.sin_port        = htons(CLIENT_PORT);       // port to listen on
    
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

Commands createMessage(char *input, Message *msg) {
    // set name - same regardles of msg type
    strcpy(msg->sender, NAME);

    // if not note send net details
    // put client ip and port into string
    char address[30];
    sprintf(address, "%s:%d", CLIENT_IP, CLIENT_PORT);

    // address info as content
    strcpy(msg->content, address);

    // check for JOIN command
    if (strncmp(input, "JOIN", 4) == 0) {
        // set type 
        msg->type = JOIN;

        // get passed in ip 
        if (strlen(input) > 5) {
            strcpy(SERVER_IP, input + 5);
        }
    }
    else if (strncmp(input, "LEAVE", 5) == 0) {
        // set type 
        msg->type = LEAVE;
    }
    else if (strncmp(input, "SHUTDOWN ALL", 12) == 0) {
        // set type 
        msg->type = SHUTDOWN_ALL;
    }
    else if (strncmp(input, "SHUTDOWN", 8) == 0) {
        // set type 
        msg->type = SHUTDOWN;
    }
    // no command -> note
    else {
        // set type 
        msg->type = NOTE;

        // replace ip with note content 
        strcpy(msg->content, input);
    }

    // return the msg type
    return msg->type;
}

void messageToChat(Message msg) {
    switch (msg.type){
        case JOIN:
            printf("%s has joined!\n", msg.sender);
            break;
        case LEAVE:
        case SHUTDOWN:
            printf("%s has left!\n", msg.sender);
            break;
        case SHUTDOWN_ALL:
            printf("%s has shutdown the server, you will now be disconnected!\n", msg.sender);
            exit(EXIT_SUCCESS);
            break;
        default:
            printf("%s: %s", msg.sender, msg.content);
            break;   
    }
}

