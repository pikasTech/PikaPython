#include "lcd.h"
#include "cfont.h"
#include "led.h"

/*********************************************************************************
**********************启明欣欣 STM32F407应用开发板(高配版)************************
**********************************************************************************
* 文件名称: lcd.c                                                                *
* 文件简述：LCD显示驱动程序                                                      *
* 创建日期：2016.10.08                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：此例程支持屏幕驱动为ILI9341，SSD1963                                 * 
*           RA8875需用模块资料里面的RA8875屏幕资料中的LCD整个文件夹替换本例程的  *
            LCD文件夹，重新编译即可                                              *
**********************************************************************************
*********************************************************************************/

SRAM_HandleTypeDef LCDSRAM_Handler; //SRAM句柄(用于控制LCD)

//初始化LCD的画笔颜色和背景色
u16 BRUSH_COLOR = BLACK; //画笔颜色
u16 BACK_COLOR = WHITE;	 //背景色

//LCD驱动参数
u16 lcd_id;		   //LCD ID
u8 dir_flag;	   //横屏与竖屏控制：0，竖屏；1，横屏
u16 lcd_width;	   //LCD的宽度
u16 lcd_height;	   //LCD的高度
u16 write_gramcmd; //写gram指令
u16 setxcmd;	   //设置x坐标指令
u16 setycmd;	   //设置y坐标指令

/****************************************************************************
* 名    称: void LCD_WriteReg(u16 LCD_Reg, u16 LCD_Value)
* 功    能：LCD写寄存器
* 入口参数：LCD_Reg: 寄存器地址
*           LCD_RegValue: 要写入的数据
* 返回参数：无
* 说    明：       
****************************************************************************/
void LCD_WriteReg(u16 LCD_Reg, u16 LCD_Value)
{
	LCD_CMD = LCD_Reg;	  //写入要写的寄存器序号
	LCD_DATA = LCD_Value; //向寄存器写入的数据
}

/****************************************************************************
* 名    称: u16 LCD_ReadReg(u16 LCD_Reg)
* 功    能：LCD读寄存器
* 入口参数：LCD_Reg:寄存器地址
* 返回参数：读到该寄存器序号里的值
* 说    明：       
****************************************************************************/
u16 LCD_ReadReg(u16 LCD_Reg)
{
	LCD_CMD = LCD_Reg; //写入要读的寄存器序号
	delay_us(4);
	return LCD_DATA; //返回读到的值
}

//lcd延时函数
void lcdm_delay(u8 i)
{
	while (i--)
		;
}

//开始写GRAM
void LCD_WriteGRAM(void)
{
	LCD_CMD = write_gramcmd;
}

//LCD开启显示
void LCD_DisplayOn(void)
{
	LCD_CMD = 0x29; //9341与1963开显示命令一样
}

//LCD关闭显示
void LCD_DisplayOff(void)
{
	LCD_CMD = 0x28; //9341与1963关显示命令一样
}

/****************************************************************************
* 名    称: void LCD_Open_Window(u16 X0,u16 Y0,u16 width,u16 height)
* 功    能：开窗口,并设置画点坐标到窗口左上角(X0,Y0)
* 入口参数：X0,Y0:窗口起始坐标(左上角)
            width,height:窗口宽度和高度
* 返回参数：无
* 说    明：窗体大小:width*height.?B
****************************************************************************/
void LCD_Open_Window(u16 X0, u16 Y0, u16 width, u16 height)
{
	width = X0 + width - 1; //算出右下角坐标
	height = Y0 + height - 1;

	if (dir_flag == 0 && lcd_id == 0X1963) //1963竖屏处理
	{
		X0 = lcd_width - width - X0;
		height = Y0 + height - 1;
		LCD_CMD = setxcmd;
		LCD_DATA = X0 >> 8;
		LCD_DATA = X0 & 0XFF;
		LCD_DATA = (X0 + width - 1) >> 8;
		LCD_DATA = (X0 + width - 1) & 0XFF;
		LCD_CMD = setycmd;
		LCD_DATA = Y0 >> 8;
		LCD_DATA = Y0 & 0XFF;
		LCD_DATA = height >> 8;
		LCD_DATA = height & 0XFF;
	}
	else
	{
		LCD_CMD = setxcmd;
		LCD_DATA = X0 >> 8;
		LCD_DATA = X0 & 0XFF;
		LCD_DATA = width >> 8;
		LCD_DATA = width & 0XFF;
		LCD_CMD = setycmd;
		LCD_DATA = Y0 >> 8;
		LCD_DATA = Y0 & 0XFF;
		LCD_DATA = height >> 8;
		LCD_DATA = height & 0XFF;
	}
}

