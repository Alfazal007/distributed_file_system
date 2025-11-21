#define _DEFAULT_SOURCE
#include "../headers/fileState.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int remove_directory(const char *path) {
    DIR *dir = opendir(path);
    struct dirent *entry;
    char filepath[1024];
    struct stat statbuf;
    if (!dir) {
        perror("opendir");
        return -1;
    }
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 ||
            strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);
        if (lstat(filepath, &statbuf) == -1) {
            perror("lstat");
            closedir(dir);
            return -1;
        }
        if (S_ISDIR(statbuf.st_mode)) {
            if (remove_directory(filepath) == -1) {
                closedir(dir);
                return -1;
            }
        } else {
            if (unlink(filepath) == -1) {
                perror("unlink");
                closedir(dir);
                return -1;
            }
        }
    }
    closedir(dir);
    if (rmdir(path) == -1) {
        perror("rmdir");
        return -1;
    }
    return 0;
}

bool delete_file(const char *filename, StorageStateOuter *state) {
    char dir_path[512];
    snprintf(dir_path, sizeof(dir_path), "data/%s", filename);
    int del_result = remove_directory(dir_path);
    if (del_result != 0) {
        return false;
    }
    remove_from_struct(state, (char *)filename);
    return true;
}
