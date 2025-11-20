#include "../headers/fileState.h"
#include "../headers/getFileDescriptor.h"
#include "../headers/handleNewChunk.h"
#include <microhttpd.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define PORT 8002

typedef struct {
    Tcp_thread_args_t *tcp_args;
} http_server_context_t;

static enum MHD_Result
answer_to_connection(void *cls, struct MHD_Connection *connection,
                     const char *url, const char *method, const char *version,
                     const char *upload_data, size_t *upload_data_size,
                     void **con_cls) {
    Tcp_thread_args_t *args = (Tcp_thread_args_t *)cls;
    connection_info_t *con_info = *con_cls;
    if (con_info == NULL) {
        con_info = malloc(sizeof(*con_info));
        if (con_info == NULL)
            return MHD_NO;
        con_info->data = NULL;
        con_info->size = 0;
        *con_cls = con_info;
        return MHD_YES;
    }
    if (strcmp(method, "POST") == 0 && *upload_data_size != 0) {
        char *new_data =
            realloc(con_info->data, con_info->size + *upload_data_size);
        if (new_data == NULL) {
            free(con_info);
            *con_cls = NULL;
            return MHD_NO;
        }
        con_info->data = new_data;
        memcpy(con_info->data + con_info->size, upload_data, *upload_data_size);
        con_info->size += *upload_data_size;
        *upload_data_size = 0;
        return MHD_YES;
    }

    const char *page = NULL;
    char *response_text = NULL;
    int status = MHD_HTTP_OK;
    int should_free = 0;

    if (strcmp(method, "POST") == 0) {
        if (strcmp(url, "/new-chunk") == 0) {
            const char *filename = MHD_lookup_connection_value(
                connection, MHD_HEADER_KIND, "X-Filename");
            const char *chunk_id_str = MHD_lookup_connection_value(
                connection, MHD_HEADER_KIND, "X-Chunk-ID");
            if (filename == NULL || chunk_id_str == NULL) {
                page = "{\"error\":\"Missing X-Filename or X-Chunk-ID\"}";
                status = MHD_HTTP_BAD_REQUEST;
            } else {
                int chunk_id = atoi(chunk_id_str);
                if (con_info->data != NULL && con_info->size > 0) {
                    bool ok = handleNewChunk(con_info, &response_text, &page,
                                             &should_free, filename,
                                             args->state, chunk_id);
                    if (!ok)
                        status = MHD_HTTP_INTERNAL_SERVER_ERROR;
                } else {
                    page = "{\"error\":\"No request body\"}";
                    status = MHD_HTTP_BAD_REQUEST;
                }
            }
        } else {
            page = "{\"error\":\"404 Not Found\"}";
            status = MHD_HTTP_NOT_FOUND;
        }
    } else if (strcmp(method, "GET") == 0) {
        if (strcmp(url, "/chunk") == 0) {
            const char *filename = MHD_lookup_connection_value(
                connection, MHD_HEADER_KIND, "X-Filename");
            const char *chunk_id_str = MHD_lookup_connection_value(
                connection, MHD_HEADER_KIND, "X-Chunk-ID");
            if (filename == NULL || chunk_id_str == NULL) {
                page = "{\"error\":\"Missing X-Filename or X-Chunk-ID\"}";
                status = MHD_HTTP_BAD_REQUEST;
            } else {
                int chunk_id = atoi(chunk_id_str);
                int fd = getFileDescriptor(filename, chunk_id, args->state);
                if (fd == -1) {
                    page = "{\"error\":\"404 Not Found\"}";
                    status = MHD_HTTP_NOT_FOUND;
                } else {
                    struct stat st;
                    if (fstat(fd, &st) != 0) {
                        close(fd);
                        page = "{\"error\":\"Could not stat file\"}";
                        status = MHD_HTTP_INTERNAL_SERVER_ERROR;
                    } else {
                        struct MHD_Response *resp =
                            MHD_create_response_from_fd((size_t)st.st_size, fd);
                        if (resp == NULL) {
                            close(fd);
                            /*
                            free(con_info);
                            *con_cls = NULL;
                            */
                            return MHD_NO;
                        }
                        MHD_add_response_header(resp, "Content-Type",
                                                "application/octet-stream");
                        int ret =
                            MHD_queue_response(connection, MHD_HTTP_OK, resp);
                        MHD_destroy_response(resp);
                        return ret;
                    }
                }
            }
        } else {
            page = "{\"error\":\"404 Not Found\"}";
            status = MHD_HTTP_NOT_FOUND;
        }
    } else {
        page = "{\"error\":\"Method Not Allowed\"}";
        status = MHD_HTTP_METHOD_NOT_ALLOWED;
    }

    struct MHD_Response *resp = MHD_create_response_from_buffer(
        strlen(page), (void *)page,
        should_free ? MHD_RESPMEM_MUST_COPY : MHD_RESPMEM_PERSISTENT);
    MHD_add_response_header(resp, "Content-Type", "application/json");
    int ret = MHD_queue_response(connection, status, resp);
    MHD_destroy_response(resp);
    if (should_free)
        free(response_text);
    return ret;
}

static void request_completed(void *cls, struct MHD_Connection *connection,
                              void **con_cls,
                              enum MHD_RequestTerminationCode toe) {
    connection_info_t *con_info = *con_cls;
    if (con_info == NULL) {
        return;
    }
    if (con_info->data != NULL) {
        free(con_info->data);
    }
    free(con_info);
    *con_cls = NULL;
}

void *httpServer(void *arg) {
    Tcp_thread_args_t *args = (Tcp_thread_args_t *)arg;
    struct MHD_Daemon *daemon = MHD_start_daemon(
        MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, &answer_to_connection,
        args, MHD_OPTION_NOTIFY_COMPLETED, request_completed, NULL,
        MHD_OPTION_END);
    if (NULL == daemon) {
        fprintf(stderr, "Failed to start HTTP server\n");
        *args->shouldWork = false;
        return NULL;
    }
    printf("HTTP Server running on port %d\n", PORT);
    while (*args->shouldWork) {
        sleep(2);
    }
    MHD_stop_daemon(daemon);
    printf("HTTP Server stopped\n");
    *args->shouldWork = false;
    return NULL;
}
