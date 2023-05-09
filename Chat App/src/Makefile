# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -pthread

# Define the source files
SRCS_SERVER = client_handler.c server.c chat_node.c message.c misc.c
HEADERS_SERVER = client_handler.h server.h chat_node.h message.h misc.h dbg.h

SRCS_CLIENT = properties.c receiver_handler.c message.c sender_handler.c client.c
HEADERS_CLIENT = properties.h receiver_handler.h message.h sender_handler.h client.h dbg.h

# Define the object files
OBJS_SERVER = $(SRCS_SERVER:.c=.o)
OBJS_CLIENT = $(SRCS_CLIENT:.c=.o)

# Define the targets
all: server client

server: $(OBJS_SERVER)
	$(CC) $(CFLAGS) $^ -o $@

client: $(OBJS_CLIENT)
	$(CC) $(CFLAGS) $^ -o $@

# Define the dependencies
client_handler.o: client_handler.c client_handler.h $(HEADERS_SERVER)
	$(CC) $(CFLAGS) -c $< -o $@

server.o: server.c server.h $(HEADERS_SERVER)
	$(CC) $(CFLAGS) -c $< -o $@

chat_node.o: chat_node.c chat_node.h $(HEADERS_SERVER)
	$(CC) $(CFLAGS) -c $< -o $@

message.o: message.c message.h $(HEADERS_SERVER) $(HEADERS_CLIENT)
	$(CC) $(CFLAGS) -c $< -o $@

misc.o: misc.c misc.h $(HEADERS_SERVER)
	$(CC) $(CFLAGS) -c $< -o $@

properties.o: properties.c properties.h $(HEADERS_CLIENT)
	$(CC) $(CFLAGS) -c $< -o $@

receiver_handler.o: receiver_handler.c receiver_handler.h $(HEADERS_CLIENT)
	$(CC) $(CFLAGS) -c $< -o $@

sender_handler.o: sender_handler.c sender_handler.h $(HEADERS_CLIENT)
	$(CC) $(CFLAGS) -c $< -o $@

client.o: client.c client.h $(HEADERS_CLIENT)
	$(CC) $(CFLAGS) -c $< -o $@

# Define the clean target
clean:
	rm -f server client $(OBJS_SERVER) $(OBJS_CLIENT)
