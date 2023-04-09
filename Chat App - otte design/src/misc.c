#include "misc.h"

void getNetDetails(char *info, char *ip, int *port) {
    // extract IP and Port info 
    // delimeter is ':'
    char *token = strtok(info, ":");

    if (token != NULL) {
        // get ip
        strcpy(ip, token);

        // look for port
        token = strtok(NULL, ":");

        if (token != NULL) {
            // get port
            *port = atoi(token);
        }
    }
}
