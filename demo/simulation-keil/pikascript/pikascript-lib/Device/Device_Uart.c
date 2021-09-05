#include "main.h"
#include "usart.h"
#include "gpio.h"
#include "pikaScript.h"

void Device_Uart_setName(PikaObj *self, char *name)
{
  obj_setStr(self, "name", name);
}

void Device_Uart_send(PikaObj *self, char *data)
{
  printf("[uart1]: %s\r\n", data);
}

void Device_Uart_printName(PikaObj *self)
{
  char *name = obj_getStr(self, "name");
  if (NULL == name)
  {
    printf("[error] Uart: can't find name.\r\n");
    return;
  }

  printf("%s\r\n", name);
}