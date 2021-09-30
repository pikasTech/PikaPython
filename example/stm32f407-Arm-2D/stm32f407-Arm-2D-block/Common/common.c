#include "common.h"

/*********************************************************************************
************************启明欣欣 STM32F407核心开发板******************************
**********************************************************************************
* 文件名称: common.c                                                             *
* 文件简述：各个工程所需调用的公共文件                                           *
* 创建日期：2015.03.03                                                           *
* 版    本：V1.0                                                                 *
* 作    者：Clever                                                               *
* 说    明：包涵数据类型定义、IO口位定义、位段定义与延时函数定义                 * 
**********************************************************************************
*********************************************************************************/	  

/****************************************************************************
* 名    称: void GPIO_group_OUT(_gpio_group *group,u16 outdata)
* 功    能：使用随意16个IO口组成一个16位并行输出口
* 入口参数：*group： 任意16个IO口为元素的结构体指针
            outdata: 16位输出数值
* 返回参数：无
* 说    明：outdata从高位开始赋值
****************************************************************************/
void GPIO_group_OUT(_gpio_group *group,u16 outdata)
{
  u8 t;
	for(t=0;t<16;t++)
    {               
        if((outdata&0x8000)>>15)  
				{
						switch(t)
						{
								case 0:	   group->data15=1; break;
								case 1:	   group->data14=1; break;
								case 2:	   group->data13=1; break;
								case 3:	   group->data12=1; break;
								case 4:	   group->data11=1; break;
								case 5:	   group->data10=1; break;
								case 6:	   group->data9=1;  break;
								case 7:	   group->data8=1;  break;
								case 8:	   group->data7=1;  break;
								case 9:	   group->data6=1;  break;
								case 10:	 group->data5=1;  break;
								case 11:	 group->data4=1;  break;
								case 12:	 group->data3=1;  break;
								case 13:	 group->data2=1;  break;
								case 14:	 group->data1=1;  break;
								case 15:	 group->data0=1;  break;
						}
				}
				else
				{
				  switch(t)
						{
								case 0:	   group->data15=0; break;
								case 1:	   group->data14=0; break;
								case 2:	   group->data13=0; break;
								case 3:	   group->data12=0; break;
								case 4:	   group->data11=0; break;
								case 5:	   group->data10=0; break;
								case 6:	   group->data9=0;  break;
								case 7:	   group->data8=0;  break;
								case 8:	   group->data7=0;  break;
								case 9:	   group->data6=0;  break;
								case 10:	 group->data5=0;  break;
								case 11:	 group->data4=0;  break;
								case 12:	 group->data3=0;  break;
								case 13:	 group->data2=0;  break;
								case 14:	 group->data1=0;  break;
								case 15:	 group->data0=0;  break;
						}
				}
     outdata<<=1; 	
	  }
}

/****************************************************************************
* 名    称: void GPIO_bits_OUT(GPIO_TypeDef* GPIOx, u8 start_bit, u8 bit_size,u16 outdata)
* 功    能：位段操作实现，同一IO口的几位并行输出操作
* 入口参数：* GPIOx：  对应的IO口
*           start_bit: 并行输出的起始位
*           bit_size:  要并行输出的位数
* 返回参数：无
* 说    明：start_bit: 0~14
            bit_size:  1~16 
            bit_size<=16-start_bit
****************************************************************************/
void GPIO_bits_OUT(GPIO_TypeDef* GPIOx, u8 start_bit, u8 bit_size,u16 outdata)
{
  u8 i=0;
	u16 bu1=0;u16 middata=1;

	if( bit_size>(16-start_bit) ) 
     bit_size=16-start_bit;
	
	i=start_bit;
	if(i>0)
		 {
			 while(i--)
         { bu1+=middata; middata*=2;}
		 }
   
	 GPIOx->ODR&=(  ( (0xffff<<bit_size) <<start_bit ) |bu1   ); 
	 GPIOx->ODR|=(outdata<<start_bit);		 
}

