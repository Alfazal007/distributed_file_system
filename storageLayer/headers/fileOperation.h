#include "../headers/fileState.h"
#include <stdbool.h>
#include <stdio.h>

bool file_exists(const char *filepath);
bool saveChunkToFile(const char *filename, unsigned char *data,
                     size_t data_size, StorageStateOuter *state, int chunk_id);

int getNextChunkNumber(const char *directory);

bool directoryHasFiles(const char *dirpath);
