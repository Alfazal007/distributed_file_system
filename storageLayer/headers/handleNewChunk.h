#include "fileState.h"

bool handleNewChunk(connection_info_t *con_info, char **response_text,
                    const char **page, int *should_free, const char *filename,
                    StorageStateOuter *state, int chunk_id);
