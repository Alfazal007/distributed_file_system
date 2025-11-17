#ifndef STORAGE_STATE_H
#define STORAGE_STATE_H

#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
    char *file_path;
    int *chunk_ids;
    int num_chunks;
} FileStateCurrentInner;

typedef struct {
    FileStateCurrentInner *file_to_chunk_state;
    int num_file_count;
    pthread_mutex_t lock;
} StorageStateOuter;

typedef struct {
    int socket_fd;
    StorageStateOuter *state;
    bool *shouldWork;
} Tcp_thread_args_t;

typedef struct {
    char *data;
    size_t size;
} connection_info_t;

typedef struct {
    unsigned char *files_bytes_data;
    size_t data_size;
} FileChunk_t;

void storage_state_init(StorageStateOuter *state);

void storage_state_destroy(StorageStateOuter *state);

void insert_to_struct(StorageStateOuter *state, const char *file_path,
                      int chunk_id);

void remove_from_struct(StorageStateOuter *state, char *file_path,
                        int chunk_id);

uint8_t *return_current_state_encoded_in_protobufs(StorageStateOuter *state,
                                                   size_t *outlen);

void print_storage_state(StorageStateOuter *state);

#endif
