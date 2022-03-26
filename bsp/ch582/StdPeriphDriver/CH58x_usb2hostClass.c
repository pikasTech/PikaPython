/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH58x_usbhost.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH58x_common.h"
#if DISK_LIB_ENABLE
  #include "CHRV3UFI.H"
#endif

/* 设置HID上传速率 */
__attribute__((aligned(4))) const uint8_t SetupSetU2HIDIdle[] = {0x21, HID_SET_IDLE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
/* 获取HID设备报表描述符 */
__attribute__((aligned(4))) const uint8_t SetupGetU2HIDDevReport[] = {0x81, USB_GET_DESCRIPTOR, 0x00,
                                                                      USB_DESCR_TYP_REPORT, 0x00, 0x00, 0x41, 0x00};
/* 获取HUB描述符 */
__attribute__((aligned(4))) const uint8_t SetupGetU2HubDescr[] = {HUB_GET_HUB_DESCRIPTOR, HUB_GET_DESCRIPTOR,
                                                                  0x00, USB_DESCR_TYP_HUB, 0x00, 0x00, sizeof(USB_HUB_DESCR), 0x00};

__attribute__((aligned(4))) uint8_t U2Com_Buffer[128]; // 定义用户临时缓冲区,枚举时用于处理描述符,枚举结束也可以用作普通临时缓冲区

/*********************************************************************
 * @fn      AnalyzeU2HidIntEndp
 *
 * @brief   从描述符中分析出HID中断端点的地址,如果HubPortIndex是0保存到ROOTHUB，如果是非零值则保存到HUB下结构体
 *
 * @param   buf     - 待分析数据缓冲区地址 HubPortIndex：0表示根HUB，非0表示外部HUB下的端口号
 *
 * @return  端点数
 */
uint8_t AnalyzeU2HidIntEndp(uint8_t *buf, uint8_t HubPortIndex)
{
    uint8_t i, s, l;
    s = 0;

    if(HubPortIndex)
    {
        memset(DevOnU2HubPort[HubPortIndex - 1].GpVar, 0, sizeof(DevOnU2HubPort[HubPortIndex - 1].GpVar)); //清空数组
    }
    else
    {
        memset(ThisUsb2Dev.GpVar, 0, sizeof(ThisUsb2Dev.GpVar)); //清空数组
    }

    for(i = 0; i < ((PUSB_CFG_DESCR)buf)->wTotalLength; i += l) // 搜索中断端点描述符,跳过配置描述符和接口描述符
    {
        if(((PUSB_ENDP_DESCR)(buf + i))->bDescriptorType == USB_DESCR_TYP_ENDP                         // 是端点描述符
           && (((PUSB_ENDP_DESCR)(buf + i))->bmAttributes & USB_ENDP_TYPE_MASK) == USB_ENDP_TYPE_INTER // 是中断端点
           && (((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK))                    // 是IN端点
        {                                                                                              // 保存中断端点的地址,位7用于同步标志位,清0
            if(HubPortIndex)
            {
                DevOnU2HubPort[HubPortIndex - 1].GpVar[s] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
            }
            else
            {
                ThisUsb2Dev.GpVar[s] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK; // 中断端点的地址，可以根据需要保存wMaxPacketSize和bInterval
            }
            PRINT("%02x ", (uint16_t)ThisUsb2Dev.GpVar[s]);
            s++;
            if(s >= 4)
            {
                break; //只分析4个端点
            }
        }
        l = ((PUSB_ENDP_DESCR)(buf + i))->bLength; // 当前描述符长度,跳过
        if(l > 16)
        {
            break;
        }
    }
    PRINT("\n");
    return (s);
}

/*********************************************************************
 * @fn      AnalyzeU2BulkEndp
 *
 * @brief   分析出批量端点,GpVar[0]、GpVar[1]存放上传端点。GpVar[2]、GpVar[3]存放下传端点
 *
 * @param   buf     - 待分析数据缓冲区地址 HubPortIndex：0表示根HUB，非0表示外部HUB下的端口号
 *
 * @return  0
 */
uint8_t AnalyzeU2BulkEndp(uint8_t *buf, uint8_t HubPortIndex)
{
    uint8_t i, s1, s2, l;
    s1 = 0;
    s2 = 2;

    if(HubPortIndex)
    {
        memset(DevOnU2HubPort[HubPortIndex - 1].GpVar, 0, sizeof(DevOnU2HubPort[HubPortIndex - 1].GpVar)); //清空数组
    }
    else
    {
        memset(ThisUsb2Dev.GpVar, 0, sizeof(ThisUsb2Dev.GpVar)); //清空数组
    }

    for(i = 0; i < ((PUSB_CFG_DESCR)buf)->wTotalLength; i += l) // 搜索中断端点描述符,跳过配置描述符和接口描述符
    {
        if((((PUSB_ENDP_DESCR)(buf + i))->bDescriptorType == USB_DESCR_TYP_ENDP)                         // 是端点描述符
           && ((((PUSB_ENDP_DESCR)(buf + i))->bmAttributes & USB_ENDP_TYPE_MASK) == USB_ENDP_TYPE_BULK)) // 是中断端点

        {
            if(HubPortIndex)
            {
                if(((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK)
                {
                    DevOnU2HubPort[HubPortIndex - 1].GpVar[s1++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
                else
                {
                    DevOnU2HubPort[HubPortIndex - 1].GpVar[s2++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
            }
            else
            {
                if(((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_DIR_MASK)
                {
                    ThisUsb2Dev.GpVar[s1++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
                else
                {
                    ThisUsb2Dev.GpVar[s2++] = ((PUSB_ENDP_DESCR)(buf + i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
                }
            }

            if(s1 == 2)
            {
                s1 = 1;
            }
            if(s2 == 4)
            {
                s2 = 3;
            }
        }
        l = ((PUSB_ENDP_DESCR)(buf + i))->bLength; // 当前描述符长度,跳过
        if(l > 16)
        {
            break;
        }
    }
    return (0);
}

/*********************************************************************
 * @fn      InitRootU2Device
 *
 * @brief   初始化指定ROOT-HUB端口的USB设备
 *
 * @param   none
 *
 * @return  错误码
 */
uint8_t InitRootU2Device(void)
{
    uint8_t i, s;
    uint8_t cfg, dv_cls, if_cls;

    PRINT("Reset U2 host port\n");
    ResetRootU2HubPort(); // 检测到设备后,复位相应端口的USB总线
    for(i = 0, s = 0; i < 100; i++)
    { // 等待USB设备复位后重新连接,100mS超时
        mDelaymS(1);
        if(EnableRootU2HubPort() == ERR_SUCCESS)
        { // 使能端口
            i = 0;
            s++;
            if(s > 100)
                break; // 已经稳定连接100mS
        }
    }
    if(i)
    { // 复位后设备没有连接
        DisableRootU2HubPort();
        PRINT("Disable U2 host port because of disconnect\n");
        return (ERR_USB_DISCON);
    }
    SetUsb2Speed(ThisUsb2Dev.DeviceSpeed); // 设置当前USB速度

    PRINT("GetU2DevDescr: ");
    s = CtrlGetU2DeviceDescr(); // 获取设备描述符
    if(s == ERR_SUCCESS)
    {
        for(i = 0; i < ((PUSB_SETUP_REQ)SetupGetU2DevDescr)->wLength; i++)
            PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
        PRINT("\n");

        ThisUsb2Dev.DeviceVID = ((PUSB_DEV_DESCR)U2Com_Buffer)->idVendor; //保存VID PID信息
        ThisUsb2Dev.DevicePID = ((PUSB_DEV_DESCR)U2Com_Buffer)->idProduct;
        dv_cls = ((PUSB_DEV_DESCR)U2Com_Buffer)->bDeviceClass;

        s = CtrlSetUsb2Address(((PUSB_SETUP_REQ)SetupSetUsb2Addr)->wValue);
        if(s == ERR_SUCCESS)
        {
            ThisUsb2Dev.DeviceAddress = ((PUSB_SETUP_REQ)SetupSetUsb2Addr)->wValue; // 保存USB地址

            PRINT("GetU2CfgDescr: ");
            s = CtrlGetU2ConfigDescr();
            if(s == ERR_SUCCESS)
            {
                for(i = 0; i < ((PUSB_CFG_DESCR)U2Com_Buffer)->wTotalLength; i++)
                {
                    PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
                }
                PRINT("\n");
                /* 分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等 */
                cfg = ((PUSB_CFG_DESCR)U2Com_Buffer)->bConfigurationValue;
                if_cls = ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceClass; // 接口类代码

                if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_STORAGE))
                { // 是USB存储类设备,基本上确认是U盘
#ifdef FOR_ROOT_UDISK_ONLY
                    CHRV3DiskStatus = DISK_USB_ADDR;
                    return (ERR_SUCCESS);
                }
                else
                    return (ERR_USB_UNSUPPORT);
#else
                    s = CtrlSetUsb2Config(cfg); // 设置USB设备配置
                    if(s == ERR_SUCCESS)
                    {
                        ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsb2Dev.DeviceType = USB_DEV_CLASS_STORAGE;
                        PRINT("U2 USB-Disk Ready\n");
                        SetUsb2Speed(1); // 默认为全速
                        return (ERR_SUCCESS);
                    }
                }
                else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_PRINTER) && ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceSubClass == 0x01)
                {                               // 是打印机类设备
                    s = CtrlSetUsb2Config(cfg); // 设置USB设备配置
                    if(s == ERR_SUCCESS)
                    {
                        //	需保存端点信息以便主程序进行USB传输
                        ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsb2Dev.DeviceType = USB_DEV_CLASS_PRINTER;
                        PRINT("U2 USB-Print Ready\n");
                        SetUsb2Speed(1); // 默认为全速
                        return (ERR_SUCCESS);
                    }
                }
                else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) && ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceSubClass <= 0x01)
                { // 是HID类设备,键盘/鼠标等
                    //  从描述符中分析出HID中断端点的地址
                    s = AnalyzeU2HidIntEndp(U2Com_Buffer, 0); // 从描述符中分析出HID中断端点的地址
                    PRINT("AnalyzeU2HidIntEndp %02x\n", (uint16_t)s);
                    //  保存中断端点的地址,位7用于同步标志位,清0
                    if_cls = ((PUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceProtocol;
                    s = CtrlSetUsb2Config(cfg); // 设置USB设备配置
                    if(s == ERR_SUCCESS)
                    {
                        //	    					Set_Idle( );
                        //	需保存端点信息以便主程序进行USB传输
                        ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
                        if(if_cls == 1)
                        {
                            ThisUsb2Dev.DeviceType = DEV_TYPE_KEYBOARD;
                            //	进一步初始化,例如设备键盘指示灯LED等
                            PRINT("U2 USB-Keyboard Ready\n");
                            SetUsb2Speed(1); // 默认为全速
                            return (ERR_SUCCESS);
                        }
                        else if(if_cls == 2)
                        {
                            ThisUsb2Dev.DeviceType = DEV_TYPE_MOUSE;
                            //	为了以后查询鼠标状态,应该分析描述符,取得中断端口的地址,长度等信息
                            PRINT("U2 USB-Mouse Ready\n");
                            SetUsb2Speed(1); // 默认为全速
                            return (ERR_SUCCESS);
                        }
                        s = ERR_USB_UNSUPPORT;
                    }
                }
                else if(dv_cls == USB_DEV_CLASS_HUB)
                { // 是HUB类设备,集线器等
                    s = CtrlGetU2HubDescr();
                    if(s == ERR_SUCCESS)
                    {
                        PRINT("Max Port:%02X ", (((PXUSB_HUB_DESCR)U2Com_Buffer)->bNbrPorts));
                        ThisUsb2Dev.GpHUBPortNum = ((PXUSB_HUB_DESCR)U2Com_Buffer)->bNbrPorts; // 保存HUB的端口数量
                        if(ThisUsb2Dev.GpHUBPortNum > HUB_MAX_PORTS)
                        {
                            ThisUsb2Dev.GpHUBPortNum = HUB_MAX_PORTS; // 因为定义结构DevOnHubPort时人为假定每个HUB不超过HUB_MAX_PORTS个端口
                        }
                        s = CtrlSetUsb2Config(cfg); // 设置USB设备配置
                        if(s == ERR_SUCCESS)
                        {
                            ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
                            ThisUsb2Dev.DeviceType = USB_DEV_CLASS_HUB;
                            //需保存端点信息以便主程序进行USB传输,本来中断端点可用于HUB事件通知,但本程序使用查询状态控制传输代替
                            //给HUB各端口上电,查询各端口状态,初始化有设备连接的HUB端口,初始化设备
                            for(i = 1; i <= ThisUsb2Dev.GpHUBPortNum; i++) // 给HUB各端口都上电
                            {
                                DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // 清外部HUB端口上设备的状态
                                s = U2HubSetPortFeature(i, HUB_PORT_POWER);
                                if(s != ERR_SUCCESS)
                                {
                                    PRINT("Ext-HUB Port_%1d# power on error\n", (uint16_t)i); // 端口上电失败
                                }
                            }
                            PRINT("U2 USB-HUB Ready\n");
                            SetUsb2Speed(1); // 默认为全速
                            return (ERR_SUCCESS);
                        }
                    }
                }
                else
                {                               // 可以进一步分析
                    s = CtrlSetUsb2Config(cfg); // 设置USB设备配置
                    if(s == ERR_SUCCESS)
                    {
                        //	需保存端点信息以便主程序进行USB传输
                        ThisUsb2Dev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsb2Dev.DeviceType = DEV_TYPE_UNKNOW;
                        SetUsb2Speed(1);      // 默认为全速
                        return (ERR_SUCCESS); /* 未知设备初始化成功 */
                    }
                }
#endif
            }
        }
    }

    PRINT("InitRootU2Dev Err = %02X\n", (uint16_t)s);
#ifdef FOR_ROOT_UDISK_ONLY
    CHRV3DiskStatus = DISK_CONNECT;
#else
    ThisUsb2Dev.DeviceStatus = ROOT_DEV_FAILED;
#endif
    SetUsb2Speed(1); // 默认为全速
    return (s);
}

/*********************************************************************
 * @fn      InitU2DevOnHub
 *
 * @brief   初始化枚举外部HUB后的二级USB设备
 *
 * @param   HubPortIndex    - 指定外部HUB
 *
 * @return  错误码
 */
uint8_t InitU2DevOnHub(uint8_t HubPortIndex)
{
    uint8_t i, s, cfg, dv_cls, if_cls;
    uint8_t ifc;
    PRINT("Init dev @ExtHub-port_%1d ", (uint16_t)HubPortIndex);
    if(HubPortIndex == 0)
    {
        return (ERR_USB_UNKNOWN);
    }
    SelectU2HubPort(HubPortIndex); // 选择操作指定的ROOT-HUB端口的外部HUB的指定端口,选择速度
    PRINT("GetDevDescr: ");
    s = CtrlGetU2DeviceDescr(); // 获取设备描述符
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    DevOnU2HubPort[HubPortIndex - 1].DeviceVID = ((uint16_t)((PUSB_DEV_DESCR)U2Com_Buffer)->idVendor); //保存VID PID信息
    DevOnU2HubPort[HubPortIndex - 1].DevicePID = ((uint16_t)((PUSB_DEV_DESCR)U2Com_Buffer)->idProduct);

    dv_cls = ((PUSB_DEV_DESCR)U2Com_Buffer)->bDeviceClass; // 设备类代码
    cfg = (1 << 4) + HubPortIndex;                         // 计算出一个USB地址,避免地址重叠
    s = CtrlSetUsb2Address(cfg);                           // 设置USB设备地址
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    DevOnU2HubPort[HubPortIndex - 1].DeviceAddress = cfg; // 保存分配的USB地址
    PRINT("GetCfgDescr: ");
    s = CtrlGetU2ConfigDescr(); // 获取配置描述符
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    cfg = ((PUSB_CFG_DESCR)U2Com_Buffer)->bConfigurationValue;
    for(i = 0; i < ((PUSB_CFG_DESCR)U2Com_Buffer)->wTotalLength; i++)
    {
        PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
    }
    PRINT("\n");
    /* 分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等 */
    if_cls = ((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceClass; // 接口类代码
    if(dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE)                     // 是USB存储类设备,基本上确认是U盘
    {
        AnalyzeU2BulkEndp(U2Com_Buffer, HubPortIndex);
        for(i = 0; i != 4; i++)
        {
            PRINT("%02x ", (uint16_t)DevOnU2HubPort[HubPortIndex - 1].GpVar[i]);
        }
        PRINT("\n");
        s = CtrlSetUsb2Config(cfg); // 设置USB设备配置
        if(s == ERR_SUCCESS)
        {
            DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            DevOnU2HubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_STORAGE;
            PRINT("USB-Disk Ready\n");
            SetUsb2Speed(1); // 默认为全速
            return (ERR_SUCCESS);
        }
    }
    else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) && (((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceSubClass <= 0x01)) // 是HID类设备,键盘/鼠标等
    {
        ifc = ((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)->cfg_descr.bNumInterfaces;
        s = AnalyzeU2HidIntEndp(U2Com_Buffer, HubPortIndex); // 从描述符中分析出HID中断端点的地址
        PRINT("AnalyzeU2HidIntEndp %02x\n", (uint16_t)s);
        if_cls = ((PXUSB_CFG_DESCR_LONG)U2Com_Buffer)->itf_descr.bInterfaceProtocol;
        s = CtrlSetUsb2Config(cfg); // 设置USB设备配置
        if(s == ERR_SUCCESS)
        {
            for(dv_cls = 0; dv_cls < ifc; dv_cls++)
            {
                s = CtrlGetU2HIDDeviceReport(dv_cls); //获取报表描述符
                if(s == ERR_SUCCESS)
                {
                    for(i = 0; i < 64; i++)
                    {
                        PRINT("x%02X ", (uint16_t)(U2Com_Buffer[i]));
                    }
                    PRINT("\n");
                }
            }
            //需保存端点信息以便主程序进行USB传输
            DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            if(if_cls == 1)
            {
                DevOnU2HubPort[HubPortIndex - 1].DeviceType = DEV_TYPE_KEYBOARD;
                //进一步初始化,例如设备键盘指示灯LED等
                if(ifc > 1)
                {
                    PRINT("USB_DEV_CLASS_HID Ready\n");
                    //                    DevOnU2HubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HID; //复合HID设备
                }
                PRINT("USB-Keyboard Ready\n");
                SetUsb2Speed(1); // 默认为全速

                return (ERR_SUCCESS);
            }
            else if(if_cls == 2)
            {
                DevOnU2HubPort[HubPortIndex - 1].DeviceType = DEV_TYPE_MOUSE;
                //为了以后查询鼠标状态,应该分析描述符,取得中断端口的地址,长度等信息
                if(ifc > 1)
                {
                    PRINT("USB_DEV_CLASS_HID Ready\n");
                    //                    DevOnU2HubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HID; //复合HID设备
                }
                PRINT("USB-Mouse Ready\n");
                SetUsb2Speed(1); // 默认为全速

                return (ERR_SUCCESS);
            }
            s = ERR_USB_UNSUPPORT;
        }
    }
    else if(dv_cls == USB_DEV_CLASS_HUB) // 是HUB类设备,集线器等
    {
        DevOnU2HubPort[HubPortIndex - 1].DeviceType = USB_DEV_CLASS_HUB;
        PRINT("This program don't support Level 2 HUB\n"); // 需要支持多级HUB级联请参考本程序进行扩展
        s = U2HubClearPortFeature(i, HUB_PORT_ENABLE);     // 禁止HUB端口
        if(s != ERR_SUCCESS)
        {
            return (s);
        }
        s = ERR_USB_UNSUPPORT;
    }
    else //其他设备
    {
        AnalyzeU2BulkEndp(U2Com_Buffer, HubPortIndex); //分析出批量端点
        for(i = 0; i != 4; i++)
        {
            PRINT("%02x ", (uint16_t)DevOnU2HubPort[HubPortIndex - 1].GpVar[i]);
        }
        PRINT("\n");
        s = CtrlSetUsb2Config(cfg); // 设置USB设备配置
        if(s == ERR_SUCCESS)
        {
            //需保存端点信息以便主程序进行USB传输
            DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_SUCCESS;
            DevOnU2HubPort[HubPortIndex - 1].DeviceType = dv_cls ? dv_cls : if_cls;
            SetUsb2Speed(1);      // 默认为全速
            return (ERR_SUCCESS); //未知设备初始化成功
        }
    }
    PRINT("InitDevOnHub Err = %02X\n", (uint16_t)s);
    DevOnU2HubPort[HubPortIndex - 1].DeviceStatus = ROOT_DEV_FAILED;
    SetUsb2Speed(1); // 默认为全速
    return (s);
}

/*********************************************************************
 * @fn      EnumU2HubPort
 *
 * @brief   枚举指定ROOT-HUB端口上的外部HUB集线器的各个端口,检查各端口有无连接或移除事件并初始化二级USB设备
 *
 * @param   RootHubIndex    - ROOT_HUB0和ROOT_HUB1
 *
 * @return  错误码
 */
uint8_t EnumU2HubPort()
{
    uint8_t i, s;

    for(i = 1; i <= ThisUsb2Dev.GpHUBPortNum; i++) // 查询集线器的端口是否有变化
    {
        SelectU2HubPort(0);        // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
        s = U2HubGetPortStatus(i); // 获取端口状态
        if(s != ERR_SUCCESS)
        {
            return (s); // 可能是该HUB断开了
        }
        if(((U2Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) && (U2Com_Buffer[2] & (1 << (HUB_C_PORT_CONNECTION & 0x07)))) || (U2Com_Buffer[2] == 0x10))
        {                                                            // 发现有设备连接
            DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_CONNECTED; // 有设备连接
            DevOnU2HubPort[i - 1].DeviceAddress = 0x00;
            s = U2HubGetPortStatus(i); // 获取端口状态
            if(s != ERR_SUCCESS)
            {
                return (s); // 可能是该HUB断开了
            }
            DevOnU2HubPort[i - 1].DeviceSpeed = U2Com_Buffer[1] & (1 << (HUB_PORT_LOW_SPEED & 0x07)) ? 0 : 1; // 低速还是全速
            if(DevOnU2HubPort[i - 1].DeviceSpeed)
            {
                PRINT("Found full speed device on port %1d\n", (uint16_t)i);
            }
            else
            {
                PRINT("Found low speed device on port %1d\n", (uint16_t)i);
            }
            mDelaymS(200);                              // 等待设备上电稳定
            s = U2HubSetPortFeature(i, HUB_PORT_RESET); // 对有设备连接的端口复位
            if(s != ERR_SUCCESS)
            {
                return (s); // 可能是该HUB断开了
            }
            PRINT("Reset port and then wait in\n");
            do // 查询复位端口,直到复位完成,把完成后的状态显示出来
            {
                mDelaymS(1);
                s = U2HubGetPortStatus(i);
                if(s != ERR_SUCCESS)
                {
                    return (s); // 可能是该HUB断开了
                }
            } while(U2Com_Buffer[0] & (1 << (HUB_PORT_RESET & 0x07))); // 端口正在复位则等待
            mDelaymS(100);
            s = U2HubClearPortFeature(i, HUB_C_PORT_RESET);      // 清除复位完成标志
                                                                 //             s = U2HubSetPortFeature( i, HUB_PORT_ENABLE );                              // 启用HUB端口
            s = U2HubClearPortFeature(i, HUB_C_PORT_CONNECTION); // 清除连接或移除变化标志
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            s = U2HubGetPortStatus(i); // 再读取状态,复查设备是否还在
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            if((U2Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) == 0)
            {
                DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // 设备不在了
            }
            s = InitU2DevOnHub(i); // 初始化二级USB设备
            if(s != ERR_SUCCESS)
            {
                return (s);
            }
            SetUsb2Speed(1); // 默认为全速
        }
        else if(U2Com_Buffer[2] & (1 << (HUB_C_PORT_ENABLE & 0x07))) // 设备连接出错
        {
            U2HubClearPortFeature(i, HUB_C_PORT_ENABLE); // 清除连接错误标志
            PRINT("Device on port error\n");
            s = U2HubSetPortFeature(i, HUB_PORT_RESET); // 对有设备连接的端口复位
            if(s != ERR_SUCCESS)
                return (s); // 可能是该HUB断开了
            do              // 查询复位端口,直到复位完成,把完成后的状态显示出来
            {
                mDelaymS(1);
                s = U2HubGetPortStatus(i);
                if(s != ERR_SUCCESS)
                    return (s);                                        // 可能是该HUB断开了
            } while(U2Com_Buffer[0] & (1 << (HUB_PORT_RESET & 0x07))); // 端口正在复位则等待
        }
        else if((U2Com_Buffer[0] & (1 << (HUB_PORT_CONNECTION & 0x07))) == 0) // 设备已经断开
        {
            if(DevOnU2HubPort[i - 1].DeviceStatus >= ROOT_DEV_CONNECTED)
            {
                PRINT("Device on port %1d removed\n", (uint16_t)i);
            }
            DevOnU2HubPort[i - 1].DeviceStatus = ROOT_DEV_DISCONNECT; // 有设备连接
            if(U2Com_Buffer[2] & (1 << (HUB_C_PORT_CONNECTION & 0x07)))
            {
                U2HubClearPortFeature(i, HUB_C_PORT_CONNECTION); // 清除移除变化标志
            }
        }
    }
    return (ERR_SUCCESS); // 返回操作成功
}

/*********************************************************************
 * @fn      EnumAllU2HubPort
 *
 * @brief   枚举所有ROOT-HUB端口下外部HUB后的二级USB设备
 *
 * @return  错误码
 */
uint8_t EnumAllU2HubPort(void)
{
    uint8_t s;

    if((ThisUsb2Dev.DeviceStatus >= ROOT_DEV_SUCCESS) && (ThisUsb2Dev.DeviceType == USB_DEV_CLASS_HUB)) // HUB枚举成功
    {
        SelectU2HubPort(0);  // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
        s = EnumU2HubPort(); // 枚举指定ROOT-HUB端口上的外部HUB集线器的各个端口,检查各端口有无连接或移除事件
        if(s != ERR_SUCCESS) // 可能是HUB断开了
        {
            PRINT("EnumAllHubPort err = %02X\n", (uint16_t)s);
        }
        SetUsb2Speed(1); // 默认为全速
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      U2SearchTypeDevice
 *
 * @brief   在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号,输出端口号为0xFFFF则未搜索到.
 *          当然也可以根据USB的厂商VID产品PID进行搜索(事先要记录各设备的VID和PID),以及指定搜索序号
 *
 * @param   type    - 搜索的设备类型
 *
 * @return  输出高8位为ROOT-HUB端口号,低8位为外部HUB的端口号,低8位为0则设备直接在ROOT-HUB端口上
 */
uint16_t U2SearchTypeDevice(uint8_t type)
{
    uint8_t RootHubIndex; //CH554只有一个USB口,RootHubIndex = 0,只需看返回值的低八位即可
    uint8_t HubPortIndex;

    RootHubIndex = 0;
    if((ThisUsb2Dev.DeviceType == USB_DEV_CLASS_HUB) && (ThisUsb2Dev.DeviceStatus >= ROOT_DEV_SUCCESS)) // 外部集线器HUB且枚举成功
    {
        for(HubPortIndex = 1; HubPortIndex <= ThisUsb2Dev.GpHUBPortNum; HubPortIndex++) // 搜索外部HUB的各个端口
        {
            if(DevOnU2HubPort[HubPortIndex - 1].DeviceType == type && DevOnU2HubPort[HubPortIndex - 1].DeviceStatus >= ROOT_DEV_SUCCESS)
            {
                return (((uint16_t)RootHubIndex << 8) | HubPortIndex); // 类型匹配且枚举成功
            }
        }
    }
    if((ThisUsb2Dev.DeviceType == type) && (ThisUsb2Dev.DeviceStatus >= ROOT_DEV_SUCCESS))
    {
        return ((uint16_t)RootHubIndex << 8); // 类型匹配且枚举成功,在ROOT-HUB端口上
    }

    return (0xFFFF);
}

/*********************************************************************
 * @fn      U2SETorOFFNumLock
 *
 * @brief   NumLock的点灯判断
 *
 * @param   buf     - 点灯键值
 *
 * @return  错误码
 */
uint8_t U2SETorOFFNumLock(uint8_t *buf)
{
    uint8_t tmp[] = {0x21, 0x09, 0x00, 0x02, 0x00, 0x00, 0x01, 0x00};
    uint8_t len, s;
    if((buf[2] == 0x53) & (buf[0] | buf[1] | buf[3] | buf[4] | buf[5] | buf[6] | buf[7] == 0))
    {
        for(s = 0; s != sizeof(tmp); s++)
        {
            ((uint8_t *)pU2SetupReq)[s] = tmp[s];
        }
        s = U2HostCtrlTransfer(U2Com_Buffer, &len); // 执行控制传输
        if(s != ERR_SUCCESS)
        {
            return (s);
        }
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlGetU2HIDDeviceReport
 *
 * @brief   获取HID设备报表描述符,返回在TxBuffer中
 *
 * @param   none
 *
 * @return  错误码
 */
uint8_t CtrlGetU2HIDDeviceReport(uint8_t infc)
{
    uint8_t s;
    uint8_t len;

    CopyU2SetupReqPkg((uint8_t *)SetupSetU2HIDIdle);
    pU2SetupReq->wIndex = infc;
    s = U2HostCtrlTransfer(U2Com_Buffer, &len); // 执行控制传输
    if(s != ERR_SUCCESS)
    {
        return (s);
    }

    CopyU2SetupReqPkg((uint8_t *)SetupGetU2HIDDevReport);
    pU2SetupReq->wIndex = infc;
    s = U2HostCtrlTransfer(U2Com_Buffer, &len); // 执行控制传输
    if(s != ERR_SUCCESS)
    {
        return (s);
    }

    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      CtrlGetU2HubDescr
 *
 * @brief   获取HUB描述符,返回在Com_Buffer中
 *
 * @param   none
 *
 * @return  错误码
 */
uint8_t CtrlGetU2HubDescr(void)
{
    uint8_t s;
    uint8_t len;

    CopyU2SetupReqPkg((uint8_t *)SetupGetU2HubDescr);
    s = U2HostCtrlTransfer(U2Com_Buffer, &len); // 执行控制传输
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    if(len < ((PUSB_SETUP_REQ)SetupGetU2HubDescr)->wLength)
    {
        return (ERR_USB_BUF_OVER); // 描述符长度错误
    }
    //  if ( len < 4 ) return( ERR_USB_BUF_OVER );                                 // 描述符长度错误
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      U2HubGetPortStatus
 *
 * @brief   查询HUB端口状态,返回在Com_Buffer中
 *
 * @param   HubPortIndex    - 端口号
 *
 * @return  错误码
 */
uint8_t U2HubGetPortStatus(uint8_t HubPortIndex)
{
    uint8_t s;
    uint8_t len;

    pU2SetupReq->bRequestType = HUB_GET_PORT_STATUS;
    pU2SetupReq->bRequest = HUB_GET_STATUS;
    pU2SetupReq->wValue = 0x0000;
    pU2SetupReq->wIndex = 0x0000 | HubPortIndex;
    pU2SetupReq->wLength = 0x0004;
    s = U2HostCtrlTransfer(U2Com_Buffer, &len); // 执行控制传输
    if(s != ERR_SUCCESS)
    {
        return (s);
    }
    if(len < 4)
    {
        return (ERR_USB_BUF_OVER); // 描述符长度错误
    }
    return (ERR_SUCCESS);
}

/*********************************************************************
 * @fn      U2HubSetPortFeature
 *
 * @brief   设置HUB端口特性
 *
 * @param   HubPortIndex    - 端口号
 * @param   FeatureSelt     - 端口特性
 *
 * @return  错误码
 */
uint8_t U2HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt)
{
    pU2SetupReq->bRequestType = HUB_SET_PORT_FEATURE;
    pU2SetupReq->bRequest = HUB_SET_FEATURE;
    pU2SetupReq->wValue = 0x0000 | FeatureSelt;
    pU2SetupReq->wIndex = 0x0000 | HubPortIndex;
    pU2SetupReq->wLength = 0x0000;
    return (U2HostCtrlTransfer(NULL, NULL)); // 执行控制传输
}

/*********************************************************************
 * @fn      U2HubClearPortFeature
 *
 * @brief   清除HUB端口特性
 *
 * @param   HubPortIndex    - 端口号
 * @param   FeatureSelt     - 端口特性
 *
 * @return  错误码
 */
uint8_t U2HubClearPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt)
{
    pU2SetupReq->bRequestType = HUB_CLEAR_PORT_FEATURE;
    pU2SetupReq->bRequest = HUB_CLEAR_FEATURE;
    pU2SetupReq->wValue = 0x0000 | FeatureSelt;
    pU2SetupReq->wIndex = 0x0000 | HubPortIndex;
    pU2SetupReq->wLength = 0x0000;
    return (U2HostCtrlTransfer(NULL, NULL)); // 执行控制传输
}
