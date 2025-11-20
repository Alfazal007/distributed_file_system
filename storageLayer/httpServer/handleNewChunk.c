#include "../headers/fileOperation.h"
#include "../headers/fileState.h"
#include <stdio.h>

bool handleNewChunk(connection_info_t *con_info, char **response_text,
                    const char **page, int *should_free, const char *filename,
                    StorageStateOuter *state, int chunk_id) {
    FileChunk_t chunk;
    chunk.files_bytes_data = (unsigned char *)con_info->data;
    chunk.data_size = con_info->size;
    bool write_result = saveChunkToFile(filename, chunk.files_bytes_data,
                                        chunk.data_size, state, chunk_id);
    if (!write_result) {
        return false;
    }
    *response_text = malloc(200);
    if (*response_text != NULL) {
        snprintf(*response_text, 200,
                 "{\"success\": true, \"chunkId\": %d, \"bytes\": %zu}",
                 chunk_id, chunk.data_size);
        *page = *response_text;
        *should_free = 1;
    }
    printf("here3");
    return true;
}
