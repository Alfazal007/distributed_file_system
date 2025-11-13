#include "headers/fileState.h"
#include "headers/tcpClient.h"
#include <stdlib.h>

int main(void) {
    StorageStateOuter *storage_state = malloc(sizeof(StorageStateOuter));
    storage_state_init(storage_state);
    int socket_fd = create_socket();
    connect_to_server(socket_fd);
    send_initial_message(socket_fd);
    receive_messages(socket_fd, storage_state);
    cleanup(socket_fd);
    storage_state_destroy(storage_state);
    return 0;
}
