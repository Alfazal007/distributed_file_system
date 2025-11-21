#define _GNU_SOURCE
#include "../headers/fileState.h"
#include "../proto/data.pb-c.h"
#include <pthread.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void storage_state_init(StorageStateOuter *storage_state) {
    // TODO:: construct state from existing if it exists
    pthread_mutex_init(&storage_state->lock, NULL);
    storage_state->file_to_chunk_state = NULL;
    storage_state->num_file_count = 0;
}

void storage_state_destroy(StorageStateOuter *storage_state) {
    pthread_mutex_lock(&storage_state->lock);
    for (int i = 0; i < storage_state->num_file_count; i++) {
        free(storage_state->file_to_chunk_state[i].file_path);
        free(storage_state->file_to_chunk_state[i].chunk_ids);
    }
    free(storage_state->file_to_chunk_state);
    pthread_mutex_unlock(&storage_state->lock);
    pthread_mutex_destroy(&storage_state->lock);
}

void push_chunk_id(FileStateCurrentInner *file_state, int chunk_id) {
    int *temp = realloc(file_state->chunk_ids,
                        (file_state->num_chunks + 1) * sizeof(int));
    if (temp == NULL) {
        fprintf(stderr, "Failed to allocate memory for chunk_ids\n");
        return;
    }
    file_state->chunk_ids = temp;
    file_state->chunk_ids[file_state->num_chunks] = chunk_id;
    file_state->num_chunks++;
}

void insert_to_struct(StorageStateOuter *state, const char *file_path,
                      int chunk_id, bool directoryHasFile) {
    pthread_mutex_lock(&state->lock);
    if (!directoryHasFile) {
        state->file_to_chunk_state = realloc(state->file_to_chunk_state,
                                             (state->num_file_count + 1) *
                                                 sizeof(FileStateCurrentInner));
        FileStateCurrentInner *new_file =
            &state->file_to_chunk_state[state->num_file_count];
        new_file->file_path = strdup(file_path);
        new_file->chunk_ids = NULL;
        new_file->num_chunks = 0;
        state->num_file_count++;
    }
    FileStateCurrentInner *current_file =
        &state->file_to_chunk_state[state->num_file_count - 1];
    push_chunk_id(current_file, chunk_id);
    pthread_mutex_unlock(&state->lock);
    print_storage_state(state);
}

void remove_from_struct(StorageStateOuter *state, char *file_path) {
    pthread_mutex_lock(&state->lock);
    int index = -1;
    for (int i = 0; i < state->num_file_count; i++) {
        if (strcmp(state->file_to_chunk_state[i].file_path, file_path) == 0) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        pthread_mutex_unlock(&state->lock);
        return;
    }
    free(state->file_to_chunk_state[index].file_path);
    free(state->file_to_chunk_state[index].chunk_ids);
    for (int i = index; i < state->num_file_count - 1; i++) {
        state->file_to_chunk_state[i] = state->file_to_chunk_state[i + 1];
    }
    state->num_file_count--;
    if (state->num_file_count > 0) {
        FileStateCurrentInner *temp =
            realloc(state->file_to_chunk_state,
                    state->num_file_count * sizeof(FileStateCurrentInner));
        if (temp == NULL) {
            fprintf(stderr, "Failed to reallocate memory\n");
            pthread_mutex_unlock(&state->lock);
            return;
        }
        state->file_to_chunk_state = temp;
    } else {
        free(state->file_to_chunk_state);
        state->file_to_chunk_state = NULL;
    }
    pthread_mutex_unlock(&state->lock);
    print_storage_state(state);
}

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
            data__file_mappings__init(fm);
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

void print_storage_state(StorageStateOuter *state) {
    pthread_mutex_lock(&state->lock);
    printf("\n=== Storage State ===\n");
    printf("Total files: %d\n", state->num_file_count);
    for (int i = 0; i < state->num_file_count; i++) {
        FileStateCurrentInner *file = &state->file_to_chunk_state[i];
        printf("\nFile %d:\n", i + 1);
        printf("  Path: %s\n", file->file_path);
        printf("  Total chunks: %d\n", file->num_chunks);
        printf("  Chunk IDs: ");
        for (int j = 0; j < file->num_chunks; j++) {
            printf("%d ", file->chunk_ids[j]);
        }
        printf("\n");
    }
    printf("===================\n\n");
    pthread_mutex_unlock(&state->lock);
}
