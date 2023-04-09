#include "message.h"

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

void printMessage(Message msg) {
    printf("Message Recieved:\n");
    printf(" - Command: ");

    if (msg.type == JOIN){
        printf("JOIN\n");
    }
    else if (msg.type == LEAVE) {
        printf("LEAVE\n");
    }
    else if (msg.type == SHUTDOWN) {
        printf("SHUTDOWN\n");
    }
    else if (msg.type == SHUTDOWN_ALL) {
        printf("SHUTDOWN ALL\n");
    }
    else {
        printf("NOTE\n");
    }

    printf(" - Sender: %s\n", msg.sender);
    
    printf(" - Content: %s\n", msg.content);
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

bool readMessage(int socket, Message *msg) {
    // prepare to receive the serialized byte stream
    char buffer[sizeof(Message)];

    // read into buffer
    ssize_t totalBytes = 0;

    // keep reading until we get all the bytes
    while (totalBytes < sizeof(buffer)) {
        ssize_t bytesReceived = recv(socket, buffer + totalBytes, sizeof(buffer) - totalBytes, 0);

        if (bytesReceived <= 0) {
            // handle error
            return false;
        }

        // update total
        totalBytes += bytesReceived;
    }

    // copy memory to the pointer
    memcpy(msg, buffer, sizeof(Message));

    // successful operation
    return true;
}

bool sendMessage(int socket, Message msg) {
    // serialize struct into byte stream
    char buffer[sizeof(Message)];
    memcpy(buffer, &msg, sizeof(Message));

    // send the byte stream
    ssize_t totalBytes = 0;

    // make sure all bytes get sent
    while (totalBytes < sizeof(buffer)) {
        ssize_t bytesSent = send(socket, buffer + totalBytes, sizeof(buffer) - totalBytes, 0);

        if (bytesSent <= 0) {
            // handle error
            // maybe more?
            return false;
        }

        // update total bytes
        totalBytes += bytesSent;
    }

    // succesfull operation
    return true;
}