/****************************************************************************
* 名    称: void Set_Scan_Direction(u8 direction)    启￥明#欣￥欣
* 功    能：设置LCD的扫描方向
* 入口参数：direction：扫描方向
* 返回参数：无
* 说    明：     
****************************************************************************/
void Set_Scan_Direction(u8 direction)
{
	u16 skhda = 0;
	u16 diomf = 0;
	//9341横屏和1963竖屏时需要转化下
	if ((dir_flag == 1 && lcd_id == 0X9341) || (dir_flag == 0 && lcd_id == 0X1963))
	{
		switch (direction) //方向转换
		{
		case 0:
			direction = 6;
			break;
		case 1:
			direction = 7;
			break;
		case 2:
			direction = 4;
			break;
		case 3:
			direction = 5;
			break;
		case 4:
			direction = 1;
			break;
		case 5:
			direction = 0;
			break;
		case 6:
			direction = 3;
			break;
		case 7:
			direction = 2;
			break;
		}
	}

	switch (direction)
	{
	case L2R_U2D: //从左到右,从上到下
		skhda |= (0 << 7) | (0 << 6) | (0 << 5);
		break;
	case L2R_D2U: //从左到右,从下到上
		skhda |= (1 << 7) | (0 << 6) | (0 << 5);
		break;
	case R2L_U2D: //从右到左,从上到下
		skhda |= (0 << 7) | (1 << 6) | (0 << 5);
		break;
	case R2L_D2U: //从右到左,从下到上
		skhda |= (1 << 7) | (1 << 6) | (0 << 5);
		break;
	case U2D_L2R: //从上到下,从左到右
		skhda |= (0 << 7) | (0 << 6) | (1 << 5);
		break;
	case U2D_R2L: //从上到下,从右到左
		skhda |= (0 << 7) | (1 << 6) | (1 << 5);
		break;
	case D2U_L2R: //从下到上,从左到右
		skhda |= (1 << 7) | (0 << 6) | (1 << 5);
		break;
	case D2U_R2L: //从下到上,从右到左
		skhda |= (1 << 7) | (1 << 6) | (1 << 5);
		break;
	}
	diomf = 0X36;
	if (lcd_id == 0X9341)
		skhda |= 0X08;
	LCD_WriteReg(diomf, skhda);
	LCD_Open_Window(0, 0, lcd_width, lcd_height); //设置完扫描方向后，开显示区域为全屏窗口
}

/****************************************************************************
* 名    称: void Set_Display_Mode(u8 mode)
* 功    能：设置LCD显示方向
* 入口参数：mode: 0,竖屏
                  1,横屏
* 返回参数：无
* 说    明：?B
****************************************************************************/
void Set_Display_Mode(u8 mode)
{
	if (mode == 0) //竖屏
	{
		dir_flag = 0;

		if (lcd_id == 0X9341)
		{
			write_gramcmd = 0X2C;
			setxcmd = 0X2A;
			setycmd = 0X2B;
			lcd_width = 240;
			lcd_height = 320;
		}
		else if (lcd_id == 0X1963)
		{
			write_gramcmd = 0X2C; //GRAM的指令
			setxcmd = 0X2B;		  //写X坐标指令
			setycmd = 0X2A;		  //写Y坐标指令
			lcd_width = 480;	  //设置宽度480
			lcd_height = 800;	  //设置高度800
		}
	}
	else //横屏
	{
		dir_flag = 1;

		if (lcd_id == 0X9341)
		{
			write_gramcmd = 0X2C;
			setxcmd = 0X2A;
			setycmd = 0X2B;
			lcd_width = 320;
			lcd_height = 240;
		}
		else if (lcd_id == 0X1963)
		{
			write_gramcmd = 0X2C; //GRAM的指令
			setxcmd = 0X2A;		  //写X坐标指令
			setycmd = 0X2B;		  //写Y坐标指令
			lcd_width = 800;	  //设置宽度800
			lcd_height = 480;	  //设置高度480
		}
	}
	Set_Scan_Direction(L2R_U2D); //设置扫描方向   从左到右,从下到上
}

