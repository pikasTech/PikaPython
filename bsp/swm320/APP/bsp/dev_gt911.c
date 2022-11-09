#include "main.h"
#include "lv_conf.h"

//GT911配置参数表
//配置文件的版本号(新下发的配置版本号大于原版本，或等于原版本号但配置内容有变化时保存，
//版本号版本正常范围：'A'~'Z',发送 0x00 则将版本号初始化为'A')
const uint8_t GT911_CfgTab[] = {
    0x41,0x20,0x03,0xe0,0x01,0x0A,0x0D,0x00,0x01,0x08,0x28,0x05,0x50,0x32,0x03,0x05,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x89,0x2A,0x0B,0x17,0x15,
    0x31,0x0D,0x00,0x00,0x03,0xBA,0x04,0x2D,0x00,0x00,0x00,0x00,0x00,0x03,0x64,0x32,
    0x00,0x00,0x00,0x0F,0x94,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,0x8D,0x13,0x00,0x5C,
    0x1E,0x00,0x3C,0x30,0x00,0x29,0x4C,0x00,0x1F,0x78,0x00,0x1F,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x16,0x14,0x12,0x10,0x0E,0x0C,0x0A,0x08,0x06,0x04,0x02,0xFF,0xFF,0xFF,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,
    0x04,0x06,0x08,0x0A,0x0F,0x10,0x12,0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,
    0x24,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x99,0x01};

#define DEV_ADDR 0x5D

#define REG_CTRL 0X8040  //GT911控制寄存器
#define REG_CFGS 0X8047  //GT911配置寄存器
#define REG_CHECK 0X80FF //GT911校验和寄存器
#define REG_PID 0X8140   //GT911产品ID寄存器

#define REG_GSTID 0X814E //当前检测到的触摸情况
#define REG_TP1 0X8150   //第一个触摸点数据地址
#define REG_TP2 0X8158   //第二个触摸点数据地址
#define REG_TP3 0X8160   //第三个触摸点数据地址
#define REG_TP4 0X8168   //第四个触摸点数据地址
#define REG_TP5 0X8170   //第五个触摸点数据地址

#define REG_GSTID_READY_Pos 7 // COUNT域是否有效
#define REG_GSTID_READY_Msk (0x01 << REG_GSTID_READY_Pos)
#define REG_GSTID_COUNT_Pos 0 // 触摸个数，0~5
#define REG_GSTID_COUNT_Msk (0x0F << REG_GSTID_COUNT_Pos)

void delay(void)
{
    for (uint32_t i = 0; i < 150; i++)
        __NOP();
}

/******************************************************************************************************************************************
* 函数名称:	GT911_WrReg()
* 功能说明:	写GT911寄存器
* 输    入: uint16_t reg		要写的寄存器
*			uint8_t *buf		要写入寄存器的数据
*			uint8_t len			要写的数据个数
* 输    出: uint8_t				1 写入成功   0 写入失败
* 注意事项: 无
******************************************************************************************************************************************/
static uint8_t GT911_WrReg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ack;

    ack = I2C_Start(I2C0, (DEV_ADDR << 1) | 0);
    if (ack == 0)
        goto wr_fail;

    ack = I2C_Write(I2C0, reg >> 8);
    if (ack == 0)
        goto wr_fail;

    ack = I2C_Write(I2C0, reg & 0XFF);
    if (ack == 0)
        goto wr_fail;

    for (i = 0; i < len; i++)
    {
        ack = I2C_Write(I2C0, buf[i]);
        if (ack == 0)
            goto wr_fail;
    }

    I2C_Stop(I2C0);
    delay();
    return 1;

wr_fail:
    I2C_Stop(I2C0);
    delay();
    return 0;
}

/******************************************************************************************************************************************
* 函数名称:	GT911_RdReg()
* 功能说明:	读GT911寄存器
* 输    入: uint16_t reg		要读的寄存器
*			uint8_t *buf		读出的寄存器值写入数组
*			uint8_t len			要读的数据个数
* 输    出: uint8_t				1 读取成功   0 读取失败
* 注意事项: 无
******************************************************************************************************************************************/
static uint8_t GT911_RdReg(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ack;

    ack = I2C_Start(I2C0, (DEV_ADDR << 1) | 0);
    if (ack == 0)
        goto rd_fail;

    ack = I2C_Write(I2C0, reg >> 8);
    if (ack == 0)
        goto rd_fail;

    ack = I2C_Write(I2C0, reg & 0XFF);
    if (ack == 0)
        goto rd_fail;

    ack = I2C_Start(I2C0, (DEV_ADDR << 1) | 1); //ReStart
    if (ack == 0)
        goto rd_fail;

    for (i = 0; i < len - 1; i++)
    {
        buf[i] = I2C_Read(I2C0, 1);
    }
    buf[i] = I2C_Read(I2C0, 0);

    I2C_Stop(I2C0);
    delay();
    return 1;

rd_fail:
    I2C_Stop(I2C0);
    delay();
    return 0;
}

