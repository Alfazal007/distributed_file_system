#include "../headers/fileState.h"
#include "../headers/serialize.h"
#include "../headers/tcpClient.h"
#include "../proto/data.pb-c.h"
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int create_socket(void) {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    return socket_fd;
}

void connect_to_server(int socket_fd) {
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("inet_pton failed");
        exit(EXIT_FAILURE);
    }
    if (connect(socket_fd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server at %s:%d\n", SERVER_IP, SERVER_PORT);
}

void send_initial_message(int socket_fd) {
    size_t buf_len;
    uint8_t *join_message_buf = join_message(&buf_len);
    if (send(socket_fd, join_message_buf, buf_len, MSG_WAITALL) < 0) {
        perror("send failed");
        free(join_message_buf);
        exit(EXIT_FAILURE);
    }
    free(join_message_buf);
}

void receive_messages(int socket_fd, StorageStateOuter *state) {
    uint8_t buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(socket_fd, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received < 0) {
            perror("recv failed");
            exit(EXIT_FAILURE);
        } else if (bytes_received == 0) {
            printf("Server closed connection\n");
            break;
        }
        Data__MessageFromMasterToStorage *msg =
            data__message_from_master_to_storage__unpack(
                NULL, bytes_received - 1, buffer);
        if (msg == NULL) {
            fprintf(stderr, "Failed to decode message from Master node\n");
            return;
        }
        printf("\nReceived msg id = : %s\n", msg->id);
        size_t buf_len;
        uint8_t *file_state_message_buf =
            return_current_state_encoded_in_protobufs(state, &buf_len);
        if (send(socket_fd, file_state_message_buf, buf_len, 0) < 0) {
            perror("send failed");
            free(file_state_message_buf);
            exit(EXIT_FAILURE);
        }
        free(file_state_message_buf);
    }
}

void cleanup(int socket_fd) {
    close(socket_fd);
    printf("Disconnected\n");
}
