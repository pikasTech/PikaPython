#ifndef __DEV_SYSTICK_H__
#define __DEV_SYSTICK_H__

void systick_init(void);
void swm_inctick(void);
void swm_delay(__IO uint32_t delay);

#endif //__DEV_SYSTICK_H__
