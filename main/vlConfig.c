#include "vlConfig.h"

uint16_t id;
    char name[16];
    char targetIp[16];
    uint16_t port;
    uint32_t msPeriod;
    uint8_t priority;
VirtualLink_t vlTable[] = 
{
    //id,   name,       target ip,          port,   ms period,  prority
    {100,   "VL100",    "192.168.1.150",    6000,   1000,       5},
    {200,   "VL200",    "192.168.1.150",    6000,   2000,       3},
};

const int VL_COUNT = sizeof(vlTable)/sizeof(vlTable[0]);