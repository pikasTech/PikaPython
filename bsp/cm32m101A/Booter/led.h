#ifndef __LED_H
#define __LED_H

//Sparrow Board LED
typedef enum{
	SP_LED1,
	SP_LED2,
	SP_LED3,
	SP_LED4,
}sp_led_t;

//LED×´Ì¬
typedef enum{
	SP_LED_STATE_CLOSE,	//LED¹Ø±Õ
	SP_LED_STATE_OPEN		//LED¿ªÆô
}sp_led_State_t;


void Led_Init(void);
void Led_SetState(sp_led_t led,sp_led_State_t state);
void Led_Toggles(sp_led_t led);
sp_led_State_t Led_GetState(sp_led_t led);

#endif
