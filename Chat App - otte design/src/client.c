#include "client.h"

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
    pthread_create(&send_thread, NULL, sender_handler, NULL);
    
    // wait for the send thread to close (SHUTDOWN)
    pthread_join(send_thread, NULL);
}
