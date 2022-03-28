#ifndef _FIF0_H_
#define _FIFO_H_

#include "wm_hal.h"

typedef struct fifo_t {
    uint8_t *buf;
	uint32_t size;
	uint32_t in;
	uint32_t out;
} _fifo_str;

#define min(x,y) ((x) < (y)?(x):(y))

int FifoInit(uint8_t *fifo_addr, uint32_t fifo_size);

int FifoDataLen(void);

int FifoSpaceLen(void);

int FifoRead(uint8_t *buf, uint32_t len);

int FifoWrite(uint8_t *buf, uint32_t len);

void FifoClear(void);


#endif

