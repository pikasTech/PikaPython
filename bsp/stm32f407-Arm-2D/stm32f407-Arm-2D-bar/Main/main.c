#include "led.h"
#include "beep.h"
#include "key.h"
#include "lcd.h"
#include "arm_2d.h"
#include "arm_2d_helper.h"
#include "uart.h"
#include "PikaMain.h"

static arm_2d_helper_pfb_t s_tPFBHelper;
MimiObj *pikaMain;
#define GLCD_COLOR_WHITE WHITE
#define GLCD_COLOR_BLACK BLACK
#define GLCD_COLOR_BLUE BLUE
int32_t GLCD_DrawBitmap (uint32_t x, 
                         uint32_t y, 
                         uint32_t width, 
                         uint32_t height, 
                         const uint8_t *bitmap)
{
    LCD_Draw_Picture(x, y, x + width - 1, y + height- 1,(uint16_t *) bitmap);
    return 0;
}

static IMPL_PFB_ON_LOW_LV_RENDERING(__pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);

    GLCD_DrawBitmap(ptTile->tRegion.tLocation.iX,
                    ptTile->tRegion.tLocation.iY,
                    ptTile->tRegion.tSize.iWidth,
                    ptTile->tRegion.tSize.iHeight,
                    ptTile->pchBuffer);

    arm_2d_helper_pfb_report_rendering_complete(&s_tPFBHelper, 
                                            (arm_2d_pfb_t *)ptPFB);
}


static IMPL_PFB_ON_DRAW(__pfb_draw_background_handler)
{
    ARM_2D_UNUSED(pTarget);
    //! 背景填充白色
    arm_2d_rgb16_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
    return arm_fsm_rt_cpl;
}

extern DMEM_STATE DMEMS;
void MemoryChecker_size(MimiObj *self)
{
    char buff[256] = {0};
	sprintf(buff, "memory loop size = %0.2f kB\r\n", DMEM_TOTAL_SIZE / 1024.0);
    uart1_sendStr(buff);
}
void MemoryChecker_max(MimiObj *self)
{
    char buff[256] = {0};
	sprintf(buff, "memory used max = %0.2f kB\r\n", DMEMS.maxNum * DMEM_BLOCK_SIZE / 1024.0);
    uart1_sendStr(buff);
}
void MemoryChecker_now(MimiObj *self)
{
    char buff[256] = {0};
	sprintf(buff, "memory used now = %0.2f kB\r\n", DMEMS.blk_num * DMEM_BLOCK_SIZE / 1024.0);
    uart1_sendStr(buff);
}


void Block_init(MimiObj *self)
{
    obj_setInt(self, "isOn", 0);
    obj_setInt(self, "x", 50);
    obj_setInt(self, "y", 50);
    obj_setInt(self, "sizeX", 150);
    obj_setInt(self, "sizeY", 75);
}
void Block_moveLeft(MimiObj *self, int x)
{
    obj_setInt(self, "x", obj_getInt(self, "x") - x);
}
void Block_moveRight(MimiObj *self, int x)
{    
    obj_setInt(self, "x", obj_getInt(self, "x") + x);
}
void Block_moveTo(MimiObj *self, int x, int y)
{    
    obj_setInt(self, "x", x);
    obj_setInt(self, "y", y);
}    
void Block_off(MimiObj *self)
{    
    obj_setInt(self, "isOn", 0);
}
void Block_on(MimiObj *self)
{    
    obj_setInt(self, "isOn", 1);
}
void Block_setColor(MimiObj *self, char * color)
{   
}
void Block_setSize(MimiObj *self, int sizeX, int sizeY)
{   
    obj_setInt(self, "sizeX", sizeX);
    obj_setInt(self, "sizeY", sizeY);
}

static IMPL_PFB_ON_DRAW(__pfb_draw_handler)
{
    ARM_2D_UNUSED(pTarget);
    int x = obj_getInt(pikaMain,"block.x");
    int y = obj_getInt(pikaMain,"block.y");
    int sizeX = obj_getInt(pikaMain,"block.sizeX");
    int sizeY = obj_getInt(pikaMain,"block.sizeY");
    int isOn = obj_getInt(pikaMain,"block.isOn");
    arm_2d_region_t tBox = {
        .tLocation = {x, y},
        .tSize = {sizeX, sizeY},
    };
    //! 背景填充白色
    arm_2d_rgb16_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);    
    if (isOn)
    {
        //! 在box指定的区域绘制黑色影子
        arm_2d_rgb16_fill_colour(ptTile, &tBox, GLCD_COLOR_BLACK);
        //! 适当向左上角移动box
        tBox.tLocation.iX -= 5;
        tBox.tLocation.iY -= 5;
        //! 在box指定的区域填充蓝色，并且使用 50%(128/255)的透明效果
        arm_2d_rgb565_fill_colour_with_alpha(   
            ptTile, 
            &tBox, 
            (arm_2d_color_rgb565_t){GLCD_COLOR_BLUE}, 
            128);      //!< 透明度
    }
    return arm_fsm_rt_cpl;
}
void draw_main()
{
    ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(   &s_tPFBHelper,
                                                &__pfb_draw_handler);
    while(arm_fsm_rt_cpl != arm_2d_helper_pfb_task(&s_tPFBHelper, NULL));
}


