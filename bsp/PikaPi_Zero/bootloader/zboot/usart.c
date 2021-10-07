#include <string.h>
#include "zboot_misc.h"
#include "usart.h"
#include "iap.h"
#include "cli.h"
#include "xprintf.h"

#define MAX_LEN 1152

#if (_USE_USART1)
    #define USARTx USART1
#elif (_USE_USART0)
    #define USARTx USART0
#elif (_USE_USART2)
    #define USARTx USART2
#elif (_USE_USART3)
    #define USARTx USART3
#elif (_USE_USART4)
    #define USARTx USART4
#elif (_USE_UART4)
    #define USARTx UART4
#elif (_USE_UART5)
    #define USARTx UART5
#else
    #error _USE_USARTx/UARTx not defined.
#endif

#if (_USE_GPIOA)
    #define GPIOx GPIOA
#elif (_USE_GPIOB) 
    #define GPIOx GPIOB
#elif (_USE_GPIOC) 
    #define GPIOx GPIOC
#elif (_USE_GPIOD) 
    #define GPIOx GPIOD
#else
    #error _USE_GPIOx not defined.
#endif

static struct {
    unsigned char msg[MAX_LEN];
    int size;
    int nocomm;
} g;

static inline void RCC_GPIO_Config(void) {
    // enables usart/uart clock
#if defined (STM32F303xC) || defined (STM32F401xx) || defined (STM32F40_41xxx) || defined (STM32F072) || defined (STM32F042) || defined (STM32F030) || defined (STM32F051) || defined (STM32F070xB) || defined (STM32F10X_MD_VL) || defined (STM32F10X_HD) || defined (STM32F030xC) || defined (STM32G030xx)
    #if (_USE_USART1)
    RCC->APB2ENR |= RCC_APB2Periph_USART1;
    #elif (_USE_USART2)
    RCC->APB1ENR |= RCC_APB1Periph_USART2;
    #elif (_USE_USART3)
    RCC->APB1ENR |= RCC_APB1Periph_USART3;
    #elif (_USE_USART4)
    RCC->APB1ENR |= RCC_APB1Periph_USART4;
    #elif (_USE_UART4)
    RCC->APB1ENR |= RCC_APB1Periph_UART4;
    #elif (_USE_UART5)
    RCC->APB1ENR |= RCC_APB1Periph_UART5;
    #else
        #error "_USE_USARTx/UARTx not defined."
    #endif
#elif defined (GD32F350) || defined (GD32F130_150) || defined (GD32F330)
    #if (_USE_USART1)
        RCU_REG_VAL(RCU_USART1) |= BIT(RCU_BIT_POS(RCU_USART1));
    #elif (_USE_USART2)
        RCU_REG_VAL(RCU_USART2) |= BIT(RCU_BIT_POS(RCU_USART2));
    #elif (_USE_USART0)
        RCU_REG_VAL(RCU_USART0) |= BIT(RCU_BIT_POS(RCU_USART0));
    #else
        #error "_USE_USARTx/UARTx not defined."
    #endif

#endif

    // enables gpio clock,  f0 & f3, gpio on ahb
#if defined (STM32F303xC) || defined (STM32F072) || defined (STM32F042) || defined (STM32F030) || defined (STM32F051) || defined (STM32F070xB) || defined (STM32F030xC) || defined (STM32G030xx) 
    #if (_USE_GPIOA)
    RCC->AHBENR |= RCC_AHBPeriph_GPIOA;
    #elif (_USE_GPIOB)
    RCC->AHBENR |= RCC_AHBPeriph_GPIOB;
    #elif (_USE_GPIOC)
    RCC->AHBENR |= RCC_AHBPeriph_GPIOC;
    #elif (_USE_GPIOD)
    RCC->AHBENR |= RCC_AHBPeriph_GPIOD;
    #else
        #error "_USE_GPIOx not defined."
    #endif

    // f4, gpio on ahb1
#elif defined (STM32F401xx) || defined (STM32F40_41xxx)
    #if (_USE_GPIOA)
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOA;
    #elif (_USE_GPIOB)
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOB;
    #elif (_USE_GPIOC)
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOC;
    #elif (_USE_GPIOD)
    RCC->AHB1ENR |= RCC_AHB1Periph_GPIOD;
    #else
        #error "_USE_GPIOx not defined."    
    #endif

    // f1, gpio on apb2
#elif defined (STM32F10X_HD) || defined (STM32F10X_MD_VL)
    #if (_USE_GPIOA)
    RCC->APB2ENR |= RCC_APB2Periph_GPIOA;
    #elif (_USE_GPIOB)
    RCC->APB2ENR |= RCC_APB2Periph_GPIOB;
    #elif (_USE_GPIOC)
    RCC->APB2ENR |= RCC_APB2Periph_GPIOC;
    #elif (_USE_GPIOD)
    RCC->APB2ENR |= RCC_APB2Periph_GPIOD;
    #else
        #error "_USE_GPIOx not defined."    
    #endif
    RCC->APB2ENR |= RCC_APB2Periph_AFIO;    // enable afio
    #ifdef _GPIO_PIN_REMAP
    GPIO_PinRemapConfig(_GPIO_PIN_REMAP, ENABLE);
    #endif

    // gd32f130/150, 330/350
#elif defined (GD32F350) || defined (GD32F130_150) || defined (GD32F330)
    #if (_USE_GPIOA)
    RCU_REG_VAL(RCU_GPIOA) |= BIT(RCU_BIT_POS(RCU_GPIOA));
    #elif (_USE_GPIOB)
    RCU_REG_VAL(RCU_GPIOB) |= BIT(RCU_BIT_POS(RCU_GPIOB));
    #elif (_USE_GPIOC)
    RCU_REG_VAL(RCU_GPIOC) |= BIT(RCU_BIT_POS(RCU_GPIOC));
    #elif (_USE_GPIOD)
    RCU_REG_VAL(RCU_GPIOD) |= BIT(RCU_BIT_POS(RCU_GPIOD));
    #else
        #error "_USE_GPIOx not defined."    
    #endif
#endif

// gpio config
#if defined (STM32F303xC) || defined (STM32F072) || defined (STM32F042) || defined (STM32F030) || defined (STM32F051) || defined (STM32F070xB) || defined (STM32F401xx) || defined (STM32F40_41xxx) || defined (STM32F030xC) || defined (STM32G030xx)
    GPIOx->OSPEEDR |= (GPIO_Speed_50MHz << (_USART_TXPIN * 2)) | (GPIO_Speed_50MHz << (_USART_RXPIN * 2));
    GPIOx->OTYPER |= (GPIO_OType_PP << (_USART_TXPIN * 2)) | (GPIO_OType_PP << (_USART_RXPIN * 2));
    GPIOx->MODER |= (GPIO_Mode_AF << (_USART_TXPIN * 2)) | (GPIO_Mode_AF << (_USART_RXPIN * 2));
//    GPIOx->MODER |= (GPIO_Mode_OUT << (_USART_TXPIN * 2)) | (GPIO_Mode_OUT << (_USART_RXPIN * 2));
    GPIOx->PUPDR |= (GPIO_PuPd_NOPULL << (_USART_TXPIN * 2)) | (GPIO_PuPd_NOPULL << (_USART_RXPIN * 2));
//    while(1) { GPIOx->ODR ^= _USART_TXPIN; }  // debug

    #if (_USART_TXPIN / 8)  // RXPIN, 8-15
    GPIOx->AFR[1] &= ~(0xf << ((_USART_TXPIN - 8) * 4));
    GPIOx->AFR[1] |= (_GPIO_AF_TXPIN << ((_USART_TXPIN - 8) * 4));
    #else
    GPIOx->AFR[0] &= ~(0xf << ((_USART_TXPIN) * 4));
    GPIOx->AFR[0] |= (_GPIO_AF_TXPIN << ((_USART_TXPIN) * 4));
    #endif
    #if (_USART_RXPIN / 8)  // RXPIN, 8-15
    GPIOx->AFR[1] &= ~(0xf << ((_USART_RXPIN - 8) * 4));
    GPIOx->AFR[1] |= (_GPIO_AF_RXPIN << ((_USART_RXPIN - 8) * 4));
    #else
    GPIOx->AFR[0] &= ~(0xf << ((_USART_RXPIN) * 4));
    GPIOx->AFR[0] |= (_GPIO_AF_RXPIN << ((_USART_RXPIN) * 4));
    #endif
#elif defined (STM32F10X_HD) || defined (STM32F10X_MD_VL)
    // there may be some additional pin map configurations here!!!
    #if (_USART_TXPIN / 8)  // TXPIN, 8-15
    GPIOx->CRH &= ~(0xf << ((_USART_TXPIN - 8) * 4));
    GPIOx->CRH |= (((GPIO_Mode_AF_PP & 0xf) | GPIO_Speed_50MHz) << ((_USART_TXPIN - 8) * 4));
    #else   // 0-7
    GPIOx->CRL &= ~(0xf << ((_USART_TXPIN) * 4));
    GPIOx->CRL |= (((GPIO_Mode_AF_PP & 0xf) | GPIO_Speed_50MHz) << ((_USART_TXPIN) * 4));
    #endif

    #if (_USART_RXPIN / 8)  // RXPIN, 8-15
    GPIOx->CRH &= ~(0xf << ((_USART_RXPIN - 8) * 4));
    GPIOx->CRH |= ((GPIO_Mode_IPU & 0xf) << ((_USART_RXPIN - 8) * 4));
    #else
    GPIOx->CRL &= ~(0xf << ((_USART_RXPIN) * 4));
    GPIOx->CRL |= ((GPIO_Mode_IPU & 0xf) << ((_USART_RXPIN) *4));
    #endif

#elif defined (GD32F350) || defined (GD32F130_150) || defined (GD32F330)
    #if (_USART_TXPIN / 8)  // TXPIN, 8-15
    GPIO_AFSEL1(GPIOx) &= ~(0xf << ((_USART_TXPIN - 8) * 4));
    GPIO_AFSEL1(GPIOx) |= (_GPIO_AF_TXPIN << ((_USART_TXPIN - 8) * 4));
    #else   // 0-7
    GPIO_AFSEL0(GPIOx) &= ~(0xf << ((_USART_TXPIN) * 4));
    GPIO_AFSEL0(GPIOx) |= (_GPIO_AF_TXPIN << ((_USART_TXPIN) * 4));
    #endif

    #if (_USART_RXPIN / 8)  // RXPIN, 8-15
    GPIO_AFSEL1(GPIOx) &= ~(0xf << ((_USART_RXPIN - 8) * 4));
    GPIO_AFSEL1(GPIOx) |= (_GPIO_AF_RXPIN << ((_USART_RXPIN - 8) * 4));
    #else   // 0-7
    GPIO_AFSEL0(GPIOx) &= ~(0xf << ((_USART_RXPIN) * 4));
    GPIO_AFSEL0(GPIOx) |= (_GPIO_AF_RXPIN << ((_USART_RXPIN) * 4));
    #endif

    GPIO_CTL(GPIOx) |= (GPIO_MODE_AF << (_USART_TXPIN * 2)) | (GPIO_MODE_AF << (_USART_RXPIN * 2));
    GPIO_PUD(GPIOx) |= (GPIO_PUPD_PULLUP << (_USART_TXPIN * 2)) | (GPIO_PUPD_PULLUP << (_USART_RXPIN * 2));
    GPIO_OMODE(GPIOx) |= (_USART_TXPIN);

    #if defined (GD32F130_150)
    GPIO_OSPD(GPIOx) |= (GPIO_OSPEED_10MHZ << (_USART_TXPIN * 2));
    #else
    GPIO_OSPD0(GPIOx) |= (GPIO_OSPEED_10MHZ << (_USART_TXPIN * 2));
    #endif
#endif
}

