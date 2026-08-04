#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "pika_capability_config.h"
#include "pika_platform.h"

#if PIKA_CAPABILITY_MACHINE_PIN_ENABLE || \
    PIKA_CAPABILITY_MACHINE_ADC_ENABLE || \
    PIKA_CAPABILITY_MACHINE_PWM_ENABLE
#include "machine_platform.h"
#endif

#define PIKA_REG32(address) (*(volatile uint32_t*)(address))
#define PIKA_RCC_APB2ENR PIKA_REG32(0x40021018u)
#define PIKA_RCC_APB1ENR PIKA_REG32(0x4002101cu)
#define PIKA_GPIOA_CRL PIKA_REG32(0x40010800u)
#define PIKA_GPIOA_IDR PIKA_REG32(0x40010808u)
#define PIKA_GPIOA_ODR PIKA_REG32(0x4001080cu)
#define PIKA_GPIOA_BSRR PIKA_REG32(0x40010810u)
#define PIKA_GPIOA_BRR PIKA_REG32(0x40010814u)
#define PIKA_GPIOC_CRH PIKA_REG32(0x40011004u)
#define PIKA_GPIOC_IDR PIKA_REG32(0x40011008u)
#define PIKA_GPIOC_ODR PIKA_REG32(0x4001100cu)
#define PIKA_GPIOC_BSRR PIKA_REG32(0x40011010u)
#define PIKA_GPIOC_BRR PIKA_REG32(0x40011014u)
#define PIKA_ADC1_SR PIKA_REG32(0x40012400u)
#define PIKA_ADC1_CR2 PIKA_REG32(0x40012408u)
#define PIKA_ADC1_SQR1 PIKA_REG32(0x4001242cu)
#define PIKA_ADC1_SQR3 PIKA_REG32(0x40012434u)
#define PIKA_ADC1_DR PIKA_REG32(0x4001244cu)
#define PIKA_TIM2_CR1 PIKA_REG32(0x40000000u)
#define PIKA_TIM2_EGR PIKA_REG32(0x40000014u)
#define PIKA_TIM2_CCMR1 PIKA_REG32(0x40000018u)
#define PIKA_TIM2_CCER PIKA_REG32(0x40000020u)
#define PIKA_TIM2_PSC PIKA_REG32(0x40000028u)
#define PIKA_TIM2_ARR PIKA_REG32(0x4000002cu)
#define PIKA_TIM2_CCR1 PIKA_REG32(0x40000034u)
#define PIKA_TIM2_CCR2 PIKA_REG32(0x40000038u)
#define PIKA_USART2_SR PIKA_REG32(0x40004400u)
#define PIKA_USART2_DR PIKA_REG32(0x40004404u)
#define PIKA_USART2_BRR PIKA_REG32(0x40004408u)
#define PIKA_USART2_CR1 PIKA_REG32(0x4000440cu)

extern uint8_t __heap_start;
extern uint8_t __heap_end;
extern uint8_t __stack_bottom;
extern uint8_t __stack_top;
extern int pika_semihost_call(int operation, const void* argument);

static uint8_t* pika_heap_cursor = &__heap_start;
static uint32_t pika_heap_peak;

#if PIKA_CAPABILITY_MACHINE_PIN_ENABLE

typedef struct {
    int64_t identifier;
    PikaMachinePinMode mode;
    uint8_t active;
} PikaF103Pin;

static PikaF103Pin pika_f103_pin;

static PikaStatus pin_validate(
    const PikaMachinePinConfig* config) {
    if (config == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (config->identifier != 0 &&
        config->identifier != 2) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->mode != PIKA_MACHINE_PIN_MODE_IN &&
        config->mode != PIKA_MACHINE_PIN_MODE_OUT &&
        config->mode != PIKA_MACHINE_PIN_MODE_OPEN_DRAIN) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    if (config->pull != PIKA_MACHINE_PIN_PULL_NONE &&
        config->pull != PIKA_MACHINE_PIN_PULL_UP &&
        config->pull != PIKA_MACHINE_PIN_PULL_DOWN) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    return PIKA_STATUS_OK;
}

static void pin_write_level(
    int64_t identifier,
    uint8_t level) {
    if (identifier == 0) {
        if (level != 0u) {
            PIKA_GPIOA_BSRR = 1u;
        } else {
            PIKA_GPIOA_BRR = 1u;
        }
    } else if (level != 0u) {
        PIKA_GPIOC_BSRR = 1u << 12;
    } else {
        PIKA_GPIOC_BRR = 1u << 12;
    }
}

