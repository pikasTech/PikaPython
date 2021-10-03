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
#define APP_SCREEN_WIDTH 240
#define APP_SCREEN_HEIGHT 360

int32_t GLCD_DrawBitmap (uint32_t x, 
                         uint32_t y, 
                         uint32_t width, 
                         uint32_t height, 
                         const uint16_t *bitmap)
{
    LCD_myDrawPicture(x, y, width, height, bitmap);
    return 0;
}

static IMPL_PFB_ON_DRAW(__pfb_draw_background_handler)
{
    ARM_2D_UNUSED(pTarget);
    //! 背景填充白色
    arm_2d_rgb16_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);
    return arm_fsm_rt_cpl;
}

static IMPL_PFB_ON_LOW_LV_RENDERING(__pfb_render_handler)
{
    const arm_2d_tile_t *ptTile = &(ptPFB->tTile);

    ARM_2D_UNUSED(pTarget);

    GLCD_DrawBitmap(ptTile->tRegion.tLocation.iX,
                    ptTile->tRegion.tLocation.iY,
                    ptTile->tRegion.tSize.iWidth,
                    ptTile->tRegion.tSize.iHeight,
                    (uint16_t *)ptTile->pchBuffer);

    arm_2d_helper_pfb_report_rendering_complete(&s_tPFBHelper, 
                                            (arm_2d_pfb_t *)ptPFB);
}

static IMPL_PFB_ON_DRAW(__pfb_draw_handler)
{
    ARM_2D_UNUSED(pTarget);
    int x = obj_getInt(pikaMain,"box.x");
    int y = obj_getInt(pikaMain,"box.y");
    int sizeX = obj_getInt(pikaMain,"box.sizeX");
    int sizeY = obj_getInt(pikaMain,"box.sizeY");
    int isOn = obj_getInt(pikaMain,"box.isOn");
    arm_2d_region_t tBox = {
        .tLocation = {x, y},
        .tSize = {sizeX, sizeY},
    };
    //! 背景填充白色
    arm_2d_rgb16_fill_colour(ptTile, NULL, GLCD_COLOR_WHITE);    
    if (isOn)
    {
        arm_2d_rgb16_fill_colour(ptTile, &tBox, GLCD_COLOR_BLUE);
    }
    return arm_fsm_rt_cpl;
}



void draw_main()
{
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
            &s_tPFBHelper,                    //!< FPB Helper object
            APP_SCREEN_WIDTH,                 //!< screen width
            APP_SCREEN_HEIGHT,                //!< screen height
            uint16_t,                         //!< colour date type
            APP_SCREEN_WIDTH ,                              //!< PFB Box width
            180 ,                              //!< PFB Box height
            1,                                //!< number of PFB in the PFB pool
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
    ARM_2D_HELPER_PFB_UPDATE_ON_DRAW_HANDLER(   &s_tPFBHelper,
                                                &__pfb_draw_handler);    
    uart1_sendStr("\r\n");    
    uart1_sendStr("==============================================\r\n");    
    uart1_sendStr("* Arm-2D and PikaScript in STM32f407         *\r\n");
    uart1_sendStr("* https://github.com/ARM-software/EndpointAI *\r\n");
    uart1_sendStr("* https://github.com/mimilib/pikascript      *\r\n");
    uart1_sendStr("* Have fun!                                  *\r\n");    
    uart1_sendStr("==============================================\r\n");   

    uart1_sendStr("Pika script Initing...\r\n");
    obj_run_uart1(pikaMain, "   box.init()                            ");
    uart1_sendStr("hello Arm-2D and PikaScript!\r\n");
    obj_run_uart1(pikaMain, "   box.on()                              ");

    draw_main();	 
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