/****************************************************************************
* 名    称: void LCD_SetCursor(u16 Xaddr, u16 Yaddr)       启#明%欣#欣
* 功    能：设置光标位置
* 入口参数：x：x坐标
            y：y坐标
* 返回参数：无
* 说    明：     
****************************************************************************/
void LCD_SetCursor(u16 Xaddr, u16 Yaddr)
{
	if (lcd_id == 0X9341)
	{
		LCD_CMD = setxcmd;
		LCD_DATA = (Xaddr >> 8);
		LCD_DATA = (Xaddr & 0XFF);
		LCD_CMD = setycmd;
		LCD_DATA = (Yaddr >> 8);
		LCD_DATA = (Yaddr & 0XFF);
	}
	else if (lcd_id == 0X1963)
	{
		if (dir_flag == 0)
		{
			Xaddr = lcd_width - 1 - Xaddr;
			LCD_CMD = setxcmd;
			LCD_DATA = 0;
			LCD_DATA = 0;
			LCD_DATA = Xaddr >> 8;
			LCD_DATA = Xaddr & 0XFF;
		}
		else
		{
			LCD_CMD = setxcmd;
			LCD_DATA = Xaddr >> 8;
			LCD_DATA = Xaddr & 0XFF;
			LCD_DATA = (lcd_width - 1) >> 8;
			LCD_DATA = (lcd_width - 1) & 0XFF;
		}
		LCD_CMD = setycmd;
		LCD_DATA = Yaddr >> 8;
		LCD_DATA = Yaddr & 0XFF;
		LCD_DATA = (lcd_height - 1) >> 8;
		LCD_DATA = (lcd_height - 1) & 0XFF;
	}
}

/****************************************************************************
* 名    称: u16 LCD_GetPoint(u16 x,u16 y)
* 功    能：读取某点的颜色值	
* 入口参数：x：x坐标
            y：y坐标
* 返回参数：此点的颜色
* 说    明：     
****************************************************************************/
u16 LCD_GetPoint(u16 x, u16 y)
{
	vu16 r = 0, g = 0, b = 0;

	LCD_SetCursor(x, y);

	LCD_CMD = 0X2E; //9341与1963读GRAM指令一样
	r = LCD_DATA;

	if (lcd_id == 0X1963)
		return r; //1963直接读出来就是16位颜色值

	else //其他驱动就是9341
	{
		lcdm_delay(2);
		b = LCD_DATA; //9341要读2次
		g = r & 0XFF; //9341第一次读取的是RG的值,R在前,G在后,各占8位
		g <<= 8;
		return (((r >> 11) << 11) | ((g >> 10) << 5) | (b >> 11)); //9341需公式转换
	}
}

/****************************************************************************
* 名    称: void LCD_DrawPoint(u16 x,u16 y)
* 功    能：画点（在该点写入画笔的颜色）
* 入口参数：x：x坐标
            y：y坐标
* 返回参数：无
* 说    明?BRUSH_COLOR:此点的颜色值
****************************************************************************/
void LCD_DrawPoint(u16 x, u16 y)
{
	LCD_SetCursor(x, y); //设置光标位置
	LCD_WriteGRAM();	 //开始写入GRAM
	LCD_DATA = BRUSH_COLOR;
}

/****************************************************************************
* 名    称: void LCD_Color_DrawPoint(u16 x,u16 y,u16 color)
* 功    能：在设置的坐标处画相应颜色（在该点写入自定义颜色）
* 入口参数：x：x坐标
            y：y坐标
            color 此点的颜色值
* 返回参数：无
* 说    明：color:写入此点的颜色值   GUI调用该函数
****************************************************************************/
void LCD_Color_DrawPoint(u16 x, u16 y, u16 color)
{
	LCD_DrawPoint(x, y);
	LCD_CMD = write_gramcmd;
	LCD_DATA = color;
}

/****************************************************************************
* 名    称: void Ssd1963_Set_BackLight(u8 BL_value)
* 功    能：SSD1963 设置背光 
* 入口参数：BL_value：背光亮度大小  取值:0-255  设置255最亮
* 返回参数：无
* 说    明：     
****************************************************************************/
void Ssd1963_Set_BackLight(u8 BL_value)
{
	LCD_CMD = 0xBE;
	LCD_DATA = 0x05;
	LCD_DATA = BL_value;
	LCD_DATA = 0x01;
	LCD_DATA = 0xFF;
	LCD_DATA = 0x00;
	LCD_DATA = 0x00;
}

/****************************************************************************
* 名    称: void LCD_Clear(u16 color)
* 功    能：清屏函数
* 入口参数：color: 要清屏的填充色
* 返回参数：无
* 说    明：?B
****************************************************************************/
void LCD_Clear(u16 color)
{
	u32 i = 0;
	u32 pointnum = 0;

	pointnum = lcd_width * lcd_height; //得到LCD总点数
	LCD_SetCursor(0x00, 0x00);		   //设置光标位置
	LCD_WriteGRAM();				   //开始写入GRAM
	for (i = 0; i < pointnum; i++)
	{
		LCD_DATA = color;
	}
}

