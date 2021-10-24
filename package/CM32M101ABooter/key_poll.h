#ifndef __KEY_POLL_H
#define __KEY_POLL_H

//#define SP_KEY_POLL_LOOSE_CHECK		//°´¼üÌ§Æð¼ì²â

typedef enum{
	SP_KEY_PRESS_NONE,
	SP_KEY_PRESS_KEY1,
	SP_KEY_PRESS_KEY2,
	SP_KEY_PRESS_KEY3,
	SP_KEY_PRESS_KEY4,
}sp_key_value_t;

void Key_PollInit(void);
sp_key_value_t Key_Poll(void);

#endif
