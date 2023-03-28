#include "chat.h"

bool readInt32(int socket, uint32_t *result) {
    // 32 bits
    ssize_t totalBytes = 4;
    ssize_t receivedBytes = 0;

    // buffer for the java int
    uint8_t buffer[totalBytes];

    // loop until we receive all 4 bytes - necessary for the bit flipping
    while (receivedBytes < totalBytes) {
        // read byte(s) from socket
        ssize_t bytesRead = read(socket, buffer + receivedBytes, totalBytes - receivedBytes);
        receivedBytes += bytesRead;
        
        // if we get 0 bytes and we dont have 4 - error
        if (receivedBytes == 0) {
            return false;
        }
    }

    *result = ntohl(*(uint32_t *)buffer);
    return true;
}

bool writeInt32(int socket, uint32_t num) {
    // host to network byte order
    num = htonl(num);

    // send the int to the socket
    ssize_t bytesSent = write(socket, &num, sizeof(uint32_t));

    return (bytesSent != -1);
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
