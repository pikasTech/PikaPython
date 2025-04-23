# MicroLink

---

## 一、产品概述

MicroLink是一款集多功能于一体的嵌入式系统开发工具，专为加速和简化开发者在**研发、调试、量产和售后服务**各阶段的工作流程而设计。不同于传统的开发工具链，MicroLink在DAPLink的基础上将**调试器**、**USB转串口**、**离线下载器**和**固件升级工具**等多种功能集成到一个设备中，为开发者提供一站式解决方案。无论您是下载固件、调试代码、批量生产还是固件升级，MicroLink都能满足您的需求，大大提升开发效率，减少工具切换带来的时间和财务成本。

### 产品特点

- [x] 支持SWD/JTAG接口，下载调试速度超越JLINK V12（时钟10Mhz）
- [x] 支持使用OpenOCD的IDE下载调试ARM/RISC-V等芯片
- [x] 支持USB转串口，最大10M波特率无丢包
- [x] 内置RTT，无需使用RTTView上位机，支持任意串口助手
- [x] 支持python脚本，可以通过脚本指定下载算法
- [x] 支持Cortex-M系列U盘拖拽下载
- [x] 支持U盘离线下载，通过python脚本触发下载
- [x] 内置ymodem协议栈，通过python脚本触发
- [x] 支持系统固件升级，为后续添加更多功能
- [x] 采用winusb对window10免驱，即插即用
- [x] 支持3V3/5V大电流输出电源
- [x] 内置防倒灌和过流保护，外部电流无法反向流入USB口，防止损坏USB
  ![MicroLink](images/microlink/MicroLink.jpg)



结合以上产品特点，为开发者提供了下载调试，批量生产，售后维护，固件升级等一站式解决方案。

### 购买地址

淘宝链接：https://item.taobao.com/item.htm?ft=t&id=895964393739

![SJTB](images/microlink/SJTB.png)

### 更多技术支持

手机扫码添加微信备注**加群**

![](images/microlink/weixin.jpg)

关注公众号及时获得最新文章

![](images/microlink/WX.jpg)

## 二、功能介绍

### 1、DAPLink 在线下载和调试

MicroLink基于标准的CMSIS-DAP在线调试下载协议，针对传统DAPLink工具下载和调试速度缓慢的问题进行了全面优化。为了弥补DAPLINK性能上的不足，硬件方面，采用了**先辑半导体**的高性能**HPM5301**芯片，该芯片主频高达480MHz，内置PHY的高速USB接口；软件方面，将USB协议替换为传输速度更快的**CherryUSB**协议栈，并且对DAPLink固件中的数据处理和通信代码进行了深度优化，减少了内部延迟和等待时间，将SWD时钟速度提升至10MHz。

高速SWD支持高达10MHz的稳定时钟频率，为资源有限的嵌入式设备提供快速、可靠的单线调试和下载体验。

![](images/microlink/10M.png)

下载过程中CLK的时钟波形：

![](images/microlink/clk.jpg)

- **下载速度对比测试**

与目前市面上最新的J-LINK-V12速度对比，目标芯片使用STM32H743，开发环境MDK V5.39，分别使用**MicroLink**和**Jlink V12**将**2558KB**的HEX文件下载到内部FLASH中。使用逻辑分析仪测试时钟引脚，计算出擦除，编程，校验全过程的时间，MicroLink使用时间为**24.205秒**，Jlink V12使用时间为**33.439秒**，测试数据如下图：

**Jlink V12测试结果：**

![](images/microlink/JLINK_Download.jpg)

**MicroLink测试结果：**

![](images/microlink/MicroLink_Download.jpg)

**测试结果对比：**

| 调试器        | 总耗时（擦除，编程，校验） |
| ------------- | :------------------------: |
| **MicroLink** |        **24.205秒**        |
| J-LINK V12    |          33.439秒          |

### 2、USB转串口

MicroLink内置USB转串口功能，支持常见的串口和485通信，串口最大支持10M波特率，无丢包。

![](images/microlink/10M_Baud.jpg)

使用逻辑分析仪抓取波形如图所示，每个bit传输的时间为1/10M=100ns。

![](images/microlink/10M_TTL.jpg)



### 3、RTTView

只要拥有了**MicroLink**，你就可以享受以下的便利：