void USART_Config(void) 
{
    RCC_GPIO_Config();

#if (defined (GD32F350) || defined (GD32F130_150) || defined (GD32F330))
    usart_deinit(USARTx); /* USART configure */
    USART_CTL0 (USARTx) = 0x0000000c;
    USART_CTL1 (USARTx) = 0x00000000;
    USART_BAUD (USARTx) = 0x00000010;
    USART_CTL0(USARTx) |= USART_CTL0_UEN; //usart_enable(USARTx);
#else

    USARTx->CR1 &= ~USART_CR1_UE; // stop
    USARTx->CR1 |= (USART_Mode_Tx | USART_Mode_Rx);

    #if !defined (STM32F401xx) && !defined(STM32F40_41xxx)
        #if (_USE_PLL == 0)
            USARTx->BRR = 16; // baudrate could only be 500000 if pll disabled
        #else
            USARTx->BRR = 24000000UL / _BAUDRATE;
        #endif
    #else
        USARTx->BRR = 32; // 16M / 32 = 500k
    #endif

    #if !defined (STM32F10X_HD) && !defined (STM32F401xx) && !defined (STM32F10X_MD_VL) && !defined (STM32F40_41xxx)
        #if (_USART_PIN_SWAP)
        USARTx->CR2 |= USART_CR2_SWAP;
        #endif
    #endif
    USARTx->CR1 |= USART_CR1_UE;
#endif

    g.nocomm = 1;
    xdev_out(uputc);
// while(1) { xprintf("Hello, world.\n"); ( {  for(volatile int i = 0; i < 100000UL; i++);}); }	// for debug
}

