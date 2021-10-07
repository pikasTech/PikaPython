#ifndef _USART_H
#define _USART_H

void USART_Config(void);
void uputc(unsigned char c);
void uwrite(const void* data, int size);
void USART_Poll(void);
int USART_NoComm(void);

#endif
