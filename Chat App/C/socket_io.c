#include "chat.h"

void readInt32(int socket, uint32_t *result) {
    // 32 bits
    ssize_t totalBytes = 4;
    ssize_t receivedBytes = 0;

    // buffer for the java int
    int8_t buffer[totalBytes];

    // loop until we receive all 4 bytes - necessary for the bit flipping
    while (receivedBytes < totalBytes) {
        // read byte(s) from socket
        ssize_t bytesRead = read(socket, buffer + receivedBytes, totalBytes - receivedBytes);
        receivedBytes += bytesRead;
        
        // if we get 0 bytes and we dont have 4 - error
        if (receivedBytes == 0) {
            perror("Error retreiving result from server\n");
            exit(EXIT_FAILURE);
        }
    }

    *result = ntohl(*(uint32_t *)buffer);
}

void writeInt32(int socket, uint32_t num) {
    // host to network byte order
    num = htonl(num);

    // send the int to the socket
    write(socket, &num, sizeof(uint32_t));
}

void sendMessage(int socket, Message msg) {
    // serialize struct into byte stream
    char buffer[sizeof(Message)];
    memcpy(buffer, &msg, sizeof(Message));

    // send the byte stream
    ssize_t bytes_sent = send(socket, buffer, sizeof(buffer), 0);

    if (bytes_sent == -1) {
        perror("Error sending\n");
        exit(EXIT_FAILURE);
    }
}

void readMessage(int socket, Message *msg) {
    // prepare to recieve the serialized byte stream
    char buffer[sizeof(Message)];

    // read into buffer
    ssize_t bytes_received = recv(socket, buffer, sizeof(buffer), 0);

    if (bytes_received == -1) {
        // handle error
        perror("Error retreiving\n");
        exit(EXIT_FAILURE);
    }

    // copy memory to the pointer
    memcpy(msg, buffer, sizeof(Message));
}