/****************************************************************************
* 名    称: void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
* 功    能：SRAM底层驱动 也就是配置FSMC引脚
* 入口参数：hsram:SRAM句柄
* 返回参数：无
* 说    明：此函数会被HAL_SRAM_Init()调用?B
****************************************************************************/
void HAL_SRAM_MspInit(SRAM_HandleTypeDef *hsram)
{
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_FSMC_CLK_ENABLE();  //使能FSMC时钟
	__HAL_RCC_GPIOD_CLK_ENABLE(); //使能GPIOD时钟
	__HAL_RCC_GPIOE_CLK_ENABLE(); //使能GPIOE时钟
	__HAL_RCC_GPIOF_CLK_ENABLE(); //使能GPIOF时钟
	__HAL_RCC_GPIOG_CLK_ENABLE(); //使能GPIOG时钟

	//初始化PD0,1,4,5,8,9,10,14,15
	GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_8 |
					   GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_14 | GPIO_PIN_15;
	GPIO_Initure.Mode = GPIO_MODE_AF_PP;	 //推挽复用
	GPIO_Initure.Pull = GPIO_PULLUP;		 //上拉
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;	 //高速
	GPIO_Initure.Alternate = GPIO_AF12_FSMC; //复用为FSMC
	HAL_GPIO_Init(GPIOD, &GPIO_Initure);	 //初始化

	//初始化PE7,8,9,10,11,12,13,14,15
	GPIO_Initure.Pin = GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 |
					   GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
	HAL_GPIO_Init(GPIOE, &GPIO_Initure);

	//初始化PG2
	GPIO_Initure.Pin = GPIO_PIN_2;
	HAL_GPIO_Init(GPIOG, &GPIO_Initure);

	//初始化PG12
	GPIO_Initure.Pin = GPIO_PIN_12;
	HAL_GPIO_Init(GPIOG, &GPIO_Initure);
}

//配置FSMC
void LCD_FSMC_Config()
{
	FSMC_NORSRAM_TimingTypeDef FSMC_ReadWriteTim;
	FSMC_NORSRAM_TimingTypeDef FSMC_WriteTim;

	LCDSRAM_Handler.Instance = FSMC_NORSRAM_DEVICE;
	LCDSRAM_Handler.Extended = FSMC_NORSRAM_EXTENDED_DEVICE;

	LCDSRAM_Handler.Init.NSBank = FSMC_NORSRAM_BANK4;						 //使用NE4
	LCDSRAM_Handler.Init.DataAddressMux = FSMC_DATA_ADDRESS_MUX_DISABLE;	 //地址/数据线不复用
	LCDSRAM_Handler.Init.MemoryType = FSMC_MEMORY_TYPE_SRAM;				 //SRAM
	LCDSRAM_Handler.Init.MemoryDataWidth = FSMC_NORSRAM_MEM_BUS_WIDTH_16;	 //16位数据宽度
	LCDSRAM_Handler.Init.BurstAccessMode = FSMC_BURST_ACCESS_MODE_DISABLE;	 //是否使能突发访问,仅对同步突发存储器有效,此处未用到
	LCDSRAM_Handler.Init.WaitSignalPolarity = FSMC_WAIT_SIGNAL_POLARITY_LOW; //等待信号的极性,仅在突发模式访问下有用
	LCDSRAM_Handler.Init.WaitSignalActive = FSMC_WAIT_TIMING_BEFORE_WS;		 //存储器是在等待周期之前的一个时钟周期还是等待周期期间使能NWAIT
	LCDSRAM_Handler.Init.WriteOperation = FSMC_WRITE_OPERATION_ENABLE;		 //存储器写使能
	LCDSRAM_Handler.Init.WaitSignal = FSMC_WAIT_SIGNAL_DISABLE;				 //等待使能位,此处未用到
	LCDSRAM_Handler.Init.ExtendedMode = FSMC_EXTENDED_MODE_ENABLE;			 //读写使用不同的时序
	LCDSRAM_Handler.Init.AsynchronousWait = FSMC_ASYNCHRONOUS_WAIT_DISABLE;	 //是否使能同步传输模式下的等待信号,此处未用到
	LCDSRAM_Handler.Init.WriteBurst = FSMC_WRITE_BURST_DISABLE;				 //禁止突发写
	LCDSRAM_Handler.Init.ContinuousClock = FSMC_CONTINUOUS_CLOCK_SYNC_ASYNC;

	//FMC读时序控制寄存器
	FSMC_ReadWriteTim.AddressSetupTime = 0x0F; //地址建立时间（ADDSET）为16个HCLK 1/168M=6ns*16=96ns
	FSMC_ReadWriteTim.AddressHoldTime = 0;
	FSMC_ReadWriteTim.DataSetupTime = 60;			   //数据保存时间为60个HCLK	=6*60=360ns
	FSMC_ReadWriteTim.AccessMode = FSMC_ACCESS_MODE_A; //模式A
	//FMC写时序控制寄存器
	FSMC_WriteTim.BusTurnAroundDuration = 0; //总线周转阶段持续时间为0，此变量不赋值的话会莫名其妙的自动修改为4。导致程序运行正常
	FSMC_WriteTim.AddressSetupTime = 9;		 //地址建立时间（ADDSET）为9个HCLK =54ns
	FSMC_WriteTim.AddressHoldTime = 0;
	FSMC_WriteTim.DataSetupTime = 8;			   //数据保存时间为6ns*9个HCLK=54n
	FSMC_WriteTim.AccessMode = FSMC_ACCESS_MODE_A; //模式A
	HAL_SRAM_Init(&LCDSRAM_Handler, &FSMC_ReadWriteTim, &FSMC_WriteTim);

	delay_ms(50);
}

