#ifndef _BMP_H_
#define _BMP_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct BMP {
  uint32_t u32Width;
  uint32_t u32Height;
  uint8_t* pu8ImageData;
} BMP_t;

#endif	/* _BMP_H_ */

