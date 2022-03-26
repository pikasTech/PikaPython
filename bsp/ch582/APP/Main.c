#include "CH58x_common.h"
#include "HAL.h"
#include "config.h"
#include "pikascript.h"

void DebugInit(void)
{
  GPIOA_SetBits(GPIO_Pin_9);
  GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
  GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
  UART1_DefInit();
}

int main()
{
  /* 默认运行频率60MHZ */
  SetSysClock(CLK_SOURCE_PLL_60MHz);
  /* 低功耗配置,全部io默认上拉输入 */
  GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
  GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
  /* uart1初始化 */
  DebugInit();
  /* pikascript初始化 */
  pikaScriptInit();
  while (1)
  {
  }
}