- 无需占用**USART**或者**USB**转串口工具，将**printf**重定位到一个由**MicroLink**提供的虚拟串口上；
- 不需要使用专门的RTTView上位机，支持任意串口助手；
- 高速通信，不影响芯片的实时响应。

**启动RTT功能：**打开任意串口助手，输入以下指令：

```python
RTTView.start(0x20000000,1024)
```

- 0x20000000:搜索RTT控制块的起始地址；
- 1024：搜寻范围大小

![](images/microlink/RTT.jpg)

### 4、U盘拖拽下载

MicroLink支持U盘拖拽下载功能，使固件更新变得像复制文件一样简单。用户只需将固件文件拖放到虚拟U盘中，MicroLink便能自动完成下载，摆脱对上位机的依赖，极大地降低了操作门槛。

U盘拖拽下载支持HEX文件和BIN文件，HEX文件自带地址信息，自动根据HEX中的地址选择烧录的位置，BIN文件的下载地址可以通过`flm_config.py`脚本进行配置。

![](images/microlink/flmo.jpg)

打开U盘内的`flm_config.py`脚本，代码如下所示：

```py
import FLMConfig
ReadFlm = FLMConfig.ReadFlm()
res1 = ReadFlm.load("STM32/STM32F10x_512.FLM.o",0X08000000,0x20000000)
```

`ReadFlm.load`函数的三个参数：

- "STM32/STM32F10x_512.FLM.o" ：选择对应单片机的下载算法文件；

![](images/microlink/STM32FLMO.jpg)

- 0X08000000：默认U盘拖拽下载的FLASH位置；
- 0x20000000：单片机的RAM基地址；

以下演示视频是将HEX文件复制到U盘中，完成固件下载：

<iframe src="https://player.bilibili.com/player.html?bvid=BV14HsKeJEQ1" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true" width="640" height="480"> </iframe>

### 5、离线下载

MicroLink支持脱机离线下载的功能，借助于强大的PikaPython开源项目，让MicroLink可以使用python脚本进行二次开发，可以非常容易得定制私有功能和上位机的开发。

> PikaPython (也称 PikaScript、PikaPy) 是一个完全重写的超轻量级 python 引擎，零依赖，零配置，可以在少于 4KB 的 RAM 下运行 (如 stm32g030c8 和 stm32f103c8)，极易部署和扩展。

MicroLink内置了一条离线下载的python函数：

```python
load.bin("boot.bin",0X8000000)
```

两个参数的含义：

- "boot.bin"：下载的文件名字；
- 0X8000000：下载的地址；

将需要下载的bin文件复制到U盘中，然后随便使用一个串口助手，打开虚拟串口，输入`load.bin("boot.bin",0X8000000)`加回车，效果如下：

![](images/microlink/loadbin.png)



### 6、内置Ymodem协议下载

MicroLink内置Ymodem协议，支持通过串口进行可靠的文件传输。Ymodem协议在多次重传时仍能保持数据的完整性，非常适用于嵌入式系统的固件升级。

使用内置的ymodem协议发送文件，首先需要目标设备支持ymodem协议接收文件，MicorBoot开源框架集成了ymodem模块，可以方便用户直接安装使用，具体使用方法请看MicorBoot简介。

MicorBoot简介：https://microboot.readthedocs.io/zh-cn/latest/

MicorBoot开源代码：https://github.com/Aladdin-Wang/MicroBoot

将需要升级的固件拷贝到U盘中，比如updata.bin，然后随便使用一个串口助手，打开虚拟串口，输入`ym.send("updata.bin")`加回车

参数的含义：

- "updata.bin"：下载的文件名字；
- 支持多个参数，多个文件连续发送；

如果你的bootloader中还没有ymodem协议，可以使用xshell等待ymodem接收协议的上位机，进行快速验证，演示视频如下，使用MicroLink和另外一个串口工具，分别使用sscom5上位机输入命令和xshell上位机进行文件接收

<iframe src="https://player.bilibili.com/player.html?bvid=BV1VYRgYYERg" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true" width="640" height="480"> </iframe>

### 7、FLM通用下载算法转换工具

以STM32F4xx_1024.FLM为例，下载算法在ARM CMSIS Pack文件夹（通常在D:\Users\Administrator\AppData\Local\Arm\Packs\Keil\STM32F4xx_DFP\2.15.0\CMSIS\Flash）中，通过FLM下载算法转换工具打开文件，可以生成对应的下载算法驱动文件，将生成的STM32F4xx_512.FLM.o文件，拷贝到MicroLink的U盘中，然后通过`flm_config.py`脚本指定使用此下载算法文件。

