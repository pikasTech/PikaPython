
#include "fifo.h"
#include "string.h"

#define MAX_FIFOCOUNT 3

_fifo_str fifo_str[MAX_FIFOCOUNT];

int FifoInit(uint8_t fifoID,uint8_t *fifo_addr, uint32_t fifo_size)
{
	_fifo_str *p = &fifo_str[fifoID];
	
	if(fifo_addr == NULL || fifo_size == 0)
		return -1;

	memset((char *)p, 0, sizeof(_fifo_str));
	p->buf = fifo_addr;
    p->in = 0;
    p->out = 0;
    p->size = fifo_size;
	return 0;
}

int FifoDataLen(uint8_t fifoID)
{
	_fifo_str *p = &fifo_str[fifoID];
	
	return (p->in - p->out);
}

int FifoSpaceLen(uint8_t fifoID)
{
	_fifo_str *p = &fifo_str[fifoID];
	
	return (p->size - (p->in - p->out));
}

int FifoRead(uint8_t fifoID,uint8_t *buf, uint32_t len)
{
	uint32_t i = 0, j = 0;
	_fifo_str *p = &fifo_str[fifoID];

	j = (p->out % p->size);
	len = min(len, p->in - p->out);
	i = min(len, p->size - j);
	memcpy(buf, p->buf + j, i);
	memcpy(buf + i, p->buf, len - i);
	p->out += len;
	return len;
}

int FifoWrite(uint8_t fifoID,uint8_t *buf, uint32_t len)
{
	uint32_t i = 0, j = 0;
	_fifo_str *p = &fifo_str[fifoID];

	j = p->in % p->size;
	len = min(len, p->size - p->in + p->out);
	i = min(len, p->size - j);
	memcpy(p->buf + j, buf, i);
	memcpy(p->buf, buf + i, len - i);
	p->in += len;

	return len;
}

void FifoClear(uint8_t fifoID)
{
	_fifo_str *p = &fifo_str[fifoID];
	
    p->in = 0;
    p->out = 0;
//	memset(p->buf,0,p->size);
}


