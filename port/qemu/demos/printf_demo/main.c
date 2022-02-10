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

static void prime_number_100_c() {
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

int main(void) {
    uint8_t b;
    int32_t num1 = 0x1234;
    char* str1 = "Strings";
    char ch1 = 'Z';

    // first init myprintf device(usart2)
    myprintf_init();
    prime_number_100_c();
    while (1) {
        myprintf("test num %d=0x%x str %s ch %c\n", num1, num1, str1, ch1);
        sleep();
    }
}
