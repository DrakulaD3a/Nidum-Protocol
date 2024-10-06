#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "libnp.h"

const uint8_t max_option[2] = {
    NP_INFO_TEXT,
    NP_REPLY_INVALID_DATA,
};

const int last_type_index = sizeof(max_option) / sizeof(uint8_t) - 1;

void print_packet(struct np_packet *pkt) {
    printf("--NP PACKET--\n");

    printf("type: %x\noption: %x\nsize: %x\n", pkt->type, pkt->option,
           pkt->size);

    printf("data: ");
    for (int i = 0; i < pkt->size; i++) {
        printf("%c", pkt->payload[i]);
    }
    printf("\n");
}

void reset_serialize_result(struct serialize_result *result) {
    result->size = 0;
    result->reply = NP_REPLY_UNSET;
}

void serialize(uint8_t buffer[NP_MAXIMUM_PKT_SIZE], struct np_packet *pkt,
               struct serialize_result *result) {
    reset_serialize_result(result);

    if (pkt->type > last_type_index) {
        result->reply = NP_REPLY_BAD_TYPE;
        return;
    }
    buffer[0] = pkt->type;

    if (pkt->option > max_option[pkt->type]) {
        result->reply = NP_REPLY_BAD_OPTION;
        return;
    }
    buffer[1] = pkt->option;

    if (pkt->size < NP_MINIMUM_PKT_SIZE || pkt->size > NP_MAXIMUM_PKT_SIZE) {
        result->reply = NP_REPLY_BAD_SIZE;
        return;
    }
    buffer[2] = pkt->size >> 8;
    buffer[3] = pkt->size;

    for (int i = 0; i < pkt->size - NP_MINIMUM_PKT_SIZE; i++) {
        buffer[NP_MINIMUM_PKT_SIZE + i] = pkt->payload[i];
    }

    result->size = pkt->size;
    result->reply = NP_REPLY_VALID;
    return;
}

void reset_np_packet(struct np_packet *pkt) {
    pkt->type = NP_TYPE_UNSET;
    pkt->option = NP_UNSET_UNSET;
    pkt->size = 0;
    pkt->payload = NULL;
}

void deserialize(uint8_t buffer[NP_MAXIMUM_PKT_SIZE], struct np_packet *pkt,
                 struct serialize_result *result) {
    reset_np_packet(pkt);
    reset_serialize_result(result);

    if (buffer[0] > last_type_index) {
        result->reply = NP_REPLY_BAD_TYPE;
        return;
    }
    pkt->type = buffer[0];

    if (buffer[1] > max_option[pkt->type]) {
        result->reply = NP_REPLY_BAD_OPTION;
        return;
    }
    pkt->option = buffer[1];

    uint16_t size = (buffer[2] << 8) + buffer[3];
    if (size < NP_MINIMUM_PKT_SIZE || size > NP_MAXIMUM_PKT_SIZE) {
        result->reply = NP_REPLY_BAD_SIZE;
        return;
    }
    pkt->size = size;
    pkt->payload = (char *)malloc(pkt->size - NP_MINIMUM_PKT_SIZE);

    for (int i = 0; i < pkt->size - NP_MINIMUM_PKT_SIZE; i++) {
        pkt->payload[i] = buffer[NP_MINIMUM_PKT_SIZE + i];
    }

    result->reply = NP_REPLY_VALID;
    result->size = pkt->size;
    return;
}
