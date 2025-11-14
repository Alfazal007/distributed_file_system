#ifndef CLIENT_H
#define CLIENT_H

#include "fileState.h"
#include <stdbool.h>
#define BUFFER_SIZE 1024
#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8001

int create_socket(void);
void connect_to_server(int socket_fd);
void send_initial_message(int socket_fd);
void *receive_messages(void *arg);
void cleanup(int socket_fd);
#endif