u16 ILI9341_Read_id(void)
{
	u16 id;

	LCD_CMD = 0xD3; //9341读ID命令
	id = LCD_DATA;
	id = LCD_DATA; //0x00
	id = LCD_DATA; //0x93
	id <<= 8;
	id |= LCD_DATA; //0x41

	return id;
}

u16 SSD1963_Read_id(void)
{
	u16 id;

	LCD_CMD = (0xA1); //1963读ID命令
	id = LCD_DATA;
	id = LCD_DATA; //0x57
	id <<= 8;
	id |= LCD_DATA; //0x61

	return id;
}

//初始化lcd
void LCD_Init(void)
{
	GPIO_InitTypeDef GPIO_Initure;
	__HAL_RCC_GPIOF_CLK_ENABLE(); //开启GPIOF时钟

	GPIO_Initure.Pin = GPIO_PIN_10;			 //PF10,背光控制
	GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP; //推挽输出
	GPIO_Initure.Pull = GPIO_PULLUP;		 //上拉
	GPIO_Initure.Speed = GPIO_SPEED_HIGH;	 //高速
	HAL_GPIO_Init(GPIOF, &GPIO_Initure);

	LCD_FSMC_Config(); //配置好FSMC就可以驱动液晶屏

	lcd_id = ILI9341_Read_id(); //先读看看所接屏幕是不是9341驱动

	if (lcd_id != 0x9341) //如果不是9341，读看看是不是1963驱动
	{
		lcd_id = SSD1963_Read_id();
		if (lcd_id == 0x5761)
			lcd_id = 0x1963; //SSD1963实际读出的ID是0x5761,为了直观，这边设置为1963
	}

	if (lcd_id == 0X9341) //此驱动,设置写时序为最快
	{
		FSMC_Bank1E->BWTR[6] &= ~(0XF << 0); //地址建立时间清零
		FSMC_Bank1E->BWTR[6] &= ~(0XF << 8); //数据保存时间清零
		FSMC_Bank1E->BWTR[6] |= 3 << 0;		 //地址建立时间为3个HCLK =18ns
		FSMC_Bank1E->BWTR[6] |= 2 << 8;		 //数据保存时间为6ns*3个HCLK=18ns
	}

	if (lcd_id == 0X9341) //9341初始化
	{
		LCD_CMD = 0xCF;
		LCD_DATA = 0x00;
		LCD_DATA = 0xC1;
		LCD_DATA = 0X30;
		LCD_CMD = 0xED;
		LCD_DATA = 0x64;
		LCD_DATA = 0x03;
		LCD_DATA = 0X12;
		LCD_DATA = 0X81;
		LCD_CMD = 0xE8;
		LCD_DATA = 0x85;
		LCD_DATA = 0x10;
		LCD_DATA = 0x7A;
		LCD_CMD = 0xCB;
		LCD_DATA = 0x39;
		LCD_DATA = 0x2C;
		LCD_DATA = 0x00;
		LCD_DATA = 0x34;
		LCD_DATA = 0x02;
		LCD_CMD = 0xF7;
		LCD_DATA = 0x20;
		LCD_CMD = 0xEA;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_CMD = 0xC0;
		LCD_DATA = 0x1B;
		LCD_CMD = 0xC1;
		LCD_DATA = 0x01;
		LCD_CMD = 0xC5;
		LCD_DATA = 0x30;
		LCD_DATA = 0x30;
		LCD_CMD = 0xC7;
		LCD_DATA = 0XB7;
		LCD_CMD = 0x36;
		LCD_DATA = 0x48;
		LCD_CMD = 0x3A;
		LCD_DATA = 0x55;
		LCD_CMD = 0xB1;
		LCD_DATA = 0x00;
		LCD_DATA = 0x1A;
		LCD_CMD = 0xB6;
		LCD_DATA = 0x0A;
		LCD_DATA = 0xA2;
		LCD_CMD = 0xF2;
		LCD_DATA = 0x00;
		LCD_CMD = 0x26;
		LCD_DATA = 0x01;
		LCD_CMD = 0xE0;
		LCD_DATA = 0x0F;
		LCD_DATA = 0x2A;
		LCD_DATA = 0x28;
		LCD_DATA = 0x08;
		LCD_DATA = 0x0E;
		LCD_DATA = 0x08;
		LCD_DATA = 0x54;
		LCD_DATA = 0XA9;
		LCD_DATA = 0x43;
		LCD_DATA = 0x0A;
		LCD_DATA = 0x0F;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_CMD = 0XE1;
		LCD_DATA = 0x00;
		LCD_DATA = 0x15;
		LCD_DATA = 0x17;
		LCD_DATA = 0x07;
		LCD_DATA = 0x11;
		LCD_DATA = 0x06;
		LCD_DATA = 0x2B;
		LCD_DATA = 0x56;
		LCD_DATA = 0x3C;
		LCD_DATA = 0x05;
		LCD_DATA = 0x10;
		LCD_DATA = 0x0F;
		LCD_DATA = 0x3F;
		LCD_DATA = 0x3F;
		LCD_DATA = 0x0F;
		LCD_CMD = 0x2B;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0x01;
		LCD_DATA = 0x3f;
		LCD_CMD = 0x2A;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0xef;
		LCD_CMD = 0x11;
		delay_ms(120);
		LCD_CMD = 0x29;

		LCD_BACK = 1; //点亮背光
	}
	else if (lcd_id == 0X1963)
	{
		LCD_CMD = 0xE2;
		LCD_DATA = 0x1D;
		LCD_DATA = 0x02;
		LCD_DATA = 0x04;
		delay_us(100);

		LCD_CMD = 0xE0;
		LCD_DATA = 0x01;
		delay_ms(10);

		LCD_CMD = 0xE0;
		LCD_DATA = 0x03;
		delay_ms(12);

		LCD_CMD = 0x01; //软复位
		delay_ms(10);

		LCD_CMD = 0xE6;
		LCD_DATA = 0x2F;
		LCD_DATA = 0xFF;
		LCD_DATA = 0xFF;

		LCD_CMD = 0xB0;
		LCD_DATA = 0x20;
		LCD_DATA = 0x00;

		LCD_DATA = (800 - 1) >> 8; //LCD水平像素设置
		LCD_DATA = 800 - 1;
		LCD_DATA = (480 - 1) >> 8; //LCD垂直像素设置
		LCD_DATA = 480 - 1;
		LCD_DATA = 0x00; //RGB序列

		LCD_CMD = 0xB4;
		LCD_DATA = (800 + 46 + 210 - 1) >> 8;
		LCD_DATA = 800 + 46 + 210 - 1;
		LCD_DATA = 46 >> 8;
		LCD_DATA = 46;
		LCD_DATA = 0;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;

		LCD_CMD = 0xB6;
		LCD_DATA = (480 + 23 + 22 - 1) >> 8;
		LCD_DATA = 480 + 23 + 22 - 1;
		LCD_DATA = 23 >> 8;
		LCD_DATA = 23;
		LCD_DATA = 21;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;

		LCD_CMD = 0xF0;
		LCD_DATA = 0x03; //SSD1963与MCU的接口为16bit

		LCD_CMD = 0x29; //开启显示
		LCD_CMD = 0xD0; //设置自动白平衡DBC
		LCD_DATA = 0x00;

		LCD_CMD = 0xBE;	 //PWM输出
		LCD_DATA = 0x05; //PWM频率
		LCD_DATA = 0xFE; //PWM占空比
		LCD_DATA = 0x01;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;
		LCD_DATA = 0x00;

		LCD_CMD = 0xB8;
		LCD_DATA = 0x03;
		LCD_DATA = 0x01;
		LCD_CMD = 0xBA;
		LCD_DATA = 0X01; //控制LCD方向

		Ssd1963_Set_BackLight(255); //背光设置
	}
	Set_Display_Mode(0); //初始化为竖屏
	LCD_Clear(WHITE);	 //清屏白色
}

