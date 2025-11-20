#include "../headers/fileState.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int getFileDescriptor(const char *filename, int chunk_id,
                      StorageStateOuter *state) {
    int fd = -1;
    pthread_mutex_lock(&state->lock);
    for (int i = 0; i < state->num_file_count; i++) {
        if (strcmp(filename, state->file_to_chunk_state[i].file_path) == 0) {
            for (int j = 0; j < state->file_to_chunk_state[i].num_chunks; j++) {
                if (chunk_id == state->file_to_chunk_state[i].chunk_ids[j]) {
                    char full_path[512];
                    snprintf(full_path, sizeof(full_path), "data/%s/%d",
                             filename, chunk_id);
                    fd = open(full_path, O_RDONLY);
                    goto breakerLoop;
                }
            }
        }
    }
breakerLoop:
    pthread_mutex_unlock(&state->lock);
    return fd;
}
