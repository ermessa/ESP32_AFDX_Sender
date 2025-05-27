#ifndef _CRC32_H_
#define _CRC32_H_

#include <stdint.h>
#include <stddef.h>

//entender o porque disso
#ifdef __cplusplus
extern "C" {
#endif

void crc32Init(void);

uint32_t crc32(const uint8_t *data, size_t len);

#ifdef __cplusplus
}
#endif
#endif