/****************************************************************************
* 名    称: void LCD_Fill_onecolor(u16 sx,u16 sy,u16 ex,u16 ey,u16 color)  启*明@欣#欣
* 功    能：在指定区域内填充单个颜色
* 入口参数：(sx,sy),(ex,ey):填充矩形对角坐标
            color:要填充的颜色
* 返回参数：无
* 说    明：区域大小为:(ex-sx+1)*(ey-sy+1)  ?B
****************************************************************************/
void LCD_Fill_onecolor(u16 sx, u16 sy, u16 ex, u16 ey, u16 color)
{
	u16 i, j;
	u16 nlen = 0;

	nlen = ex - sx + 1;
	for (i = sy; i <= ey; i++)
	{
		LCD_SetCursor(sx, i); //设置光标位置
		LCD_WriteGRAM();	  //开始写入GRAM
		for (j = 0; j < nlen; j++)
			LCD_DATA = color; //设置光标位置
	}
}

/****************************************************************************
* 名    称: void LCD_Draw_Picture(u16 sx,u16 sy,u16 ex,u16 ey,u16 *color)
* 功    能：在指定区域内画入图片
* 入口参数：(sx,sy),(ex,ey):填充矩形对角坐标
            color:要填充的图片像素颜色数组
* 返回参数：无
* 说    明：区域大小为:(ex-sx+1)*(ey-sy+1)  ?B
****************************************************************************/
void LCD_Draw_Picture(u16 sx, u16 sy, u16 ex, u16 ey, u16 *color)
{
	u16 height, width;
	u16 i, j;
	width = ex - sx + 1;  //得到图片的宽度
	height = ey - sy + 1; //得到图片的高度
	for (i = 0; i < height; i++)
	{
		LCD_SetCursor(sx, sy + i); //设置光标位置
		LCD_WriteGRAM();		   //开始写入GRAM
		for (j = 0; j < width; j++)
			LCD_DATA = color[i * height + j]; //写入颜色值
	}
}

