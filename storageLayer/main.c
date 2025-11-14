#include "headers/fileState.h"
#include "headers/httpMain.h"
#include "headers/tcpClient.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    bool shouldWork = true;
    StorageStateOuter *storage_state = malloc(sizeof(StorageStateOuter));
    storage_state_init(storage_state);
    int socket_fd = create_socket();
    connect_to_server(socket_fd);
    send_initial_message(socket_fd);

    Tcp_thread_args_t *args_tcp = malloc(sizeof(Tcp_thread_args_t));
    args_tcp->socket_fd = socket_fd;
    args_tcp->state = storage_state;
    args_tcp->shouldWork = &shouldWork;

    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, (void *(*)(void *))receive_messages,
                   args_tcp);
    pthread_create(&thread2, NULL, (void *(*)(void *))httpServer, args_tcp);

    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);

    cleanup(socket_fd);
    free(args_tcp);
    storage_state_destroy(storage_state);
    return 0;
}
