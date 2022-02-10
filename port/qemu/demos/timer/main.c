#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"

#include <stdlib.h>

// Note that we need to multiply the period by 2 in order to work properly.
// Not sure why...
uint16_t period_value = (uint16_t) 2 * (36000000 / 65535);

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

void TIM2_IRQHandler(void)
{
    /* Note that I think we could have used TIM_GetFlagStatus
       and TIM_ClearFlag instead of TIM_GetITStatus and TIM_ClearITPendingBit.
       These routines seem to overlap quite a bit in functionality. */

    /* Make sure the line has a pending interrupt
     * which should always be true.
     *  */
    if(TIM_GetITStatus(TIM2, USART_IT_TXE) != RESET) {
        /* Toggle the LED */
        GPIOC->ODR ^= 0x00001000;
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }
}

/* Timer functions */
void init_timer(void) {
    /* Configure peripheral clock. */
    /* Let's leave PCLK1 at it's default setting of 36 MHz. */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

    /* Time base configuration */
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_Period = 5 * period_value - 1;
    TIM_TimeBaseStructure.TIM_Prescaler = 65535;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

    // Enable the update interrupt
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    /* Enable the timer IRQ in the NVIC module (so that the TIM2 interrupt
     * handler is enabled). */
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Enable the timer
    TIM_Cmd(TIM2, ENABLE);
}




int main(void)
{
    /* Initialization */
    init_led();

    init_timer();

    init_button();
    enable_button_interrupts();

    init_rs232();
    enable_rs232();

    rs232_print_str("Starting...\n");

    /* Infinite loop to sample timer and print results.
     * There are several modes - see the README. */
    int mode = 1;
    uint16_t last_second_count = 0;
    while(1) {
        uint16_t curr_raw_count = TIM_GetCounter(TIM2);
        uint16_t curr_second_count = (curr_raw_count / period_value) + 1;

        // Whnever the second count changes, print a line
        if(curr_second_count != last_second_count) {
            rs232_print_str("MODE ");
            send_number(mode, 10);
            rs232_print_str("   Second Count=");
            send_number(curr_second_count, 10);
            rs232_print_str("  ");
            rs232_print_str("Raw Count=");
            send_number(curr_raw_count, 10);
            rs232_print_str("\n");
            last_second_count = curr_second_count;
        }

        // Handle special actions for modes 4 and 5.
        switch(mode) {
            case 4:
                if(!(TIM2->CR1 & TIM_CR1_CEN)) {
                    rs232_print_str("Done counting up.  Restarting timer...\n");
                    TIM_Cmd(TIM2, ENABLE);
                }
                break;
            case 5:
                if(curr_second_count == 3) {
                    rs232_print_str("Setting UG bit to reset count...\n");
                    TIM2->EGR |= 0x0001;
                }
                break;
        }

        /* A button has been pressed.  Update the mode. */
        if(button_pressed) {
            RCC_ClocksTypeDef RCC_Clocks;

            button_pressed = 0;

            mode++;
            if(mode > 6) {
                mode = 1;
            }

            // Note that documentation says to disable the timer when changing
            // between edge-aligned and center-aligned modes.  To be safe we
            // will always disable the timer when making configuration changes.
            switch(mode) {
                case 1:
                    TIM_Cmd(TIM2, DISABLE);
                    TIM_CounterModeConfig(TIM2, TIM_CounterMode_Up);
                    TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Repetitive);
                    TIM_Cmd(TIM2, ENABLE);
                    rs232_print_str("Counting up...\n");
                    break;
                case 2:
                    TIM_Cmd(TIM2, DISABLE);
                    TIM_CounterModeConfig(TIM2, TIM_CounterMode_Down);
                    TIM_Cmd(TIM2, ENABLE);
                    rs232_print_str("Counting down...\n");
                    break;
                case 3:
                    TIM_Cmd(TIM2, DISABLE);
                    TIM_CounterModeConfig(TIM2, TIM_CounterMode_CenterAligned1);
                    TIM_Cmd(TIM2, ENABLE);
                    rs232_print_str("Counting up/down...\n");
                    break;
                case 4:
                    TIM_Cmd(TIM2, DISABLE);
                    // We specify to count up.  However, it appears the timer
                    // will still count down if the timer was previously
                    // counting down during the last mode.  Not sure why...
                    TIM_CounterModeConfig(TIM2, TIM_CounterMode_Up);
                    TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Single);
                    TIM_Cmd(TIM2, ENABLE);
                    rs232_print_str("Counting up (one shot mode)...\n");
                    break;
                case 5:
                    TIM_Cmd(TIM2, DISABLE);
                    TIM_CounterModeConfig(TIM2, TIM_CounterMode_Up);
                    TIM_SelectOnePulseMode(TIM2, TIM_OPMode_Repetitive);
                    TIM_Cmd(TIM2, ENABLE);
                    rs232_print_str("Counting up (UG bit test)...\n");
                    break;
                case 6:
                    TIM_Cmd(TIM2, DISABLE);
                    RCC_GetClocksFreq(&RCC_Clocks);
                    rs232_print_str("\nMODE 4\n");
                    rs232_print_str("SYSCLK=");
                    send_number(RCC_Clocks.SYSCLK_Frequency, 10);
                    rs232_print_str("\n");
                    rs232_print_str("HCLK=");
                    send_number(RCC_Clocks.HCLK_Frequency, 10);
                    rs232_print_str("\n");
                    rs232_print_str("PCLK1=");
                    send_number(RCC_Clocks.PCLK1_Frequency, 10);
                    rs232_print_str("\n");
                    rs232_print_str("Timer Counts per Second=");
                    send_number(period_value, 10);
                    rs232_print_str("\n");
                    break;
            }
        }
    }
}
