#include "vlConfig.h"

VirtualLink_t vlTable[] = 
{
    {100, "VL100", "192.168.1.150", 6000, 1000, 5},
    {200, "VL200", "192.168.1.150", 6000, 2000, 3},
};

const int VL_COUNT = sizeof(vlTable)/sizeof(vlTable[0]);