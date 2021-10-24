#ifndef __KEY_EXTI_H
#define __KEY_EXTI_H

typedef enum{
	SP_KEY_PRESS_NONE,
	SP_KEY_PRESS_KEY1,
	SP_KEY_PRESS_KEY2,
	SP_KEY_PRESS_KEY3,
	SP_KEY_PRESS_KEY4,
}sp_key_value_t;

void Key_ExtiInit(void);
sp_key_value_t Key_GetValue(void);
#endif

