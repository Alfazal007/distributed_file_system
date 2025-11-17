#include "../headers/fileOperation.h"
#include "../headers/fileState.h"
#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

bool createDirectories(const char *path) {
    char temp[512];
    char *pos = NULL;
    strncpy(temp, path, sizeof(temp) - 1);
    temp[sizeof(temp) - 1] = '\0';
    pos = strrchr(temp, '/');
    if (pos == NULL) {
        return true;
    }
    *pos = '\0';
    for (char *p = temp + 1; *p; p++) {
        if (*p == '/') {
            *p = '\0';
            mkdir(temp, 0755);
            *p = '/';
        }
    }
    mkdir(temp, 0755);
    return true;
}

bool saveChunkToFile(const char *filename, unsigned char *data,
                     size_t data_size, StorageStateOuter *state, int chunk_id) {
    if (filename == NULL || data == NULL || data_size == 0) {
        fprintf(stderr, "Invalid arguments to saveChunkToFile\n");
        return false;
    }
    char full_path[512];
    snprintf(full_path, sizeof(full_path), "data/%s/%d", filename, chunk_id);
    if (!createDirectories(full_path)) {
        fprintf(stderr, "Failed to create directories\n");
        return false;
    }
    FILE *file = fopen(full_path, "ab");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", full_path);
        return false;
    }
    size_t written = fwrite(data, 1, data_size, file);
    if (written != data_size) {
        fprintf(stderr,
                "Failed to write all bytes. Expected: %zu, "
                "Written: %zu\n",
                data_size, written);
        fclose(file);
        return false;
    }
    insert_to_struct(state, filename, chunk_id);
    fclose(file);
    printf("Successfully saved %zu bytes to %s\n", data_size, full_path);
    return true;
}

int getNextChunkNumber(const char *directory) {
    char full_dir[512];
    snprintf(full_dir, sizeof(full_dir), "data/%s", directory);
    DIR *dir = opendir(full_dir);
    if (dir == NULL) {
        return 1;
    }
    int max_number = 0;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        int num = atoi(entry->d_name);
        if (num > max_number) {
            max_number = num;
        }
    }
    closedir(dir);
    return max_number + 1;
}