_m_tp_dev tp_dev;
const uint16_t GT911_TPX_TBL[5] = {REG_TP1, REG_TP2, REG_TP3, REG_TP4, REG_TP5};
/******************************************************************************************************************************************
* 函数名称:	GT911_Scan()
* 功能说明:	扫描触摸屏识别触摸(采用查询方式)
* 输    入: uint16_t *x 			读取到的触摸点横坐标
*			uint16_t *y 		读取到的触摸点纵坐标
*			uint8_t count 		x和y最多能存储几个触摸点的坐标数据
* 输    出: uint8_t				实际扫描到的触摸点个数
* 注意事项: 无
******************************************************************************************************************************************/
void GT911_Scan(void)
{
    uint8_t buf[4], touch_status = 0;
    uint8_t temp;
    uint8_t tempsta;
    
    GT911_RdReg(REG_GSTID, &touch_status, 1);
    if (touch_status & 0x80 && ((touch_status & 0x0F) < 6))
    {
        temp = 0;
        GT911_WrReg(REG_GSTID, &temp, 1); // 清除READY标志
    }
    if ((touch_status & 0x0F) && ((touch_status & 0x0F) < 6))
    {
        temp = 0xFF << (touch_status & 0x0F); //将点的个数转换为1的位数,匹配tp_dev.sta定义
        tempsta = tp_dev.sta;                 //保存当前的tp_dev.sta值
        tp_dev.sta = (~temp) | TP_PRES_DOWN;
        tp_dev.x[4] = tp_dev.x[0]; //保存触点0的数据
        tp_dev.y[4] = tp_dev.y[0];
        for (uint32_t i = 0; i < 5; i++)
        {
            if (tp_dev.sta & (1 << i)) //触摸有效?
            {
//                for (uint32_t i = 0; i < 150; i++)
//                    __NOP();
                GT911_RdReg(GT911_TPX_TBL[i], buf, 4);   //读取XY坐标值
                tp_dev.x[i] = (((uint16_t)buf[3] << 8) + buf[2]);
                tp_dev.y[i] = (((uint16_t)buf[1] << 8) + buf[0]);
            }
        }
        if (tp_dev.x[0] > LV_HOR_RES_MAX || tp_dev.y[0] > LV_VER_RES_MAX) //非法数据(坐标超出了)
        {
            if ((touch_status & 0x0F) > 1) //有其他点有数据,则复第二个触点的数据到第一个触点.
            {
                tp_dev.x[0] = tp_dev.x[1];
                tp_dev.y[0] = tp_dev.y[1];
            }
            else //非法数据,则忽略此次数据(还原原来的)
            {
                tp_dev.x[0] = tp_dev.x[4];
                tp_dev.y[0] = tp_dev.y[4];
                touch_status = 0x80;
                tp_dev.sta = tempsta; //恢复tp_dev.sta
            }
        }
    }
    if ((touch_status & 0x8F) == 0x80) //无触摸点按下
    {
        if (tp_dev.sta & TP_PRES_DOWN) //之前是被按下的
        {
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
            tp_dev.sta &= ~(1 << 7); //标记按键松开
        }
        else //之前就没有被按下
        {
            tp_dev.x[0] = 0xffff;
            tp_dev.y[0] = 0xffff;
            tp_dev.sta &= 0XE0; //清除点有效标记
        }
    }
}

/******************************************************************************************************************************************
* 函数名称:	GT911_Init()
* 功能说明:	GT911初始化
* 输    入: 无
* 输    出: uint8_t				1 初始化成功   0 初始化失败
* 注意事项: 无
******************************************************************************************************************************************/
uint8_t GT911_Init(void)
{
    uint32_t i;
    uint8_t temp[186] = {0};
    I2C_InitStructure I2C_initStruct;

    PORT_Init(PORTA, PIN10, FUNMUX0_I2C0_SCL, 1); //GPIOA.10配置为I2C0 SCL引脚
    PORT->PORTA_PULLU |= (1 << PIN10);            //必须使能上拉，用于模拟开漏
    PORT_Init(PORTA, PIN9, FUNMUX1_I2C0_SDA, 1);  //GPIOA.9配置为I2C0 SDA引脚
    PORT->PORTA_PULLU |= (1 << PIN9);             //必须使能上拉，用于模拟开漏

    I2C_initStruct.Master = 1;
    I2C_initStruct.Addr7b = 1;
    I2C_initStruct.MstClk = 400000;
    I2C_initStruct.MstIEn = 0;
    I2C_Init(I2C0, &I2C_initStruct);
    I2C_Open(I2C0);

    GPIO_Init(GPIOB, PIN7, 0, 0, 1); // 输入，开启下拉。复位时INT为低，选择0xBA作为地址

    // GPIO_Init(GPIOM, PIN20, 1, 0, 0);    // GT911复位
    GPIO_ClrBit(GPIOM, PIN20); // 复位
    swm_delay(1);
    GPIO_SetBit(GPIOM, PIN20); // 释放复位
    swm_delay(1);

    GT911_RdReg(REG_PID, temp, 4);
    if (strcmp((char *)temp, "911") == 0) // ID==911
    {
        temp[0] = 0X02;
        GT911_WrReg(REG_CTRL, temp, 1); // 软复位
        swm_delay(10);
        
// //更新触摸芯片配置
//        {
//            GT911_WrReg(REG_CFGS, (uint8_t *)GT911_CfgTab, sizeof(GT911_CfgTab)); //发送寄存器配置
//            GT911_RdReg(REG_CFGS, temp, sizeof(GT911_CfgTab));
//        }

        temp[0] = 0X00;
        GT911_WrReg(REG_CTRL, temp, 1); // 释放软复位

        return 1;
    }

    return 0;
}
