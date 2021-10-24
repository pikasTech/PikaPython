
#include "cm32m101a_conf.h"
#include "share.h"
#include "led.h"
#include "relay.h"
#include "key_exti.h"
#include <stdio.h>
#include "uart.h"
#include "pikaScript.h"

int main(void)
{
	Led_Init();
	Relay_Init();
	Key_ExtiInit();
	Usart1_Init();
    
    /* launch pikascript */
    pikaScriptInit();
	while (1)
	{
	}
}