![](images/microlink/FLMTool.png)

借助单片机原厂提供的FLM下载算法文件，便可以几乎适配所有的Cortex-M系列单片机。

### 8、固件升级

MicroLink支持系统固件升级，可以为后续添加更多的功能，升级方式非常简单，只需要将microlink.rbl升级包，复制到MicroLink的U盘中即可自动完成升级，升级完成后会自动重启设备，并删除升级包。升级完成可以查看DETAILS.TXT文件，了解升级后的新功能。



**开发资料下载地址**：https://pan.baidu.com/s/1Dr8Ss16cBRWXtQpyOGrROg?pwd=zyo0 



![](images/microlink/microlink.pack.png)

## 三、使用说明

### 1、U盘文件说明

- DETAILS.TXT

DETAILS.TXT记录了MicroLink软硬件版本和每次版本更新的内容。

![](E:/software/MicroBoot/docs/images/microlink/DETAILS.png)

- MBED.HTM

MBED.HTM是一个在线文档的网址链接，双击该文件即可访问在线文档，进一步了解更多的功能。

![](images/microlink/readdocs.png)

- flm_config.py

设备上电会首先读取flm_config.py脚本，根据脚本内容加载Flash下载算法，执行用户指令。

- xxx.FLM.o

借助单片机厂家提供的pack包里的xxx.FLM下载算法文件，提取出擦写函数，生成xxx.FLM.o文件。

比如STM32F1系列的下载算法所在的电脑默认位置如下：

![](images/microlink/FLM.jpg)

### 2、引脚说明

| 引脚名称 | 功能                                |
| -------- | ----------------------------------- |
| 5V       | 5V电源输出                          |
| 3.3V     | 3.3V电源输出                        |
| GND      | GND公共地                           |
| DIO/TMS  | SWD接口数据信号，或JTAG接口模式选择 |
| CLK/TCK  | SWD接口时钟信号，或JTAG接口时钟     |
| TDO      | JTAG接口数据输出                    |
| TDI      | JTAG接口数据输入                    |
| RX/A     | 串口数据接收，或者485接口A          |
| TX/B     | 串口数据发生，或者485接口B          |
| RST      | 复位脚输出                          |

SWD简化接线图：

![](images/microlink/SWD.jpg)

JTAG简化接线图：

![](images/microlink/JTAG.jpg)

### 3、端口说明

电脑使用USB TypeC数据线与MicroLink连接以后，电脑设备端会弹出三个设备：

![](images/microlink/shebei.png)

- 磁盘设备

![](images/microlink/Upan.png)

- 串口(两路)

**一路为USB转串口**；

另外一路为**虚拟串口**，打开串口时，输入回车，会自动打印python的信息；

**为了方便区分两路串口，通过USB转串口发送数据LED指示灯会闪烁，通过虚拟串口发送数据不LED指示灯不会闪烁。**

![](images/microlink/python.jpg)

- MicroLink CMSIS-DAP

**如果Keil无法识别，请给keil升级到最新版本（不能低于5.29），并尝试卸载MicroLink CMSIS-DAP设备驱动，重新连接MicroLink**

### 4、操作说明

- 以Keil为例

1、在DEBUG栏中选择CMSIS-DAP Debugger

![](images/microlink/DEBUG.png)

2、与设备连接好SWD引脚，选择MICROLINK CMSIS-DAP，Max Clock下载时钟频率选择10MHz

![](microlink/MAX_Clock.jpg)

3、勾选自动复位选项，添加下载算法

![](E:/software/MicroBoot/docs/images/microlink/XZSF.jpg)

- 以SEGGER Embedded Studio为例

1、点击工程， 右击选择“options” ， 在弹出的对话框中点击Debugger,然后选择GDB Server  

![](images/microlink/SES1.jpg)

2、点击GDB Server,在GDB Server Command Line中查看openocd配置文件,更改此配置文件为 cmsis-dap.cfg

![](images/microlink/SES2.jpg)

3、与设备连接好JTAG引脚，点击Target,连接connect GDB Server，连接成功后Output窗口如图所示

![](images/microlink/SES3.jpg)