/****************************************************************************
* 名    称: void LCD_DisplayChar(u16 x,u16 y,u8 num,u8 size)
* 功    能：在指定位置显示一个字符
* 入口参数：x,y:起始坐标
            word:要显示的字符:abcdefg1234567890...
            size:字体大小 12/16/24
* 返回参数：无
* 说    明：该字模取模方向为先从左到右，再从上到下  低位在前  ?B
****************************************************************************/
void LCD_DisplayChar(u16 x, u16 y, u8 word, u8 size)
{
	u8 bytenum, bytedata, a, b;

	u16 xmid = x; //存储初始X值(位置)

	if (size == 12)
		bytenum = 12; //从字库数组中可知道每种字体单个字符所占的字节数
	else if (size == 16)
		bytenum = 16;
	else if (size == 24)
		bytenum = 48;
	else
		return; //其他字体退出

	word = word - ' '; //字库数组是按ASCII表排列
					   //cfont.h中字库是从空格开始的 空格就是第一个元素 其他字符的ASCII码减去空格后就得到在数组中的偏移值(位置)
	for (b = 0; b < bytenum; b++)
	{
		if (size == 12)
			bytedata = char_1206[word][b]; //调用1206字体
		else if (size == 16)
			bytedata = char_1608[word][b]; //调用1608字体
		else if (size == 24)
			bytedata = char_2412[word][b]; //调用2412字体

		for (a = 0; a < 8; a++)
		{
			if (bytedata & 0x01)
				LCD_Color_DrawPoint(x, y, BRUSH_COLOR); //由于子模是低位在前 所以先从低位判断  为1时显示画笔颜色
			else
				LCD_Color_DrawPoint(x, y, BACK_COLOR); //0时显示背景颜色
			bytedata >>= 1;							   //低位判断完 依次往高位判断
			x++;									   //显示完一位 往下一位显示
			if ((x - xmid) == size / 2)				   //x方向超出字体大小 如：16字体 实际是 08*16的点阵  故这边 size/2
			{
				x = xmid; //从初始X位置在写下一行
				y++;	  //上一行写完 从下一行再写
				break;	  //跳出for(a=0;a<8;a++)循环
			}
		}
	}
}

