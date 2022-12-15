#ifndef __MAIN_BOARD_H__
#define __MAIN_BOARD_H__

#include <stdint.h>

uint32_t bl60x_board_get_factory_addr(void);
int bl60x_board_cfg(uint8_t board_code);

#endif /*__MAIN_BOARD_H__*/