int USART_NoComm(void) {
    return g.nocomm;
}

void USART_Poll(void) {
    if (usart_flag_get1(USARTx, USART_FLAG_RXNE)) {
        if (g.size < MAX_LEN) {
#if defined(GD32F350) || defined (GD32F130_150) || defined (GD32F330)
            g.msg[g.size] = USART_RDATA(USARTx);
#elif defined(STM32F303xC) || defined (STM32F072) || defined (STM32F030) || defined (STM32F042) || defined (STM32F051) || defined (STM32F070xB) || defined (STM32F030xC) || defined (STM32G030xx)
            g.msg[g.size] = USARTx->RDR;
#elif defined(STM32F10X_HD) || defined (STM32F401xx) || defined (STM32F10X_MD_VL) || defined (STM32F40_41xxx)
            g.msg[g.size] = USARTx->DR;
#endif
            g.size++;
        }
        usart_flag_clear1(USARTx, USART_FLAG_RXNE);
    }
    if (usart_flag_get1(USARTx, USART_FLAG_IDLE)) {
        if(0) {
        }
#if _USE_CLI
//        else if (strncasecmp((char*) g.msg, "##", 2) == 0) {
        else if(g.msg[0] == '#' && g.msg[1] == '#') {
            CLI_Parse(g.msg, g.size);
            g.nocomm = 0;
        }
#endif
        else if (g.msg[0] >= 0x80) {
            IAP_Parse(g.msg, g.size);
            g.nocomm = 0;
        }
        g.size = 0;
#if defined(STM32F10X_HD) || defined (STM32F10X_MD_VL) || defined (STM32F401xx) || defined (STM32F40_41xxx)
        volatile unsigned long tmp = tmp;
        tmp = USARTx->SR;
        tmp = USARTx->DR;
#else
        usart_flag_clear1(USARTx, USART_FLAG_IDLE);
#endif
    }
    if (usart_flag_get1(USARTx, USART_FLAG_ORE)) {
#if defined(STM32F10X_HD) || defined (STM32F10X_MD_VL) || defined (STM32F401xx) || defined (STM32F40_41xxx)
        volatile unsigned long tmp = tmp;
        tmp = USARTx->SR;
        tmp = USARTx->DR;
#else
        usart_flag_clear1(USARTx, USART_FLAG_ORE);
#endif
    }
    if (usart_flag_get1(USARTx, USART_FLAG_FE)) {
#if defined(STM32F10X_HD) || defined (STM32F10X_MD_VL) || defined (STM32F401xx) || defined (STM32F40_41xxx)
        volatile unsigned long tmp = tmp;
        tmp = USARTx->SR;
        tmp = USARTx->DR;
#else
        usart_flag_clear1(USARTx, USART_FLAG_FE);
#endif
    }
}

void uwrite(const void *data, int size) {
    while (size--)
        uputc(*(unsigned char*) data++);
}

void uputc(unsigned char c) {
    ( {  while(RESET == usart_flag_get1(USARTx, USART_FLAG_TXE));});
#if defined (GD32F350) || defined (GD32F130_150) || defined (GD32F330)
    USART_TDATA (USARTx) = (USART_TDATA_TDATA & c);
#elif defined (STM32F303xC) || defined (STM32F072) || defined (STM32F030) || defined (STM32F042) || defined (STM32F051) || defined (STM32F070xB) || defined (STM32F030xC) || defined (STM32G030xx)
    USARTx->TDR = c;
#elif defined (STM32F10X_HD) || defined (STM32F401xx) || defined (STM32F10X_MD_VL) || defined (STM32F40_41xxx)
    USARTx->DR = c;
#endif
    ( {  while(RESET == usart_flag_get1(USARTx, USART_FLAG_TC));});
}
