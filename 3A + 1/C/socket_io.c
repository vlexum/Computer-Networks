#include "collatz.h"

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