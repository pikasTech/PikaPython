
#include "fifo.h"
#include "string.h"

_fifo_str fifo_str;

int FifoInit(uint8_t *fifo_addr, uint32_t fifo_size)
{
	_fifo_str *p = &fifo_str;
	
	if(fifo_addr == NULL || fifo_size == 0)
		return -1;

	memset((char *)p, 0, sizeof(_fifo_str));
	p->buf = fifo_addr;
    p->in = 0;
    p->out = 0;
    p->size = fifo_size;
	return 0;
}

int FifoDataLen(void)
{
	_fifo_str *p = &fifo_str;
	
	return (p->in - p->out);
}

int FifoSpaceLen(void)
{
	_fifo_str *p = &fifo_str;
	
	return (p->size - (p->in - p->out));
}

int FifoRead(uint8_t *buf, uint32_t len)
{
	uint32_t i = 0, j = 0;
	_fifo_str *p = &fifo_str;

	j = (p->out % p->size);
	len = min(len, p->in - p->out);
	i = min(len, p->size - j);
	memcpy(buf, p->buf + j, i);
	memcpy(buf + i, p->buf, len - i);
	p->out += len;
	return len;
}

int FifoWrite(uint8_t *buf, uint32_t len)
{
	uint32_t i = 0, j = 0;
	_fifo_str *p = &fifo_str;

	j = p->in % p->size;
	len = min(len, p->size - p->in + p->out);
	i = min(len, p->size - j);
	memcpy(p->buf + j, buf, i);
	memcpy(p->buf, buf + i, len - i);
	p->in += len;

	return len;
}

void FifoClear(void)
{
	_fifo_str *p = &fifo_str;
	
    p->in = 0;
    p->out = 0;
}