static PikaStatus pin_configure(
    PikaF103Pin* pin,
    const PikaMachinePinConfig* config) {
    uint32_t configuration;
    uint32_t mode_bits;
    PikaStatus status;
    if (pin == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    status = pin_validate(config);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    if (pin->active != 0u &&
        pin->identifier != config->identifier) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    PIKA_RCC_APB2ENR |=
        config->identifier == 0 ? 1u << 2 : 1u << 4;
    if (config->mode == PIKA_MACHINE_PIN_MODE_IN) {
        if (config->pull == PIKA_MACHINE_PIN_PULL_NONE) {
            mode_bits = 0x4u;
        } else {
            mode_bits = 0x8u;
            pin_write_level(
                config->identifier,
                config->pull == PIKA_MACHINE_PIN_PULL_UP ? 1u : 0u
            );
        }
    } else {
        if (config->initial_value_valid != 0u) {
            pin_write_level(
                config->identifier, config->initial_value);
        }
        mode_bits =
            config->mode == PIKA_MACHINE_PIN_MODE_OPEN_DRAIN
                ? 0x6u
                : 0x2u;
    }
    if (config->identifier == 0) {
        configuration = PIKA_GPIOA_CRL;
        configuration &= ~0xfu;
        configuration |= mode_bits;
        PIKA_GPIOA_CRL = configuration;
    } else {
        configuration = PIKA_GPIOC_CRH;
        configuration &= ~(0xfu << 16);
        configuration |= mode_bits << 16;
        PIKA_GPIOC_CRH = configuration;
    }
    pin->identifier = config->identifier;
    pin->mode = config->mode;
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pin_open(
    const PikaMachinePinConfig* config,
    void** handle) {
    PikaStatus status;
    if (handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *handle = NULL;
    if (pika_f103_pin.active != 0u) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    status = pin_configure(&pika_f103_pin, config);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    pika_f103_pin.active = 1u;
    *handle = &pika_f103_pin;
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pin_configure(
    void* handle,
    const PikaMachinePinConfig* config) {
    if (handle != &pika_f103_pin ||
        pika_f103_pin.active == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return pin_configure(&pika_f103_pin, config);
}

PikaStatus pika_machine_platform_pin_read(
    void* handle,
    int* value) {
    uint32_t levels;
    if (handle != &pika_f103_pin ||
        pika_f103_pin.active == 0u || value == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (pika_f103_pin.identifier == 0) {
        levels = pika_f103_pin.mode == PIKA_MACHINE_PIN_MODE_IN
                     ? PIKA_GPIOA_IDR
                     : PIKA_GPIOA_ODR;
        *value = (levels & 1u) != 0u ? 1 : 0;
    } else {
        levels = pika_f103_pin.mode == PIKA_MACHINE_PIN_MODE_IN
                     ? PIKA_GPIOC_IDR
                     : PIKA_GPIOC_ODR;
        *value = (levels & (1u << 12)) != 0u ? 1 : 0;
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pin_write(
    void* handle,
    int value) {
    if (handle != &pika_f103_pin ||
        pika_f103_pin.active == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (pika_f103_pin.mode == PIKA_MACHINE_PIN_MODE_IN ||
        (value != 0 && value != 1)) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    pin_write_level(
        pika_f103_pin.identifier, value != 0 ? 1u : 0u);
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pin_close(void* handle) {
    uint32_t configuration;
    if (handle != &pika_f103_pin ||
        pika_f103_pin.active == 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (pika_f103_pin.identifier == 0) {
        configuration = PIKA_GPIOA_CRL;
        configuration &= ~0xfu;
        configuration |= 0x4u;
        PIKA_GPIOA_CRL = configuration;
    } else {
        configuration = PIKA_GPIOC_CRH;
        configuration &= ~(0xfu << 16);
        configuration |= 0x4u << 16;
        PIKA_GPIOC_CRH = configuration;
    }
    pika_f103_pin.active = 0u;
    pika_f103_pin.identifier = 0;
    pika_f103_pin.mode = PIKA_MACHINE_PIN_MODE_IN;
    return PIKA_STATUS_OK;
}

#endif

#if PIKA_CAPABILITY_MACHINE_ADC_ENABLE

#ifndef PIKA_F103_ADC_OBJECT_LIMIT
#define PIKA_F103_ADC_OBJECT_LIMIT 4u
#endif

#ifndef PIKA_F103_ADC_POLL_LIMIT
#define PIKA_F103_ADC_POLL_LIMIT 1000000u
#endif

#if PIKA_F103_ADC_OBJECT_LIMIT < 1u
#error "PIKA_F103_ADC_OBJECT_LIMIT must be at least 1"
#endif

#if PIKA_F103_ADC_POLL_LIMIT < 1u
#error "PIKA_F103_ADC_POLL_LIMIT must be at least 1"
#endif

#define PIKA_ADC_SR_EOC (1u << 1)
#define PIKA_ADC_CR2_SWSTART (1u << 22)
#define PIKA_ADC_CR2_EXTTRIG (1u << 20)
#define PIKA_ADC_CR2_EXTSEL_MASK (7u << 17)
#define PIKA_ADC_CR2_EXTSEL_SOFTWARE (7u << 17)
#define PIKA_ADC_CR2_ALIGN (1u << 11)
#define PIKA_ADC_CR2_CAL (1u << 2)
#define PIKA_ADC_CR2_RSTCAL (1u << 3)
#define PIKA_ADC_CR2_CONT (1u << 1)
#define PIKA_ADC_CR2_ADON 1u

typedef struct {
    uint8_t channel;
    uint8_t active;
} PikaF103Adc;

static PikaF103Adc
    pika_f103_adc_objects[PIKA_F103_ADC_OBJECT_LIMIT];

static int adc_any_active(void) {
    uint32_t index;
    for (index = 0u;
         index < (uint32_t)PIKA_F103_ADC_OBJECT_LIMIT;
         ++index) {
        if (pika_f103_adc_objects[index].active != 0u) {
            return 1;
        }
    }
    return 0;
}

static PikaF103Adc* adc_allocate(void) {
    uint32_t index;
    for (index = 0u;
         index < (uint32_t)PIKA_F103_ADC_OBJECT_LIMIT;
         ++index) {
        if (pika_f103_adc_objects[index].active == 0u) {
            pika_f103_adc_objects[index].channel = 0u;
            pika_f103_adc_objects[index].active = 1u;
            return &pika_f103_adc_objects[index];
        }
    }
    return NULL;
}

static PikaF103Adc* adc_find(void* handle) {
    uint32_t index;
    for (index = 0u;
         index < (uint32_t)PIKA_F103_ADC_OBJECT_LIMIT;
         ++index) {
        if (pika_f103_adc_objects[index].active != 0u &&
            handle == &pika_f103_adc_objects[index]) {
            return &pika_f103_adc_objects[index];
        }
    }
    return NULL;
}

static void adc_configure_gpio(uint8_t channel) {
    uint32_t configuration = PIKA_GPIOA_CRL;
    uint32_t shift = (uint32_t)channel * 4u;
    configuration &= ~(0xfu << shift);
    PIKA_GPIOA_CRL = configuration;
}

static PikaStatus adc_wait_clear(uint32_t mask) {
    uint32_t poll;
    for (poll = 0u;
         poll < (uint32_t)PIKA_F103_ADC_POLL_LIMIT;
         ++poll) {
        if ((PIKA_ADC1_CR2 & mask) == 0u) {
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_IO_ERROR;
}

static PikaStatus adc_enable(void) {
    PikaStatus status;
    PIKA_RCC_APB2ENR |= (1u << 2) | (1u << 9);
    PIKA_ADC1_CR2 = PIKA_ADC_CR2_ADON;
    PIKA_ADC1_CR2 |= PIKA_ADC_CR2_RSTCAL;
    status = adc_wait_clear(PIKA_ADC_CR2_RSTCAL);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    PIKA_ADC1_CR2 |= PIKA_ADC_CR2_CAL;
    return adc_wait_clear(PIKA_ADC_CR2_CAL);
}

PikaStatus pika_machine_platform_adc_open(
    const PikaMachineAdcConfig* config,
    void** handle) {
    PikaF103Adc* object;
    PikaStatus status;
    int first;
    if (handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *handle = NULL;
    if (config == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (config->identifier < 0 ||
        config->identifier > 7) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    first = !adc_any_active();
    object = adc_allocate();
    if (object == NULL) {
        return PIKA_STATUS_STORAGE_TOO_SMALL;
    }
    object->channel = (uint8_t)config->identifier;
    adc_configure_gpio(object->channel);
    if (first) {
        status = adc_enable();
        if (status != PIKA_STATUS_OK) {
            object->active = 0u;
            return status;
        }
    }
    *handle = object;
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_adc_read(
    void* handle,
    uint32_t* raw_value,
    uint8_t* resolution_bits) {
    PikaF103Adc* object = adc_find(handle);
    uint32_t control;
    uint32_t poll;
    if (object == NULL || raw_value == NULL ||
        resolution_bits == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    adc_configure_gpio(object->channel);
    PIKA_ADC1_SQR1 &= ~(0xfu << 20);
    PIKA_ADC1_SQR3 =
        (PIKA_ADC1_SQR3 & ~0x1fu) | object->channel;
    control = PIKA_ADC1_CR2;
    control &= ~(PIKA_ADC_CR2_CONT | PIKA_ADC_CR2_ALIGN |
                 PIKA_ADC_CR2_EXTSEL_MASK);
    control |= PIKA_ADC_CR2_ADON | PIKA_ADC_CR2_EXTTRIG |
               PIKA_ADC_CR2_EXTSEL_SOFTWARE;
    PIKA_ADC1_CR2 = control;
    PIKA_ADC1_CR2 = control | PIKA_ADC_CR2_SWSTART;
    for (poll = 0u;
         poll < (uint32_t)PIKA_F103_ADC_POLL_LIMIT;
         ++poll) {
        if ((PIKA_ADC1_SR & PIKA_ADC_SR_EOC) != 0u) {
            *raw_value = PIKA_ADC1_DR & 0xfffu;
            *resolution_bits = 12u;
            return PIKA_STATUS_OK;
        }
    }
    return PIKA_STATUS_IO_ERROR;
}

PikaStatus pika_machine_platform_adc_read_uv(
    void* handle,
    int64_t* microvolts) {
    if (adc_find(handle) == NULL || microvolts == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    return PIKA_STATUS_UNSUPPORTED_CAPABILITY;
}

PikaStatus pika_machine_platform_adc_close(void* handle) {
    PikaF103Adc* object = adc_find(handle);
    if (object == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    object->active = 0u;
    object->channel = 0u;
    if (!adc_any_active()) {
        PIKA_ADC1_CR2 = 0u;
        PIKA_RCC_APB2ENR &= ~(1u << 9);
    }
    return PIKA_STATUS_OK;
}

#endif

#if PIKA_CAPABILITY_MACHINE_PWM_ENABLE

#define PIKA_F103_PWM_CHANNEL_COUNT 2u
#define PIKA_F103_PWM_TIMER_CLOCK_HZ UINT32_C(8000000)
#define PIKA_F103_PWM_NANOSECONDS_PER_SECOND \
    UINT64_C(1000000000)
#define PIKA_F103_PWM_PERIOD_LIMIT UINT32_C(65536)
#define PIKA_F103_PWM_TRACE_RATIO_1KHZ (1u << 0)
#define PIKA_F103_PWM_TRACE_SHARED_2KHZ (1u << 1)
#define PIKA_F103_PWM_TRACE_FINAL_DISABLED (1u << 2)
#define PIKA_F103_PWM_TRACE_REQUIRED \
    (PIKA_F103_PWM_TRACE_RATIO_1KHZ | \
     PIKA_F103_PWM_TRACE_SHARED_2KHZ | \
     PIKA_F103_PWM_TRACE_FINAL_DISABLED)
#define PIKA_RCC_APB1ENR_TIM2EN (1u << 0)
#define PIKA_RCC_APB2ENR_IOPAEN (1u << 2)
#define PIKA_TIM_CR1_CEN (1u << 0)
#define PIKA_TIM_CR1_ARPE (1u << 7)
#define PIKA_TIM_EGR_UG (1u << 0)
#define PIKA_TIM_CCMR_PWM1_CHANNEL_1 \
    ((6u << 4) | (1u << 3))
#define PIKA_TIM_CCMR_PWM1_CHANNEL_2 \
    ((6u << 12) | (1u << 11))
#define PIKA_TIM_CCMR_PWM1_BOTH \
    (PIKA_TIM_CCMR_PWM1_CHANNEL_1 | \
     PIKA_TIM_CCMR_PWM1_CHANNEL_2)
#define PIKA_TIM_CCER_CC1E (1u << 0)
#define PIKA_TIM_CCER_CC2E (1u << 4)
#define PIKA_TIM_CCER_PWM_ENABLE_MASK \
    (PIKA_TIM_CCER_CC1E | PIKA_TIM_CCER_CC2E)

typedef struct {
    uint16_t duty_u16;
    uint64_t duty_ns;
    PikaMachinePwmDutyMode duty_mode;
    uint8_t allocated;
    uint8_t enabled;
} PikaF103Pwm;

static PikaF103Pwm
    pika_f103_pwm_channels[PIKA_F103_PWM_CHANNEL_COUNT];
static uint32_t pika_f103_pwm_frequency_hz;
static uint32_t pika_f103_pwm_trace;

static PikaF103Pwm* pwm_find(
    void* handle,
    uint32_t* channel_index) {
    uint32_t index;
    for (index = 0u;
         index < (uint32_t)PIKA_F103_PWM_CHANNEL_COUNT;
         ++index) {
        if (pika_f103_pwm_channels[index].allocated != 0u &&
            handle == &pika_f103_pwm_channels[index]) {
            if (channel_index != NULL) {
                *channel_index = index;
            }
            return &pika_f103_pwm_channels[index];
        }
    }
    return NULL;
}

static int pwm_any_allocated(void) {
    uint32_t index;
    for (index = 0u;
         index < (uint32_t)PIKA_F103_PWM_CHANNEL_COUNT;
         ++index) {
        if (pika_f103_pwm_channels[index].allocated != 0u) {
            return 1;
        }
    }
    return 0;
}

static PikaStatus pwm_timer_period(
    uint32_t frequency_hz,
    uint16_t* prescaler,
    uint16_t* auto_reload,
    uint32_t* period_counts) {
    uint64_t denominator;
    uint64_t divider;
    uint64_t timer_hz;
    uint64_t counts;
    if (frequency_hz == 0u ||
        frequency_hz > PIKA_F103_PWM_TIMER_CLOCK_HZ ||
        prescaler == NULL || auto_reload == NULL ||
        period_counts == NULL) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    denominator =
        (uint64_t)frequency_hz *
        (uint64_t)PIKA_F103_PWM_PERIOD_LIMIT;
    divider =
        ((uint64_t)PIKA_F103_PWM_TIMER_CLOCK_HZ +
         denominator - UINT64_C(1)) /
        denominator;
    if (divider == 0u) {
        divider = 1u;
    }
    if (divider > (uint64_t)PIKA_F103_PWM_PERIOD_LIMIT) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    timer_hz =
        (uint64_t)PIKA_F103_PWM_TIMER_CLOCK_HZ / divider;
    counts = timer_hz / (uint64_t)frequency_hz;
    if (counts == 0u ||
        counts > (uint64_t)PIKA_F103_PWM_PERIOD_LIMIT) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    *prescaler = (uint16_t)(divider - UINT64_C(1));
    *auto_reload = (uint16_t)(counts - UINT64_C(1));
    *period_counts = (uint32_t)counts;
    return PIKA_STATUS_OK;
}

static uint32_t pwm_compare_value(
    const PikaF103Pwm* pwm,
    uint32_t period_counts,
    uint64_t period_ns) {
    uint64_t compare = 0u;
    if (pwm->duty_mode == PIKA_MACHINE_PWM_DUTY_U16) {
        compare =
            ((uint64_t)period_counts *
                 (uint64_t)pwm->duty_u16 +
             UINT64_C(32767)) /
            UINT64_C(65535);
    } else if (
        pwm->duty_mode == PIKA_MACHINE_PWM_DUTY_NS &&
        period_ns != 0u) {
        compare =
            ((uint64_t)period_counts * pwm->duty_ns +
             period_ns / UINT64_C(2)) /
            period_ns;
    }
    if (compare > (uint64_t)period_counts) {
        compare = (uint64_t)period_counts;
    }
    return (uint32_t)compare;
}

static void pwm_configure_gpio(uint32_t channel_index) {
    uint32_t configuration = PIKA_GPIOA_CRL;
    uint32_t shift = channel_index * 4u;
    PIKA_RCC_APB2ENR |= PIKA_RCC_APB2ENR_IOPAEN;
    configuration &= ~(0xfu << shift);
    configuration |= 0xau << shift;
    PIKA_GPIOA_CRL = configuration;
}

static void pwm_release_gpio(uint32_t channel_index) {
    uint32_t configuration = PIKA_GPIOA_CRL;
    uint32_t shift = channel_index * 4u;
    configuration &= ~(0xfu << shift);
    configuration |= 0x4u << shift;
    PIKA_GPIOA_CRL = configuration;
}

static void pwm_record_trace(void) {
    uint32_t enabled =
        PIKA_TIM2_CCER & PIKA_TIM_CCER_PWM_ENABLE_MASK;
    if (pika_f103_pwm_frequency_hz == 1000u &&
        pika_f103_pwm_channels[0].allocated != 0u &&
        pika_f103_pwm_channels[0].enabled != 0u &&
        pika_f103_pwm_channels[0].duty_mode ==
            PIKA_MACHINE_PWM_DUTY_U16 &&
        pika_f103_pwm_channels[0].duty_u16 == 32768u &&
        PIKA_TIM2_PSC == 0u && PIKA_TIM2_ARR == 7999u &&
        PIKA_TIM2_CCR1 == 4000u &&
        PIKA_TIM2_CCMR1 == PIKA_TIM_CCMR_PWM1_BOTH &&
        (enabled & PIKA_TIM_CCER_CC1E) != 0u) {
        pika_f103_pwm_trace |=
            PIKA_F103_PWM_TRACE_RATIO_1KHZ;
    }
    if (pika_f103_pwm_frequency_hz == 2000u &&
        pika_f103_pwm_channels[0].allocated != 0u &&
        pika_f103_pwm_channels[1].allocated != 0u &&
        pika_f103_pwm_channels[1].duty_mode ==
            PIKA_MACHINE_PWM_DUTY_NS &&
        pika_f103_pwm_channels[1].duty_ns ==
            UINT64_C(250000) &&
        PIKA_TIM2_PSC == 0u && PIKA_TIM2_ARR == 3999u &&
        PIKA_TIM2_CCR1 == 2000u &&
        PIKA_TIM2_CCR2 == 2000u &&
        PIKA_TIM2_CCMR1 == PIKA_TIM_CCMR_PWM1_BOTH &&
        enabled == PIKA_TIM_CCER_PWM_ENABLE_MASK) {
        pika_f103_pwm_trace |=
            PIKA_F103_PWM_TRACE_SHARED_2KHZ;
    }
}

static PikaStatus pwm_refresh_hardware(void) {
    uint16_t prescaler;
    uint16_t auto_reload;
    uint32_t period_counts;
    uint64_t period_ns;
    uint32_t enabled = 0u;
    PikaStatus status;
    if (pika_f103_pwm_frequency_hz == 0u) {
        PIKA_TIM2_CR1 &= ~PIKA_TIM_CR1_CEN;
        PIKA_TIM2_CCER &= ~PIKA_TIM_CCER_PWM_ENABLE_MASK;
        PIKA_TIM2_CCR1 = 0u;
        PIKA_TIM2_CCR2 = 0u;
        return PIKA_STATUS_OK;
    }
    status = pwm_timer_period(
        pika_f103_pwm_frequency_hz,
        &prescaler,
        &auto_reload,
        &period_counts);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    period_ns =
        PIKA_F103_PWM_NANOSECONDS_PER_SECOND /
        (uint64_t)pika_f103_pwm_frequency_hz;
    PIKA_RCC_APB1ENR |= PIKA_RCC_APB1ENR_TIM2EN;
    PIKA_TIM2_CR1 &= ~PIKA_TIM_CR1_CEN;
    PIKA_TIM2_PSC = (uint32_t)prescaler;
    PIKA_TIM2_ARR = (uint32_t)auto_reload;
    PIKA_TIM2_CCMR1 = PIKA_TIM_CCMR_PWM1_BOTH;
    PIKA_TIM2_CCR1 = pwm_compare_value(
        &pika_f103_pwm_channels[0],
        period_counts,
        period_ns);
    PIKA_TIM2_CCR2 = pwm_compare_value(
        &pika_f103_pwm_channels[1],
        period_counts,
        period_ns);
    if (pika_f103_pwm_channels[0].allocated != 0u &&
        pika_f103_pwm_channels[0].enabled != 0u) {
        enabled |= PIKA_TIM_CCER_CC1E;
    }
    if (pika_f103_pwm_channels[1].allocated != 0u &&
        pika_f103_pwm_channels[1].enabled != 0u) {
        enabled |= PIKA_TIM_CCER_CC2E;
    }
    PIKA_TIM2_CCER =
        (PIKA_TIM2_CCER & ~PIKA_TIM_CCER_PWM_ENABLE_MASK) |
        enabled;
    PIKA_TIM2_EGR = PIKA_TIM_EGR_UG;
    PIKA_TIM2_CR1 |= PIKA_TIM_CR1_ARPE;
    if (enabled != 0u) {
        PIKA_TIM2_CR1 |= PIKA_TIM_CR1_CEN;
    }
    pwm_record_trace();
    return PIKA_STATUS_OK;
}

static PikaStatus pwm_validate_proposed(
    const PikaF103Pwm* channels,
    uint32_t frequency_hz) {
    uint16_t prescaler;
    uint16_t auto_reload;
    uint32_t period_counts;
    uint64_t period_ns;
    uint32_t index;
    PikaStatus status;
    if (frequency_hz == 0u) {
        return PIKA_STATUS_OK;
    }
    status = pwm_timer_period(
        frequency_hz,
        &prescaler,
        &auto_reload,
        &period_counts);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    period_ns =
        PIKA_F103_PWM_NANOSECONDS_PER_SECOND /
        (uint64_t)frequency_hz;
    for (index = 0u;
         index < (uint32_t)PIKA_F103_PWM_CHANNEL_COUNT;
         ++index) {
        if (channels[index].allocated != 0u &&
            channels[index].duty_mode ==
                PIKA_MACHINE_PWM_DUTY_NS &&
            channels[index].duty_ns > period_ns) {
            return PIKA_STATUS_VALUE_ERROR;
        }
    }
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pwm_open(
    const PikaMachinePwmConfig* config,
    void** handle) {
    uint32_t index;
    if (handle == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    *handle = NULL;
    if (config == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (config->identifier < 0 ||
        config->identifier >=
            (int64_t)PIKA_F103_PWM_CHANNEL_COUNT) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    index = (uint32_t)config->identifier;
    if (pika_f103_pwm_channels[index].allocated != 0u) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    memset(
        &pika_f103_pwm_channels[index],
        0,
        sizeof(pika_f103_pwm_channels[index]));
    pika_f103_pwm_channels[index].allocated = 1u;
    pwm_configure_gpio(index);
    PIKA_RCC_APB1ENR |= PIKA_RCC_APB1ENR_TIM2EN;
    *handle = &pika_f103_pwm_channels[index];
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pwm_configure(
    void* handle,
    const PikaMachinePwmSettings* settings) {
    PikaF103Pwm proposed[PIKA_F103_PWM_CHANNEL_COUNT];
    PikaF103Pwm* pwm;
    uint32_t channel_index;
    uint32_t frequency_hz;
    PikaStatus status;
    pwm = pwm_find(handle, &channel_index);
    if (pwm == NULL || settings == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    if (settings->frequency_present > 1u ||
        settings->enabled > 1u ||
        (settings->duty_mode !=
             PIKA_MACHINE_PWM_DUTY_UNCHANGED &&
         settings->duty_mode != PIKA_MACHINE_PWM_DUTY_U16 &&
         settings->duty_mode != PIKA_MACHINE_PWM_DUTY_NS)) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    frequency_hz =
        settings->frequency_present != 0u
            ? settings->frequency_hz
            : pika_f103_pwm_frequency_hz;
    if (settings->frequency_present != 0u &&
        settings->frequency_hz == 0u) {
        return PIKA_STATUS_VALUE_ERROR;
    }
    memcpy(proposed, pika_f103_pwm_channels, sizeof(proposed));
    if (settings->duty_mode == PIKA_MACHINE_PWM_DUTY_U16) {
        proposed[channel_index].duty_mode =
            PIKA_MACHINE_PWM_DUTY_U16;
        proposed[channel_index].duty_u16 =
            settings->duty_u16;
    } else if (
        settings->duty_mode == PIKA_MACHINE_PWM_DUTY_NS) {
        proposed[channel_index].duty_mode =
            PIKA_MACHINE_PWM_DUTY_NS;
        proposed[channel_index].duty_ns =
            settings->duty_ns;
    }
    proposed[channel_index].enabled = settings->enabled;
    status = pwm_validate_proposed(proposed, frequency_hz);
    if (status != PIKA_STATUS_OK) {
        return status;
    }
    memcpy(
        pika_f103_pwm_channels,
        proposed,
        sizeof(pika_f103_pwm_channels));
    pika_f103_pwm_frequency_hz = frequency_hz;
    return pwm_refresh_hardware();
}

PikaStatus pika_machine_platform_pwm_state(
    void* handle,
    PikaMachinePwmState* state) {
    PikaF103Pwm* pwm = pwm_find(handle, NULL);
    uint64_t period_ns;
    if (pwm == NULL || state == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    memset(state, 0, sizeof(*state));
    state->frequency_hz = pika_f103_pwm_frequency_hz;
    state->frequency_valid =
        pika_f103_pwm_frequency_hz != 0u ? 1u : 0u;
    state->duty_mode = pwm->duty_mode;
    if (state->frequency_valid != 0u) {
        period_ns =
            PIKA_F103_PWM_NANOSECONDS_PER_SECOND /
            (uint64_t)pika_f103_pwm_frequency_hz;
        if (pwm->duty_mode == PIKA_MACHINE_PWM_DUTY_U16) {
            state->duty_u16 = pwm->duty_u16;
            state->duty_ns =
                (period_ns * (uint64_t)pwm->duty_u16 +
                 UINT64_C(32767)) /
                UINT64_C(65535);
        } else if (
            pwm->duty_mode == PIKA_MACHINE_PWM_DUTY_NS) {
            state->duty_ns = pwm->duty_ns;
            state->duty_u16 = (uint16_t)(
                (pwm->duty_ns * UINT64_C(65535) +
                 period_ns / UINT64_C(2)) /
                period_ns);
        }
    }
    state->enabled =
        pwm->enabled != 0u &&
                state->frequency_valid != 0u
            ? 1u
            : 0u;
    return PIKA_STATUS_OK;
}

PikaStatus pika_machine_platform_pwm_disable(void* handle) {
    PikaF103Pwm* pwm = pwm_find(handle, NULL);
    PikaStatus status;
    if (pwm == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    pwm->enabled = 0u;
    status = pwm_refresh_hardware();
    if (status == PIKA_STATUS_OK &&
        (PIKA_TIM2_CCER & PIKA_TIM_CCER_PWM_ENABLE_MASK) ==
            0u &&
        (PIKA_TIM2_CR1 & PIKA_TIM_CR1_CEN) == 0u) {
        pika_f103_pwm_trace |=
            PIKA_F103_PWM_TRACE_FINAL_DISABLED;
    }
    return status;
}

PikaStatus pika_machine_platform_pwm_close(void* handle) {
    PikaF103Pwm* pwm;
    uint32_t channel_index;
    PikaStatus status;
    pwm = pwm_find(handle, &channel_index);
    if (pwm == NULL) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    memset(pwm, 0, sizeof(*pwm));
    pwm_release_gpio(channel_index);
    if (!pwm_any_allocated()) {
        pika_f103_pwm_frequency_hz = 0u;
    }
    status = pwm_refresh_hardware();
    if (!pwm_any_allocated()) {
        PIKA_TIM2_CR1 = 0u;
        PIKA_TIM2_CCER = 0u;
        PIKA_RCC_APB1ENR &= ~PIKA_RCC_APB1ENR_TIM2EN;
    }
    return status;
}

PikaStatus pika_qemu_pwm_verify(void) {
    if ((pika_f103_pwm_trace &
         PIKA_F103_PWM_TRACE_REQUIRED) !=
            PIKA_F103_PWM_TRACE_REQUIRED ||
        (PIKA_TIM2_CCER & PIKA_TIM_CCER_PWM_ENABLE_MASK) !=
            0u ||
        (PIKA_TIM2_CR1 & PIKA_TIM_CR1_CEN) != 0u) {
        return PIKA_STATUS_RUNTIME_ERROR;
    }
    return PIKA_STATUS_OK;
}

#endif

static void uart_write_byte(uint8_t value) {
    while ((PIKA_USART2_SR & (1u << 7)) == 0u) {
    }
    PIKA_USART2_DR = value;
}

static uint8_t uart_read_byte(void) {
    while ((PIKA_USART2_SR & (1u << 5)) == 0u) {
    }
    return (uint8_t)PIKA_USART2_DR;
}

void pika_qemu_platform_initialize(void) {
    uint32_t configuration;
    PIKA_RCC_APB2ENR |= (1u << 0) | (1u << 2);
    PIKA_RCC_APB1ENR |= (1u << 17);
    configuration = PIKA_GPIOA_CRL;
    configuration &= ~((0xfu << 8) | (0xfu << 12));
    configuration |= (0xbu << 8) | (0x4u << 12);
    PIKA_GPIOA_CRL = configuration;
    PIKA_USART2_BRR = 0x341u;
    PIKA_USART2_CR1 = (1u << 13) | (1u << 3) | (1u << 2);
}

void* _sbrk(ptrdiff_t increment) {
    uint8_t* previous = pika_heap_cursor;
    if (increment < 0) {
        uintptr_t decrease =
            (uintptr_t)(-(increment + 1)) + 1u;
        if (decrease >
            (uintptr_t)(pika_heap_cursor - &__heap_start)) {
            return (void*)-1;
        }
        pika_heap_cursor -= decrease;
        return previous;
    }
    if ((uintptr_t)increment >
        (uintptr_t)(&__heap_end - pika_heap_cursor)) {
        return (void*)-1;
    }
    pika_heap_cursor += (uintptr_t)increment;
    if ((uint32_t)(pika_heap_cursor - &__heap_start) >
        pika_heap_peak) {
        pika_heap_peak =
            (uint32_t)(pika_heap_cursor - &__heap_start);
    }
    return previous;
}

static void write_u32(uint32_t value) {
    char digits[10];
    size_t count = 0u;
    do {
        digits[count++] = (char)('0' + value % 10u);
        value /= 10u;
    } while (value != 0u);
    while (count > 0u) {
        --count;
        uart_write_byte((uint8_t)digits[count]);
    }
}

static uint32_t stack_peak_bytes(void) {
    const uint32_t pattern = 0xa5a5a5a5u;
    const uint32_t* cursor =
        (const uint32_t*)(const void*)&__stack_bottom;
    const uint32_t* top =
        (const uint32_t*)(const void*)&__stack_top;
    while (cursor < top && *cursor == pattern) {
        ++cursor;
    }
    return (uint32_t)((uintptr_t)top - (uintptr_t)cursor);
}

void pika_qemu_report_resources(void) {
    static const char heap_prefix[] =
        "PIKA_QEMU_RESOURCE heap_peak_bytes=";
    static const char stack_prefix[] = " stack_peak_bytes=";
    static const char capacity_prefix[] = " heap_capacity_bytes=";
    static const char newline[] = "\n";
    (void)pika_platform_write(
        heap_prefix, sizeof(heap_prefix) - 1u);
    write_u32(pika_heap_peak);
    (void)pika_platform_write(
        stack_prefix, sizeof(stack_prefix) - 1u);
    write_u32(stack_peak_bytes());
    (void)pika_platform_write(
        capacity_prefix, sizeof(capacity_prefix) - 1u);
    write_u32((uint32_t)(&__heap_end - &__heap_start));
    (void)pika_platform_write(newline, sizeof(newline) - 1u);
}

PikaStatus pika_platform_write(const char* data, size_t size) {
    size_t index;
    if (data == NULL && size != 0u) {
        return PIKA_STATUS_INVALID_ARGUMENT;
    }
    for (index = 0u; index < size; ++index) {
        uart_write_byte((uint8_t)data[index]);
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
    data[0] = uart_read_byte();
    *received = 1u;
    return PIKA_STATUS_OK;
}

void pika_qemu_exit(int status) {
    static const char prefix[] = "PIKA_QEMU_EXIT status=";
    static const char newline[] = "\n";
    (void)pika_platform_write(prefix, sizeof(prefix) - 1u);
    write_u32(status < 0 ? 1u : (uint32_t)status);
    (void)pika_platform_write(newline, sizeof(newline) - 1u);
    while ((PIKA_USART2_SR & (1u << 6)) == 0u) {
    }
    (void)pika_semihost_call(0x18, NULL);
    for (;;) {
    }
}

void _exit(int status) {
    pika_qemu_exit(status);
}
