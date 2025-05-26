#ifndef _AFDX_H_
#define _AFDX_H_

#include <stdint.h>

/// @brief AFDX STRUCTURE
typedef struct
{
    uint16_t id;        //VIRTUAL LINK ID
    uint32_t timestamp; //TIMESTAMP IN MILISSECONDS
    char payload[50];   //FRAME DATA
}AfdxFrame_t;

/// @brief FUNCTION TO CREATE AN AFDX FRAME
/// @param frame AFDX FRAME
/// @param vlId AFDX ID
/// @param data DATA TO SEND
void AfdxCreateFrame(AfdxFrame_t *frame, uint16_t vlId, const char *data);

#endif