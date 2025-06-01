#include "afdx.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "esp_timer.h"
#include "crc32.h"
#include "arpa\inet.h"

void AfdxCreateFrame(AfdxFrame_t *frame, uint16_t vlId, const char *data)
{
    frame->id = vlId;
    frame->timestamp = esp_timer_get_time() / 1000;
    strncpy(frame->payload, data, sizeof(frame->payload)-1);
    frame->payload[sizeof(frame->payload)-1] = '\0';

    frame->crc32 = 0;

    const uint8_t *raw = (const uint8_t *)frame;
    uint32_t crc = crc32(raw, sizeof(AfdxFrame_t) - sizeof(uint32_t));

    frame->crc32 = htonl(crc);
}

void AfdxPrintFrame(const AfdxFrame_t *frame)
{
    printf("---------- AFDX Frame to Send ----------\n");
    printf("VL ID       : %u\n", frame->id);
    printf("timestamp   : %lu ms\n", frame->timestamp);
    printf("payload     : %s\n", frame->payload);
    printf("checksum    : 0x%lx\n", frame->crc32);
    printf("----------------------------------------\n");
}