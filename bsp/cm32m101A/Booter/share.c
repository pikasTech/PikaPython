#include "share.h"

void Delay_Ms(int ms)
{
	volatile unsigned short i;
	for (; ms > 0; ms--)
	{
		for(i = 12000;i > 0;i--) ;
	}

}

