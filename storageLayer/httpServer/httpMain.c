#include "../headers/fileState.h"
#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8002

typedef struct {
    Tcp_thread_args_t *tcp_args;
} http_server_context_t;

static const char *handle_hello(void) { return "Hello World!"; }

static const char *handle_status(Tcp_thread_args_t *args) {
    return "Server is running";
}

static const char *handle_file_state(Tcp_thread_args_t *args) {
    return "{\"status\": \"file state\"}";
}

static enum MHD_Result
answer_to_connection(void *cls, struct MHD_Connection *connection,
                     const char *url, const char *method, const char *version,
                     const char *upload_data, size_t *upload_data_size,
                     void **con_cls) {
    const char *page = NULL;
    int status = MHD_HTTP_OK;
    Tcp_thread_args_t *args = (Tcp_thread_args_t *)cls;

    if (strcmp(url, "/hello") == 0) {
        page = handle_hello();
    } else if (strcmp(url, "/status") == 0) {
        page = handle_status(args);
    } else if (strcmp(url, "/file-state") == 0) {
        page = handle_file_state(args);
    } else {
        page = "404 Not Found";
        status = MHD_HTTP_NOT_FOUND;
    }

    struct MHD_Response *response = MHD_create_response_from_buffer(
        strlen(page), (void *)page, MHD_RESPMEM_PERSISTENT);
    enum MHD_Result ret = MHD_queue_response(connection, status, response);
    MHD_destroy_response(response);

    return ret;
}

void *httpServer(void *arg) {
    Tcp_thread_args_t *args = (Tcp_thread_args_t *)arg;
    struct MHD_Daemon *daemon =
        MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL,
                         &answer_to_connection, args, MHD_OPTION_END);

    if (NULL == daemon) {
        fprintf(stderr, "Failed to start HTTP server\n");
        *args->shouldWork = false;
        return NULL;
    }

    printf("HTTP Server running on port %d\n", PORT);

    while (*args->shouldWork) {
        sleep(1);
    }

    MHD_stop_daemon(daemon);
    printf("HTTP Server stopped\n");
    *args->shouldWork = false;
    return NULL;
}
