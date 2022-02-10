#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"
#include "myprintk.h"

void sleep()
{
   int i=1000000;
   while(i-->0);
}

int main(void)
{
    uint8_t b;
    int32_t num1=0x1234;
    char *str1="Strings";
    char ch1='Z';

    //first init myprintf device(usart2)
    myprintf_init();
    while(1) {
        myprintf("test num %d=0x%x str %s ch %c\n",num1,num1,str1,ch1);
        sleep();
    }
}
