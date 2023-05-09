#include "chat_node.h"

pthread_mutex_t lock;

void* handle_client(void* client_socket);