void obj_run_uart1(MimiObj *self, char *cmd)
{
    /* safe, stop when error occord and error info would be print32_t */
    Args *res = obj_runDirect(self, cmd);
    char *sysOut = args_getStr(res, "sysOut");
    if (NULL != sysOut)
    {
        uart1_sendStr(sysOut);
        uart1_sendStr("\r\n");
    }
    if (0 != args_getInt(res, "errCode"))
    {
        uart1_sendStr("[info] input commond: \r\n");
        uart1_sendStr(cmd);
        uart1_sendStr("\r\n");
        while (1)
            ;
    }
    if (NULL != res)
    {
        args_deinit(res);
    }
}

int main(void)
{ 
    /* hardware init */
    HAL_Init();                    	//初始化HAL库    
    Stm32_Clock_Init(336,8,2,7);  	//设置时钟,168Mhz
	delay_init();         //延时函数初始化
	LED_Init();			  //LED初始化
	BEEP_Init();          //蜂鸣器初始化
	KEY_Init();           //按键初始化
 	LCD_Init();           //初始化LCD FSMC接口和显示驱动
    uart1_init(115200);

    /* Arm-2D init */
    arm_2d_init(); 
    
    /* Arg-2D-helper init */
    //! initialise FPB helper
    if (ARM_2D_HELPER_PFB_INIT( 
            &s_tPFBHelper,     //!< FPB Helper object
            24 * 10,               //!< screen width
            32 * 10,               //!< screen height
            uint16_t,          //!< colour date type
            16 ,                //!< PFB block width
            16 ,                //!< PFB block height
            1,                 //!< number of PFB in the PFB pool
            {
                .evtOnLowLevelRendering = {
                    //! callback for low level rendering 
                    .fnHandler = &__pfb_render_handler,                         
                },
                .evtOnDrawing = {
                    //! callback for drawing GUI 
                    .fnHandler = &__pfb_draw_background_handler, 
                },
            }
        ) < 0) {
        //! error detected
        assert(false);
    }
    /* PikaScript init */
    pikaMain = newRootObj("pikaMain", New_PikaMain);
    /* user init */
    while(arm_fsm_rt_cpl != arm_2d_helper_pfb_task(&s_tPFBHelper, NULL));  
    uart1_sendStr("\r\n");    
    uart1_sendStr("==============================================\r\n");    
    uart1_sendStr("* Arm-2D and PikaScript in STM32f407         *\r\n");
    uart1_sendStr("* https://github.com/ARM-software/EndpointAI *\r\n");
    uart1_sendStr("* https://github.com/mimilib/pikascript      *\r\n");
    uart1_sendStr("* Have fun!                                  *\r\n");    
    uart1_sendStr("==============================================\r\n");   

    obj_run_uart1(pikaMain, "   print('Pika script Initing...')         ");
    obj_run_uart1(pikaMain, "   block.init()                            ");
    obj_run_uart1(pikaMain, "   print('hello Arm-2D and PikaScript!')   ");
    uart1_sendStr(">>> ");

    while(1) 
	{	
        if(UART1_reciveOk)
        {
            UART1_reciveOk = 0;
            /* run mimiScript and get res */
            Args *resArgs = obj_runDirect(pikaMain, (char *)UART1_reciveStr);

            /* get system output of mimiScript*/
            char *sysOut = args_getStr(resArgs, "sysOut");

            uart1_sendStr((char *)UART1_reciveStr);
            uart1_sendStr("\r\n");
            if (NULL != sysOut)
            {
                /* print32_t out the system output */
                uart1_sendStr(sysOut);
                uart1_sendStr("\r\n");
            }
                uart1_sendStr(">>> ");


            /* deinit the res */
            args_deinit(resArgs);

            draw_main();	 
        }
	}
}

