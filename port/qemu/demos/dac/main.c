#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"
    
/* Flag whenever the button is pressed.
 * Note that the interrupt handler is initialized to only
 * fire when the button is pressed, not released.
 */
uint8_t button_pressed = 0;
void EXTI0_IRQHandler(void)
{
    /* Make sure the line has a pending interrupt
     * (should this always be true if we are inside the interrupt handle?) */
    if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
        button_pressed = 1;
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

/* ADC functions */

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

void init_dac(void) {

    DAC_InitTypeDef DAC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    DAC_InitStructure.DAC_Trigger=DAC_Trigger_Software;
    DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_Noise;//DAC_WaveGeneration_Triangle;//DAC_WaveGeneration_Noise
    DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude=DAC_LFSRUnmask_Bits3_0;//DAC_TriangleAmplitude_255; //DAC_LFSRUnmask_Bits3_0
    DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_2, &DAC_InitStructure);

    DAC_Cmd(DAC_Channel_2, ENABLE);

    //set_DAC2_triangle_waveform_amplitude(peak);   
    //enable_DAC2_channel(true);

}
void USART1_Init(void)
{
    USART_InitTypeDef USART_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
   
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

void init_RCC()
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC ,ENABLE);
}

void sleep()
{
   int i=1000000;
   while(i-->0);
}


int main(void)
{
    /* Initialization */

    init_led();
    USART1_Init();
    init_dac();
    init_RCC();
    USART_Cmd(USART1, ENABLE);

    send_str("*******DEMO of DAC*******");     
    DAC_SetChannel2Data(DAC_Align_12b_L,150<<4);

    while(1)
    {
     DAC->SWTRIGR=0x02;
     sleep();     
    }


 return 0;  
    
}
