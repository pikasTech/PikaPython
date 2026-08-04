#include "main.h"
#include "gpio.h"
#include "pikapython_modules.h"
#include "pika_platform.h"
#include "usart.h"

static void SystemClock_Config(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART1_UART_Init();
    if (pikaPythonInit() != PIKA_STATUS_OK) {
        Error_Handler();
    }
    for (;;) {
    }
}

PikaStatus pika_platform_write(const char* data, size_t size) {
    if (data == NULL || size > UINT16_MAX) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (HAL_UART_Transmit(&huart1, (uint8_t*)data, (uint16_t)size,
                          HAL_MAX_DELAY) != HAL_OK) {
        return PIKA_STATUS_OUTPUT_ERROR;
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_platform_repl_read(
    uint8_t* data,
    size_t capacity,
    size_t* received) {
    if (data == NULL || received == NULL || capacity == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *received = 0u;
    if (HAL_UART_Receive(
            &huart1, data, 1u, HAL_MAX_DELAY) != HAL_OK) {
        return PIKA_STATUS_IO_ERROR;
    }
    *received = 1u;
    return PIKA_STATUS_OK;
}

static void SystemClock_Config(void) {
    RCC_OscInitTypeDef oscillator = {0};
    RCC_ClkInitTypeDef clock = {0};

    oscillator.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    oscillator.HSEState = RCC_HSE_ON;
    oscillator.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    oscillator.PLL.PLLState = RCC_PLL_ON;
    oscillator.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    oscillator.PLL.PLLMUL = RCC_PLL_MUL9;
    if (HAL_RCC_OscConfig(&oscillator) != HAL_OK) {
        Error_Handler();
    }

    clock.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                      RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    clock.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    clock.AHBCLKDivider = RCC_SYSCLK_DIV1;
    clock.APB1CLKDivider = RCC_HCLK_DIV2;
    clock.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&clock, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
}

void Error_Handler(void) {
    for (;;) {
    }
}
