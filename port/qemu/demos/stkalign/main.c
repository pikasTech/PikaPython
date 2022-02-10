#define USE_STDPERIPH_DRIVER
#include "stm32_p103.h"

#define NVIC_CCR ((volatile unsigned long *)(0xE000ED14))

void EXTI0_IRQHandler(void)
{
    /* Make sure the line has a pending interrupt
     * (should this alwaysbe true if we are inside this interrupt handle? */

    if(EXTI_GetITStatus(EXTI_Line0) != RESET) {
        /* Toggle LED */
        GPIOC->ODR = GPIOC->ODR ^ 0x00001000;

        /* Clear the pending interrupt flag that triggered this interrupt. */
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
}

int main(void)
{
    unsigned long var;

    init_led();

    init_button();
    enable_button_interrupts();

    *NVIC_CCR = *NVIC_CCR | 0x200;

    __asm volatile (
            "mov r0, sp \n"
            "and r0, r0, #4 \n"
            "cmp r0, #0 \n"
            "bne    fourbytealigned1 \n"
            "sub sp, #4 \n"
            "fourbytealigned1: \n"
            "mov r0, sp \n"
            :
            :
            : "r0");

    EXTI->SWIER |= EXTI_Line0; //Address should end in 0 or 8 inside interrupt

    __asm volatile (
            "mov r0, sp \n"
            "and r0, r0, #4 \n"
            "cmp r0, #0 \n"
            "beq    eightbytealigned1 \n"
            "sub sp, #4 \n"
            "eightbytealigned1: \n"
            "mov r0, sp \n"
            :
            :
            : "r0");

    EXTI->SWIER |= EXTI_Line0; //Address should end in 0 or 8 inside interrupt






    *NVIC_CCR = *NVIC_CCR & 0xfffffdff;

    __asm volatile (
            "mov r0, sp \n"
            "and r0, r0, #4 \n"
            "cmp r0, #0 \n"
            "bne    fourbytealigned2 \n"
            "sub sp, #4 \n"
            "fourbytealigned2: \n"
            "mov r0, sp \n"
            :
            :
            : "r0");

    EXTI->SWIER |= EXTI_Line0; //Address should end in 4 or c inside interrupt

    __asm volatile (
            "mov r0, sp \n"
            "and r0, r0, #4 \n"
            "cmp r0, #0 \n"
            "beq    eightbytealigned2 \n"
            "sub sp, #4 \n"
            "eightbytealigned2: \n"
            "mov r0, sp \n"
            :
            :
            : "r0");

    EXTI->SWIER |= EXTI_Line0; //Address should end in 0 or 8 inside interrupt


    while(1);
}
