#ifndef	__RELAY_H
#define __RELAY_H

//Sparrow Board RELAY

typedef enum{
	SP_RELAY1,
}sp_relay_t;

//¼ÌµçÆ÷×´Ì¬
typedef enum{
	SP_RELAY_STATE_CLOSE,	//¹Ø±Õ
	SP_RELAY_STATE_OPEN		//¿ªÆô
}sp_relay_state_t;


void Relay_Init(void);
void Relay_SetState(sp_relay_t relay,sp_relay_state_t state);
void Relay_Toggles(sp_relay_t relay);
sp_relay_state_t Relay_GetState(sp_relay_t relay);

#endif
