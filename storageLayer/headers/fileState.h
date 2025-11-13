#ifndef STORAGE_STATE_H
#define STORAGE_STATE_H

#include <pthread.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    char *file_path;
    int *chunk_ids;
} FileStateCurrentInner;

typedef struct {
    FileStateCurrentInner *file_to_chunk_state;
    pthread_mutex_t lock;
} StorageStateOuter;

void storage_state_init(StorageStateOuter *state);

void storage_state_destroy(StorageStateOuter *state);

void insert_to_struct(StorageStateOuter *state, char *file_path, int chunk_id);

void remove_from_struct(StorageStateOuter *state, char *file_path,
                        int chunk_id);

uint8_t *return_current_state_encoded_in_protobufs(StorageStateOuter *state,
                                                   size_t *outlen);
#endif
