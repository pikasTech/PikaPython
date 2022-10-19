#include "main.h"
#include "lv_conf.h"

//GT9147配置参数表
//第一个字节为版本号(0X60),必须保证新的版本号大于等于GT9147内部
//flash原有版本号,才会更新配置.
static const uint8_t GT9147_CFG_TBL[] = {
    0x61, 0xE0, 0x01, 0x10, 0x01, 0x05, 0x0C, 0x00, 0x01, 0x08,
    0x28, 0x05, 0x50, 0x32, 0x03, 0x05, 0x00, 0x00, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x89, 0x28, 0x0A,
    0x17, 0x15, 0x31, 0x0D, 0x00, 0x00, 0x02, 0x9B, 0x03, 0x25,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00,
    0x00, 0x0F, 0x94, 0x94, 0xC5, 0x02, 0x07, 0x00, 0x00, 0x04,
    0x8D, 0x13, 0x00, 0x5C, 0x1E, 0x00, 0x3C, 0x30, 0x00, 0x29,
    0x4C, 0x00, 0x1E, 0x78, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x08, 0x0A, 0x0C, 0x0E, 0x10, 0x12, 0x14, 0x16,
    0x18, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0x00, 0x02, 0x04, 0x05, 0x06, 0x08, 0x0A, 0x0C,
    0x0E, 0x1D, 0x1E, 0x1F, 0x20, 0x22, 0x24, 0x28, 0x29, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF};

void i2c_init(void)
{
    I2C_InitStructure I2C_initStruct;

    PORT_Init(PORTA, PIN8, FUNMUX0_I2C0_SCL, 1);
    PORT->PORTA_PULLU |= (1 << PIN8);
    PORT_Init(PORTA, PIN5, FUNMUX1_I2C0_SDA, 1);
    PORT->PORTA_PULLU |= (1 << PIN5);

    I2C_initStruct.Master = 1;
    I2C_initStruct.Addr7b = 1;
    I2C_initStruct.MstClk = 400000;
    I2C_initStruct.MstIEn = 0;
    I2C_Init(I2C0, &I2C_initStruct);

    I2C_Open(I2C0);
}

