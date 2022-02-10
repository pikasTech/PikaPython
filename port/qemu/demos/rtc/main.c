// ADC : cf AN3116
// stm32flash /dev/ttyUSB0 -w main.bin 
#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"



void USART1_Init(void)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable peripheral clocks. */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1, ENABLE);

    /* Configure USART1 Rx pin as floating input. */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* Configure USART1 Tx as alternate function push-pull. */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

 
    /* Configure the USART1 */
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);    
}

void init_led1(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Enable GPIO C clock. */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

    /* Set the LED pin state such that the LED is off.  The LED is connected
     * between power and the microcontroller pin, which makes it turn on when
     * the pin is low.
     */
    GPIO_WriteBit(GPIOC,GPIO_Pin_12,Bit_SET);

    /* Configure the LED pin as push-pull output. */
    GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}

/* demos of RTC */
void time(int *h,int *min,int *sec)
{
     *h=(*h +(*min/59)*(*sec/59));
     *min=(*min+*sec/59)%60;
     *sec=(*sec+1)%60;
}

void send_char(char x)
{
   while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
   USART_SendData(USART1,x);
}

void send_str(char* str)
{
    int x=0;
    while(str[x]) 
     send_char(str[x++]);
}

void send_nbr(int nbr)
{
   if(nbr/10)
   send_nbr(nbr/10);
   send_char((nbr%10)+48);
}

void send_heure(int h,int min,int s)
{
   send_nbr(h);
   send_char(':');
   send_nbr(min);
   send_char(':');
   send_nbr(s);
   send_char('\n');
}

int main(void)
{
     
    init_led1();
    USART1_Init();
    int h,min,sec;
    h=0;
    min=0;
    sec=0;
    //USART_Cmd(USART2, ENABLE);
    USART_Cmd(USART1, ENABLE);
    /* enable LSE SRC CLOCK */
    RCC->BDCR |=RCC_BDCR_LSEON; 
    /*select LSE as CLOCK of RTC*/
    RCC->BDCR |=RCC_BDCR_RTCSEL_LSE; // Enable
    /* enable RTC CLK */
    RCC->BDCR |= RCC_BDCR_RTCEN;
    /*wait for syncro*/
    RTC_WaitForSynchro();
    /*Enable Sec Interrupt*/
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /*Enable ALR Interrupt*/
    RTC_ITConfig(RTC_IT_ALR, ENABLE);
    /*set prescaler for 1 seconde*/
    RTC_SetPrescaler(0x7FFF);
    /*wait until last write
     operation has finished*/
    RTC_WaitForLastTask();
    /*set ALARM 10 seconde */
    RTC_SetAlarm(0xA);
    RTC_WaitForLastTask();
    /*clear counter*/
    RTC_SetCounter(0x0);

    /*clear SEC and ALR flag*/
    RTC_ClearFlag(RTC_FLAG_SEC);
    RTC_ClearFlag(RTC_FLAG_ALR);
    
    /*declench alarm each 10 sec*/
 
  while (1)
  {
     
     while(RTC_GetFlagStatus(RTC_FLAG_SEC)==0);
       time(&h,&min,&sec);
       send_heure(h,min,sec);

       RTC_ClearFlag(RTC_FLAG_SEC); /*clear sec flag*/       
       
    if(RTC_GetFlagStatus(RTC_FLAG_ALR)){

       send_str("ALARME\n");      
       RTC_ClearFlag(RTC_FLAG_ALR); /*clear alarm flag*/
       /*clear count for declench 
            alarm each 10 sec*/
       RTC_SetCounter(0x0);

       GPIO_ResetBits(GPIOC, GPIO_Pin_12);
       GPIO_SetBits(GPIOC, GPIO_Pin_12);   // Turn on led connected to PC.4 pin 
      
       }
  }
}


