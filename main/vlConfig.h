#ifndef _VLCONFIG_H_
#define _VLCONFIG_H_
#include <stdint.h>
#include <stdlib.h>
#include "stdio.h"

/// @brief VIRTUAL-LINK STRUCTURE
typedef struct
{
    uint16_t id;
    char name[16];
    char targetIp[16];
    uint16_t port;
    uint32_t msPeriod;
    uint8_t priority;
} VirtualLink_t;

/// @brief VIRTUAL-LINK TABLE FOLLOWING VIRTUAL-LINK STRUCTURE
extern VirtualLink_t vlTable[];

/// @brief ELEMENTS NUMBER IN THE ARRAY
extern const int VL_COUNT;

#endif