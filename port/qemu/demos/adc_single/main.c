#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"

#include <stdlib.h>

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

void send_adc_sample(unsigned int sample)
{
    send_byte(hex_to_char((sample >> 8) & 0xf));
    send_byte(hex_to_char((sample >> 4) & 0xf));
    send_byte(hex_to_char(sample & 0xf));
}

/* ADC functions */
void init_adc(void) {
    ADC_InitTypeDef ADC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    /* Configure peripheral clock. */
    /* Per the STM32F103xB datasheet, the maximum frequency for the
     * the ADC is 14 Mhz.  Using a ADC prescaler value of 6 gives
     * 72 Mhz / 6 = 12 Mhz
     */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_Init(ADC1, &ADC_InitStructure);
     
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
   // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
   
    ADC_Cmd(ADC1, ENABLE);

    /* Enable ADC1 reset calibration register */
    ADC_ResetCalibration(ADC1);
    /* Check the end of ADC1 reset calibration register */
    while(ADC_GetResetCalibrationStatus(ADC1));

    /* Start ADC1 calibration */
    ADC_StartCalibration(ADC1);
    /* Check the end of ADC1 calibration */
    while(ADC_GetCalibrationStatus(ADC1));

    ADC_TempSensorVrefintCmd(ENABLE);
}

uint16_t sample_adc(uint8_t adc_channel) {
    /* Is it safe to set the channel immediately before a conversion?
     * As long as the channel does not change since the last sampling,
     * I assume it is OK.
     */
    ADC_RegularChannelConfig(ADC1, adc_channel, 1, ADC_SampleTime_239Cycles5);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);

    while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
   
    return ADC_GetConversionValue(ADC1);
}



int main(void)
{
    /* Initialization */
    init_led();

    init_button();
    enable_button_interrupts();

    init_rs232();
    enable_rs232();

    init_adc();

    rs232_print_str("Starting...\n");

    /* Infinite loop to sample ADC and print results.
     * There are several modes - see the README. */
    int mode = 1;
    while(1) {
        uint16_t adc_value;
        switch(mode) {
            case 1:                
                adc_value = sample_adc(ADC_Channel_16);
                rs232_print_str("MODE 1   Temp(Raw)=");
                send_adc_sample(adc_value);
                /* Also try converting the raw value to an actual
                 * temperature using the formula from the reference manual.
                 * TODO: validate this formula or adjust it to make it
                 * accurate.
                 */

                int tempC = (((1.43*0xFFF/3.3) - adc_value)/4.3) + 25;
                rs232_print_str("   Temp(Deg Celsius)=");
                send_number(tempC, 10);
                send_byte('\n');
                break;
            case 2:
                adc_value = sample_adc(ADC_Channel_17);
                rs232_print_str("MODE 2   VREF=");
                send_number(adc_value, 10);
                //send_adc_sample(adc_value);
                send_byte('\n');
                break;
            case 3:
                adc_value = sample_adc(ADC_Channel_8);
                rs232_print_str("MODE 3   PC0=");
                send_adc_sample(adc_value);
                send_byte('\n');
                break;
        }

        /* A button has been pressed.  Update the mode. */
        if(button_pressed) {
            button_pressed = 0;

            mode++;
            if(mode > 4) {
                mode = 1;
            }

            if(mode == 4) {
                RCC_ClocksTypeDef RCC_Clocks;
                RCC_GetClocksFreq(&RCC_Clocks);
                rs232_print_str("\nMODE 4\n");
                rs232_print_str("SYSCLK=");
                send_number(RCC_Clocks.SYSCLK_Frequency, 10);
                rs232_print_str("\n");
                rs232_print_str("HCLK=");
                send_number(RCC_Clocks.HCLK_Frequency, 10);
                rs232_print_str("\n");
                rs232_print_str("PCLK2=");
                send_number(RCC_Clocks.PCLK2_Frequency, 10);
                rs232_print_str("\n");
                rs232_print_str("ADCCLK=");
                send_number(RCC_Clocks.ADCCLK_Frequency, 10);
                rs232_print_str("\n\n");
            }
        }
    }
}
