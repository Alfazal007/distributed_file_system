#include "../proto/data.pb-c.h"
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *file_path;
    int *chunk_ids;
    int32_t num_chunks;
} FileStateCurrentInner;

typedef struct {
    FileStateCurrentInner *file_to_chunk_state;
    int num_file_count;
    pthread_mutex_t lock;
} StorageStateOuter;

void storage_state_init(StorageStateOuter *storage_state) {
    pthread_mutex_init(&storage_state->lock, NULL);
    storage_state->file_to_chunk_state = NULL;
    storage_state->num_file_count = 0;
}

void storage_state_destroy(StorageStateOuter *storage_state) {
    pthread_mutex_destroy(&storage_state->lock);
    free(storage_state);
}

// TODO:: implement this later on when working on http layer
void insert_to_struct(StorageStateOuter *state, char *file_path, int chunk_id) {
}

void remove_from_struct(StorageStateOuter *state, char *file_path,
                        int chunk_id) {}

uint8_t *return_current_state_encoded_in_protobufs(StorageStateOuter *state,
                                                   size_t *outlen) {
    pthread_mutex_lock(&state->lock);
    Data__MessageFromStorageToMaster top_wrapper =
        DATA__MESSAGE_FROM_STORAGE_TO_MASTER__INIT;
    top_wrapper.msg_type_case =
        DATA__MESSAGE_FROM_STORAGE_TO_MASTER__MSG_TYPE_HEALTH;
    Data__HealthMessageFromStorageToMaster msg =
        DATA__HEALTH_MESSAGE_FROM_STORAGE_TO_MASTER__INIT;
    msg.n_file_mappings = state->num_file_count;
    if (state->num_file_count > 0) {
        msg.file_mappings =
            malloc(state->num_file_count * sizeof(Data__FileMappings *));
        for (int i = 0; i < state->num_file_count; i++) {
            Data__FileMappings *fm = malloc(sizeof(Data__FileMappings));
            fm->filename = state->file_to_chunk_state[i].file_path;
            fm->n_chunk_ids = state->file_to_chunk_state[i].num_chunks;
            fm->chunk_ids = state->file_to_chunk_state[i].chunk_ids;
            msg.file_mappings[i] = fm;
        }
    }
    pthread_mutex_unlock(&state->lock);
    top_wrapper.health = &msg;
    size_t len =
        data__message_from_storage_to_master__get_packed_size(&top_wrapper);
    uint8_t *buf = malloc(len);
    data__message_from_storage_to_master__pack(&top_wrapper, buf);
    *outlen = len;
    if (state->num_file_count > 0) {
        for (int i = 0; i < state->num_file_count; i++)
            free(msg.file_mappings[i]);
        free(msg.file_mappings);
    }
    return buf;
}
