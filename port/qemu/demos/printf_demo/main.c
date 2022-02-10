#define USE_STDPERIPH_DRIVER
#include "myprintk.h"
#include "stm32_p103.h"

void sleep() {
    int i = 1000000;
    while (i-- > 0)
        ;
}

/* redirect printf to myprintf */
#define printf myprintf

static void prime_number_100_c(void) {
    int num = 0;
    num = 0;
    /* run */
    for (int i = 2; i < 100; i++) {
        int is_prime = 1;
        for (int j = 2; j < i; j++) {
            if (i % j == 0) {
                is_prime = 0;
                break;
            }
        }
        if (is_prime) {
            num = num + i;
        }
    }
    if (1060 != num) {
        printf("[error]: prime_number_100\r\n");
    }
}

void systick_init(void) {
    uint32_t hclk_ticks_per_sec, ext_clock_ticks_per_sec;
    hclk_ticks_per_sec = SystemCoreClock;
    ext_clock_ticks_per_sec = hclk_ticks_per_sec / 8;
    if (SysTick_Config(ext_clock_ticks_per_sec)) {
        /* If SysTick_Config returns 1, that means the number ticks exceeds the
         * limit. */
        while (1)
            ;
    }
}

void SysTick_Handler(void) {
    GPIOC->ODR ^= 0x00001000;
}

int main(void) {
    uint8_t b;
    int32_t num1 = 0x1234;
    char* str1 = "Strings";
    char ch1 = 'Z';

    // first init myprintf device(usart2)
    myprintf_init();
    systick_init();
    uint32_t tic = SysTick->VAL;
    prime_number_100_c();
    myprintf("SysTick spend: %d\n", tic - (uint32_t)SysTick->VAL);
}
