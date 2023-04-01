#include "chat.h"

bool sendMessage(int socket, Message msg) {
    // serialize struct into byte stream
    char buffer[sizeof(Message)];
    memcpy(buffer, &msg, sizeof(Message));

    // send the byte stream
    ssize_t totalBytes = 0;

    // make sure all bytes get sent
    while (totalBytes < sizeof(buffer)) {
        ssize_t bytesSent = send(socket, buffer + totalBytes, sizeof(buffer) - totalBytes, 0);

        if (bytesSent == -1) {
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

bool readMessage(int socket, Message *msg) {
    // prepare to receive the serialized byte stream
    char buffer[sizeof(Message)];

    // read into buffer
    ssize_t totaBytes = 0;

    // keep reading until we get all the bytes
    while (totaBytes < sizeof(buffer)) {
        ssize_t bytesReceived = recv(socket, buffer + totaBytes, sizeof(buffer) - totaBytes, 0);

        if (bytesReceived == -1) {
            // handle error
            return false;
        }

        // update total
        totaBytes += bytesReceived;
    }

    // copy memory to the pointer
    memcpy(msg, buffer, sizeof(Message));

    // successful operation
    return true;
}

// Debug Functions
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