/*****************************************************************************
**********************以下代码参考网上，仅供学习参考**************************
*****************************************************************************/
/****************************************************************************
* 名    称: void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
* 功    能：时钟系统配置函数
* 入口参数：Fvco:VCO频率
            SYSCLK:系统时钟频率
            Fusb:USB,SDIO,RNG等的时钟频率
            Fs:PLL输入时钟频率,可以是HSI,HSE等. 
            plln:主PLL倍频系数(PLL倍频),取值范围:64~432.
            pllm:主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
            pllp:系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
            pllq:USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
* 返回参数：无
* 说    明：Fvco=Fs*(plln/pllm);
            SYSCLK=Fvco/pllp=Fs*(plln/(pllm*pllp));
            Fusb=Fvco/pllq=Fs*(plln/(pllm*pllq));外部晶振为8M的时候,推荐值:plln=336,pllm=8,pllp=2,pllq=7.
            得到:Fvco=8*(336/8)=336Mhz
            SYSCLK=336/2=168Mhz
            Fusb=336/7=48Mhz
****************************************************************************/
void Stm32_Clock_Init(u32 plln,u32 pllm,u32 pllp,u32 pllq)
{
    HAL_StatusTypeDef ret = HAL_OK;
    RCC_OscInitTypeDef RCC_OscInitStructure; 
    RCC_ClkInitTypeDef RCC_ClkInitStructure;
    
    __HAL_RCC_PWR_CLK_ENABLE(); //使能PWR时钟
    
    //下面这个设置用来设置调压器输出电压级别，以便在器件未以最大频率工作
    //时使性能与功耗实现平衡。
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);//设置调压器输出电压级别1
    
    RCC_OscInitStructure.OscillatorType=RCC_OSCILLATORTYPE_HSE;    //时钟源为HSE
    RCC_OscInitStructure.HSEState=RCC_HSE_ON;                      //打开HSE
    RCC_OscInitStructure.PLL.PLLState=RCC_PLL_ON;//打开PLL
    RCC_OscInitStructure.PLL.PLLSource=RCC_PLLSOURCE_HSE;//PLL时钟源选择HSE
    RCC_OscInitStructure.PLL.PLLM=pllm; //主PLL和音频PLL分频系数(PLL之前的分频),取值范围:2~63.
    RCC_OscInitStructure.PLL.PLLN=plln; //主PLL倍频系数(PLL倍频),取值范围:64~432.  
    RCC_OscInitStructure.PLL.PLLP=pllp; //系统时钟的主PLL分频系数(PLL之后的分频),取值范围:2,4,6,8.(仅限这4个值!)
    RCC_OscInitStructure.PLL.PLLQ=pllq; //USB/SDIO/随机数产生器等的主PLL分频系数(PLL之后的分频),取值范围:2~15.
    ret=HAL_RCC_OscConfig(&RCC_OscInitStructure);//初始化
	
    if(ret!=HAL_OK) while(1);
    
    //选中PLL作为系统时钟源并且配置HCLK,PCLK1和PCLK2
    RCC_ClkInitStructure.ClockType=(RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStructure.SYSCLKSource=RCC_SYSCLKSOURCE_PLLCLK;//设置系统时钟时钟源为PLL
    RCC_ClkInitStructure.AHBCLKDivider=RCC_SYSCLK_DIV1;//AHB分频系数为1
    RCC_ClkInitStructure.APB1CLKDivider=RCC_HCLK_DIV4; //APB1分频系数为4
    RCC_ClkInitStructure.APB2CLKDivider=RCC_HCLK_DIV2; //APB2分频系数为2
    ret=HAL_RCC_ClockConfig(&RCC_ClkInitStructure,FLASH_LATENCY_5);//同时设置FLASH延时周期为5WS，也就是6个CPU周期。
		
    if(ret!=HAL_OK) while(1);

	 //STM32F405x/407x/415x/417x Z版本的器件支持预取功能
	if (HAL_GetREVID() == 0x1001)
	{
		__HAL_FLASH_PREFETCH_BUFFER_ENABLE();  //使能flash预取
	}
}


////THUMB指令不支持汇编内联
////采用如下方法实现执行汇编指令WFI  
//__asm void WFI_SET(void)
//{
//	WFI;		  
//}
////关闭所有中断(但是不包括fault和NMI中断)
//__asm void INTX_DISABLE(void)
//{
//	CPSID   I
//	BX      LR	  
//}
////开启所有中断
//__asm void INTX_ENABLE(void)
//{
//	CPSIE   I
//	BX      LR  
//}
////设置栈顶地址
////addr:栈顶地址
//__asm void MSR_MSP(u32 addr) 
//{
//	MSR MSP, r0 			//set Main Stack value
//	BX r14
//}

//利用系统滴答定时，编写的延时函数

static u8  fac_us=0; //us延时倍乘数			   

/****************************************************************************
* 名    称: delay_init()
* 功    能：延时函数初始化
* 入口参数：无
* 返回参数：无
* 说    明：
****************************************************************************/
void delay_init()
{
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);//SysTick频率为HCLK
	fac_us=SYSCLK;					
}								    

/****************************************************************************
* 名    称: void delay_us(u32 nus)
* 功    能：延时nus
* 入口参数：要延时的微妙数
* 返回参数：无
* 说    明：nus的值,不要大于798915us
****************************************************************************/
void delay_us(u32 nus)
{		
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD的值	    	 
	ticks=nus*fac_us; 						//需要的节拍数 
	told=SysTick->VAL;        				//刚进入时的计数器值
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;	//这里注意一下SYSTICK是一个递减的计数器就可以了.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;			//时间超过/等于要延迟的时间,则退出.
		}  
	}; 
}

/****************************************************************************
* 名    称: void delay_ms(u16 nms)
* 功    能：延时nms
* 入口参数：要延时的毫妙数
* 返回参数：无
* 说    明： 
****************************************************************************/
void delay_ms(u16 nms)
{
	u32 i;
	for(i=0;i<nms;i++) delay_us(1000);
} 

			 