/****************************************************************************
* 名    称: void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p)
* 功    能：显示字符串
* 入口参数：x,y:起点坐标
*           size:字体大小
*           *p:字符串起始地址	
* 返回参数：无
* 说    明：  ?B
****************************************************************************/
void LCD_DisplayString(u16 x, u16 y, u8 size, u8 *p)
{
	while ((*p >= ' ') && (*p <= '~')) //只显示“ ”到“~”之间的字符
	{
		LCD_DisplayChar(x, y, *p, size);
		x += size / 2;
		if (x >= lcd_width)
			break;
		p++;
	}
}

/****************************************************************************
* 名    称: void LCD_DisplayString(u16 x,u16 y,u8 size,u8 *p)
* 功    能：显示自定义字符串
* 入口参数：x,y:起点坐标
*           width,height:区域大小  
*           size:字体大小
*           *p:字符串起始地址	
*           brushcolor：自定义画笔颜色
*           backcolor： 自定义背景颜色
* 返回参数：无
* 说    明：  ?B
****************************************************************************/
void LCD_DisplayString_color(u16 x, u16 y, u8 size, u8 *p, u16 brushcolor, u16 backcolor)
{
	u16 bh_color, bk_color;

	bh_color = BRUSH_COLOR; //暂存画笔颜色
	bk_color = BACK_COLOR;	//暂存背景颜色

	BRUSH_COLOR = brushcolor;
	BACK_COLOR = backcolor;

	LCD_DisplayString(x, y, size, p);

	BRUSH_COLOR = bh_color; //不改变系统颜色
	BACK_COLOR = bk_color;
}

//a^n函数，返回值:a^n次方
u32 Counter_Power(u8 a, u8 n)
{
	u32 mid = 1;
	while (n--)
		mid *= a;
	return mid;
}

/****************************************************************************
* 名    称: void LCD_DisplayNum(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
* 功    能：在指定位置显示一串数字
* 入口参数：x,y:起点坐标
            value:数值;	 
            len:长度(设置显示的位数)
            size:字体大小
            mode: 0：高位为0不显示
                  1：高位为0根据len长度补显示几个0
* 返回参数：无
* 说    明：  ?B
****************************************************************************/
void LCD_DisplayNum(u16 x, u16 y, u32 value, u8 len, u8 size, u8 mode)
{
	u8 t, numtemp;
	u8 value_num; //数值总共几位数
	u32 value_mid;

	value_mid = value; //计算位数时不影响要显示的数值大小
	for (value_num = 0; value_mid > 0; value_num++)
	{
		value_mid /= 10;
	} //执行完for，就知道要显示的数值为几位数

	if (value_num > len) //数值位数大于设置位数，即显示区域不够 显示错误
	{
		LCD_DisplayString(x, y, size, (u8 *)"ERROR");
		return; //退出函数
	}
	else
	{
		for (t = 0; t < len; t++)
		{
			if (t < (len - value_num))
			{
				if (mode)
					LCD_DisplayChar(x + (size / 2) * t, y, '0', size);
				else
					LCD_DisplayChar(x + (size / 2) * t, y, ' ', size);
			}
			else
			{
				numtemp = (value / Counter_Power(10, len - t - 1)) % 10; //取出各位数值
				LCD_DisplayChar(x + (size / 2) * t, y, numtemp + '0', size);
			}
		}
	}
}

/****************************************************************************
* 名    称: void LCD_DisplayNum_color(u16 x,u16 y,u32 num,u8 len,u8 size,u8 mode)
* 功    能：在指定位置显示一串自定义颜色的数字  启#明*欣&欣
* 入口参数：x,y:起点坐标
            num:数值;	 
            len:长度(即要显示的位数)
            size:字体大小
            mode: 0：高位为0不显示
                  1：高位为0显示0
            brushcolor：自定义画笔颜色
            backcolor： 自定义背景颜色
* 返回参数：无
* 说    明：  ?B
****************************************************************************/
void LCD_DisplayNum_color(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode, u16 brushcolor, u16 backcolor)
{
	u16 bh_color, bk_color;

	bh_color = BRUSH_COLOR; //暂存画笔颜色
	bk_color = BACK_COLOR;	//暂存背景颜色

	BRUSH_COLOR = brushcolor;
	BACK_COLOR = backcolor;

	LCD_DisplayNum(x, y, num, len, size, mode);

	BRUSH_COLOR = bh_color; //不改变系统颜色
	BACK_COLOR = bk_color;
}
