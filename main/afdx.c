#include "afdx.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "esp_timer.h"

void AfdxCreateFrame(AfdxFrame_t *frame, uint16_t vlId, const char *data)
{
    frame->id = vlId;
    frame->timestamp = esp_timer_get_time() / 1000;
    strncpy(frame->payload, data, sizeof(frame->payload)-1);
    frame->payload[sizeof(frame->payload)-1] = '\0';
}