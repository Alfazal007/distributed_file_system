#include "../../proto/data.pb-c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint8_t *join_message(size_t *outlen) {
    Data__MessageFromStorageToMaster top_wrapper =
        DATA__MESSAGE_FROM_STORAGE_TO_MASTER__INIT;
    Data__JoinMessageFromStorageToMaster msg =
        DATA__JOIN_MESSAGE_FROM_STORAGE_TO_MASTER__INIT;
    top_wrapper.msg_type_case =
        DATA__MESSAGE_FROM_STORAGE_TO_MASTER__MSG_TYPE_JOIN;
    top_wrapper.join = &msg;
    size_t len =
        data__message_from_storage_to_master__get_packed_size(&top_wrapper);
    uint8_t *buffer = malloc(len);
    data__message_from_storage_to_master__pack(&top_wrapper, buffer);
    *outlen = len;
    printf("Join message \n");
    for (int i = 0; i < len; i++) {
        printf("%u ", buffer[i]);
    }
    return buffer;
}