uint8_t gt9147_write_reg(uint16_t reg, uint8_t *data, uint8_t len)
{
    I2C0->MSTDAT = GT_CMD_WR; // 发送器件地址+写命令
    I2C0->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |
                   (1 << I2C_MSTCMD_WR_Pos); // 发送起始位和从机地址
    while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
        ;                                    // 等待发送完成
    if (I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk) // 判断是否接收到应答位
    {
        return 1;
    }

    I2C0->MSTDAT = reg >> 8; // 写入数据
    I2C0->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
    while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
        ;
    if (I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk) // 读应答位
    {
        return 1;
    }
    I2C0->MSTDAT = reg & 0xFF; // 写入数据
    I2C0->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
    while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
        ;
    if (I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk) // 读应答位
    {
        return 1;
    }

    for (uint32_t i = 0; i < len; i++)
    {
        I2C0->MSTDAT = *data++; // 写入数据
        I2C0->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
        while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
            ;
        if (I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk) // 读应答位
        {
            return 1;
        }
    }

    I2C0->MSTCMD = (1 << I2C_MSTCMD_STO_Pos);
    while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
        ;
    if (I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
    {
        return 1;
    }
    return 0;
}

uint8_t gt9147_read_reg(uint16_t reg, uint8_t *data, uint8_t len)
{
    I2C0->MSTDAT = GT_CMD_WR; // 发送器件地址+写命令
    I2C0->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |
                   (1 << I2C_MSTCMD_WR_Pos); // 发送起始位和从机地址
    while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
        ;                                    // 等待发送完成
    if (I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk) // 判断是否接收到应答位
    {
        return 1;
    }

    I2C0->MSTDAT = reg >> 8; // 写入数据
    I2C0->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
    while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
        ;
    if (I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk) // 读应答位
    {
        return 1;
    }
    I2C0->MSTDAT = reg & 0xFF; // 写入数据
    I2C0->MSTCMD = (1 << I2C_MSTCMD_WR_Pos);
    while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
        ;
    if (I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk) // 读应答位
    {
        return 1;
    }
    I2C0->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |
                   (1 << I2C_MSTCMD_WR_Pos); // 发送Restart
    while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
        ;

    I2C0->MSTDAT = GT_CMD_RD; // 发送器件地址+读命令
    I2C0->MSTCMD = (1 << I2C_MSTCMD_STA_Pos) |
                   (1 << I2C_MSTCMD_WR_Pos); // 发送Restart
    while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
        ;
    if (I2C0->MSTCMD & I2C_MSTCMD_RXACK_Msk)
    {
        return 1;
    }
    // 注意，这里只需循环（len - 1）次，因为最后一个字节不需要应答
    for (uint32_t i = 0; i < len - 1; i++)
    {
        I2C0->MSTCMD = (1 << I2C_MSTCMD_RD_Pos) |
                       (0 << I2C_MSTCMD_ACK_Pos); // 产生ACK信号
        while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
            ; // 等待接收完成
        *data++ = I2C0->MSTDAT;
    }

    I2C0->MSTCMD = (1 << I2C_MSTCMD_RD_Pos) |
                   (1 << I2C_MSTCMD_ACK_Pos) |
                   (1 << I2C_MSTCMD_STO_Pos); // 发送NACK响应,完成后发送STOP
    while (I2C0->MSTCMD & I2C_MSTCMD_TIP_Msk)
        ;                   // 等待接收完成
    *data++ = I2C0->MSTDAT; // 最后一个字节，先发送命令，再读取即可
    return 0;
}
//发送GT9147配置参数
//mode:0,参数不保存到flash
//     1,参数保存到flash
void gt9147_send_cfg(uint8_t mode)
{
    uint8_t buf[2];
    buf[0] = 0;
    buf[1] = mode; //是否写入到GT9147 FLASH?  即是否掉电保存
    for (uint32_t i = 0; i < sizeof(GT9147_CFG_TBL); i++)
        buf[0] += GT9147_CFG_TBL[i]; //计算校验和
    buf[0] = (~buf[0]) + 1;
    gt9147_write_reg(GT_CFGS_REG, (uint8_t *)GT9147_CFG_TBL, sizeof(GT9147_CFG_TBL)); //发送寄存器配置
    swm_delay(5);
    gt9147_write_reg(GT_CHECK_REG, buf, 2); //写入校验和,和配置更新标记
}
_m_tp_dev tp_dev;
void gt9147_init(void)
{
    // 第一阶段设置端口，并拉低两个端口
    GPIO_Init(GPIO_GT_RST, PIN_GT_RST, 1, 0, 0); // 复位脚 输出
    GPIO_ClrBit(GPIO_GT_RST, PIN_GT_RST);
    GPIO_Init(GPIO_GT_INT, PIN_GT_INT, 1, 0, 0); // 中断脚 输出
    GPIO_ClrBit(GPIO_GT_INT, PIN_GT_INT);
    i2c_init();
    swm_delay(10);
    // 第二阶段复位芯片
    GPIO_SetBit(GPIO_GT_RST, PIN_GT_RST); // 拉高开始复位芯片
    swm_delay(10);
    // 第三阶段设置中断引脚为 中断功能
    GPIO_Init(GPIO_GT_INT, PIN_GT_INT, 0, 0, 0);

    swm_delay(100);
    uint8_t temp[200];
    // gt9147_read_reg(GT_PID_REG, temp, 4); //读取产品ID
    // temp[4] = 0;
    // printf("Product ID:%s\r\n", temp);

    temp[0] = 0x02;
    gt9147_write_reg(GT_CTRL_REG, temp, 1); //软复位GT9147
    swm_delay(10);
    gt9147_read_reg(GT_CFGS_REG, temp, 1);
    // printf("default ver:0x%x\r\n",temp[0]);
    if (temp[0] < 0X61) //默认版本比较低，需要更新flash配置
    {
        gt9147_send_cfg(1); //更新并保存配置
    }
    swm_delay(10);
    temp[0] = 0x00;
    gt9147_write_reg(GT_CTRL_REG, temp, 1); //结束复位

    // swm_delay(100);
    // gt9147_read_reg(GT_CFGS_REG, temp, 186);
    // printf("read register: ");
    // for (uint32_t i = 0; i < 186; i++)
    // {
    //     if ((i % 10) == 0)
    //     {
    //         printf("\r\n");
    //     }

    //     printf("0x%x,", temp[i]);
    // }
    // printf("\r\n");

    timer_init(&tp_dev.timer, 100, gt9147_scan, LOOP_FOREVER, NULL);
    timer_start(&tp_dev.timer);
}

const uint16_t GT9147_TPX_TBL[5] = {GT_TP1_REG, GT_TP2_REG, GT_TP3_REG, GT_TP4_REG, GT_TP5_REG};
//扫描触摸屏(采用查询方式)
void gt9147_scan(void)
{
    uint8_t buf[4], touch_status = 0;
    uint8_t temp;
    uint8_t tempsta;

    gt9147_read_reg(GT_GSTID_REG, &touch_status, 1); //读取触摸点的状态
    if (touch_status & 0x80 && ((touch_status & 0x0F) < 6))
    {
        temp = 0;
        gt9147_write_reg(GT_GSTID_REG, &temp, 1); //清标志
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
                swm_delay(5);
                gt9147_read_reg(GT9147_TPX_TBL[i], buf, 4); //读取XY坐标值
                tp_dev.x[i] = (((uint16_t)buf[1] << 8) + buf[0]);
                tp_dev.y[i] = (((uint16_t)buf[3] << 8) + buf[2]);
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
