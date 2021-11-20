#include "Key_PikaPiZero_KEY.h"
#include "main.h"
int Key_PikaPiZero_KEY_get(PikaObj *self){
    /* SW0 */
    if(LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_0)){
        return 0;
    }
    /* SW1 */
    if(!LL_GPIO_IsInputPinSet(GPIOA, LL_GPIO_PIN_15)){
        return 1;
    }    
    /* SW2 */
    if(!LL_GPIO_IsInputPinSet(GPIOB, LL_GPIO_PIN_6)){
        return 2;
    }      
    /* SW3 */
    if(!LL_GPIO_IsInputPinSet(GPIOC, LL_GPIO_PIN_13)){
        return 3;
    }
    return -1;
}

void Key_PikaPiZero_KEY_init(PikaObj *self){
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
    /*Configure GPIO*/
    /* SW0 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    /* SW1 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_15;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);    
    /* SW3 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_13;
    LL_GPIO_Init(GPIOC, &GPIO_InitStruct);      
    /* SW2 */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_6;
    LL_GPIO_Init(GPIOB, &GPIO_InitStruct);      
}