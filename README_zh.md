
<img align="left" src="https://emojis.slackmojis.com/emojis/images/1563480763/5999/meow_party.gif" width="60" height="60"/>

[![CI](https://github.com/pikasTech/pikascript/actions/workflows/CI.yml/badge.svg)](https://github.com/pikasTech/pikascript/actions/workflows/CI.yml)

# 来颗Star求求了~ Star please~
## issue, pr请往主仓库：[Github/pikastech/pikascript](https://github.com/pikastech/pikascript)

# 1.简介

PikaScript是一个完全重写的超轻量级python引擎，零依赖，零配置，可以在少于4KB的RAM下运行(如stm32g030c8和stm32f103c8)，极易部署和扩展。

![output_Kgj52R](https://user-images.githubusercontent.com/88232613/132940452-d07d766c-5aa7-4187-96f2-66b02984e82c.gif)


<img src="https://user-images.githubusercontent.com/88232613/137866679-642e4e74-c373-4880-9d97-20dfefeafc18.png" width="500"/>

![image](https://user-images.githubusercontent.com/88232613/138021445-13643518-b0a3-4872-a290-5dec27697837.png)

# 获取PikaScript:

## 使用Pika包管理器[PikaPackage.exe](../../raw/master/tools/pikaPackageManager/pikaPackage.exe)

1. 直接运行[PikaPackage.exe](../../raw/master/tools/pikaPackageManager/pikaPackage.exe), 自动下载并更新pikaScript主仓库(文件在当前磁盘的/tmp/pikaScript文件夹)

2. 将[requestment.txt](/bsp/stm32g030c8/pikascript/requestment.txt)放在[PikaPackage.exe](../../raw/master/tools/pikaPackageManager/pikaPackage.exe)同一文件夹下, 运行[pikaPackage.exe](../../raw/master/tools/pikaPackageManager/pikaPackage.exe), 自动在当前目录下安装[内核](../../tree/master/src)、[预编译器](../../tree/master/tools/pikaCompiler)与[模块](../../tree/master/package)。

3. 已发布的模块列表：[packages.toml](/packages.toml)

# 开发板

官方支持的开发板Pika派——Zero现已发布，基于STM32G030C8T6小资源MCU，仅64kB Flash，8kB RAM即可运行完整解释器，和完整的外设驱动（GPIO、TIME、IIC、RGB、KEY、LCD）。

![image](https://user-images.githubusercontent.com/88232613/141250135-17059a4e-3ebc-4c19-ba32-66ca358e99a0.png)

开发板配套1.8寸 128x160分辨率SPI彩屏：

![image](https://user-images.githubusercontent.com/88232613/141250437-10401f7d-3609-40f6-8911-7de38b976069.png)


# 视频教程

## 入门篇 - 面向单片机Python编程用户

[PikaScript入门-单片机python编程和环境配置](https://www.bilibili.com/video/BV1mg411L72e)

[Pika派开发板手把手单片机python编程01——模块安装和GPIO类](https://www.bilibili.com/video/BV1kg411K7W2)

## 中级篇 - 面向驱动开发者

[PikaScript中级 RISC-V部署PikaScript CH32V103R8](https://www.bilibili.com/video/BV1Cq4y1G7Tj)

[PikaScript中级 国产MCU CM32M101A部署Python引擎PikaScript](https://www.bilibili.com/video/BV1jv411u7EJ)

[PikaScript中级 驱动模块开发01 最简单的模块](https://www.bilibili.com/video/BV1aP4y1L7pi)

[PikaScript中级 驱动模块开发02 CH32V103 LED](https://www.bilibili.com/video/BV1Jr4y117Z8)

## 进阶篇 - 面向编译器爱好者和开源项目维护者

[PikaScript进阶-字节码与虚拟机](https://www.bilibili.com/video/BV1J3411y7tX/)

[PikaScript进阶-Github自动测试-Github Actions CI](https://www.bilibili.com/video/BV1o3411C76q/)

[PikaScript进阶-Googletest测试框架](https://www.bilibili.com/video/BV1UF411Y7Z6/)

[PikaScript进阶-数据结构源码精讲](https://www.bilibili.com/video/BV1Ky4y157nW/)

# 开发手册

[1. PikaScript-三分钟快速上手](document/1.三分钟快速上手.md)

[2. PikaScript-架构与原理](https://mp.weixin.qq.com/s?__biz=MzU4NzUzMDc1OA==&mid=2247484127&idx=1&sn=f66cff49c488e48c52570c7bb570328f&chksm=fdebd5b6ca9c5ca0707fd221c32f3ad63e94aeb6f917a92774b89ea042381ea261990f5cca3c&token=2045971639&lang=zh_CN#rd)

[3. PikaScript-十分钟快速部署](document/2.十分钟快速部署.md)

[4. PikaScript-标准开发流程](document/3.PikaScript标准开发流程.md)

[5. PikaScript-Demo展示](document/5.我就要用最便宜的单片机来跑python，还要用Keil开发，怎么地吧.md)

[6. Arm-2D GUI引擎和pikascript联合仿真](document/6.用Python玩ARM%202D，认真的嘛.md)

## 文件目录
[src](../../tree/master/src) - 内核源码

[bsp](../../tree/master/bsp) - 裸机芯片/板卡支持

[port](../../tree/master/port) - 操作系统支持

[document](../../tree/master/document) - 开发文档

[examples](../../tree/master/examples) - 示例脚本

[package](../../tree/master/package) - 模块目录

[pikaCompiler](../../tree/master/tools/pikaCompiler) - 使用rust编写的预编译器

[pikaPackageManager](../../tree/master/tools/pikaPackageManager) - 使用go编写的模块管理器

# 2.驱动适配进度
|MCU/Board|bsp|gpio|uart|pwm|adc|i2c|spi|rgb|oled|
|---|---|---|---|---|---|---|---|---|---|
|[PikaPi Zero](https://item.taobao.com/item.htm?spm=a230r.1.14.1.4f2e27a8R0qWJn&id=654947372034&ns=1&abbucket=15#detail)|√|√|√|√|√|√| |√|√|
|stm32g030c8|√|√|√|√|√|√| | | |
|stm32g070cB|√|√|√|√|√| | | | |
|stm32f103c8|√|√|√|√|√| | | | |
|ch32v103r8t6|√|√| | | | | | | |
|cm32m101a|√| | | | | | | | |
# 3.特性
### (1)运行环境

支持裸机运行，可运行于 **RAM ≥ 4kB** ，**FLASH ≥ 32kB** 的mcu中，如stm32g030, stm32f103c8t6，esp8266。

### (2)开发环境
支持串口下载Python脚本。

<img src="https://user-images.githubusercontent.com/88232613/134841230-85de6734-8467-4245-93a5-d452b5022b42.gif" width="400" alt="微信交流群"/><br/>

支持Keil、IAR、rt-thread studio、segger embedded studio等IDE开发。

支持CMake、makeFile、Scons等构建工具

零依赖，零配置，开箱即用，极易集成进已有的C工程。

极易拓展自定义的C原生函数。

支持跨平台，可在linux环境开发内核。

### (3)语法特性

使用python3标准语法的子集。

在编译时支持python类和方法定义，完整支持封装、继承、多态、模块功能 - 基于[Pika预编译器](../../tree/master/tools/pikaCompiler)。

在运行时支持python方法调用、变量定义、对象构造、对象释放、控制流(if\while) - 基于[Pika运行时内核](../../tree/master/src/package/pikascript/pikascript-core)。

|语法|编译时|运行时|
|---|---|---|
|类定义|√| |
|类继承|√| |
|方法定义|√|√|
|方法重载|√|√|
|方法调用|√|√|
|模块导入|√|√|
|参数定义|√|√|
|参数赋值|√|√|
|对象新建|√|√|
|对象销毁|√|√|
|对象嵌套|√|√|

#### 操作符

| + | - | * | / | == | > | < | >= | <= |
|---|---|---|---|---|---|---|---|---|
|√|√|√|√|√|√|√| | |

#### 控制流

|if|while|for|
|--- |--- |--- |
|√|√| |

### (4)源码规范

注重源码可读性，命名规范，标准统一，完全不使用宏，几乎不使用全局变量。

完整的googletest单元测试。

# 4.交流与技术支持：

## 微信交流群

![微信截图_20210917133247](https://user-images.githubusercontent.com/88232613/133729804-e1d0406b-a859-4ebc-b335-477e9c03810e.png)

### 群已经满啦，加我QQ拉你进群~ qq: 645275593

## Tencent QQ： 645275593

## Tencent QQ Group:

<img src="https://user-images.githubusercontent.com/88232613/136304186-e49610e4-ce01-4524-99b6-b56842d56411.png" width="300" alt="微信交流群"/><br/>

## E-mail: 645275593@qq.com

# 5.贡献者
| 内容 | 贡献者 |
| --- | --- |
| 基于QEMU的ARM-2D仿真工程 | [liuduanfei](https://github.com/liuduanfei) |
| 捐赠：GD32E103TB芯片2片 | 信息牛(微信名) |
| Rt-thread 支持包模板| [Meco Jianting Man](https://github.com/mysterywolf) |
| 捐赠：移远EC600S-CN 4G模块 | 移远模块 |
| 捐赠：博流BL706 开发板|博流智能 [bouffalolab](https://github.com/bouffalolab)|
| 捐赠：中国移动CM32M101A 开发板| 孟巍(微信名) |
| 捐赠：APM32F030R8 开发板 | 极海半导体 陈成 |
| 捐赠：APM32E103VB 开发板 | 极海半导体 陈成 |
| 捐赠：APEX-Link仿真器 | 极海半导体 陈成 |
| 源码格式化，增加git属性文件 | [Meco Jianting Man](https://github.com/mysterywolf) |
| demo/simulation-keil | 千帆(微信名) |
| demo/stm32f103zet6/demo01-led-stm32f103zet6 | [甜航](https://github.com/easyzoom) |
| demo/stm32f103zet6/demo02-led-stm32f103zet6_tworoot | [甜航](https://github.com/easyzoom) |
| demo/stm32f407zgt/demo01-led-stm32f407vgt | [甜航](https://github.com/easyzoom) |

# 6.内核测试与开发

## linux下测试pikascript内核(推荐ubuntu20.04)：

step1: 拉取项目
``` shell
git clone https://github.com/pikastech/pikascript
cd pikascript/src
```

step2: 编译项目
```
sh init.sh # 第一次编译前运行，之后就不用运行了 
sh make.sh # 编译项目
```

step3: 运行单元测试
``` shell	
sh test.sh
```

step4: 运行基准测试
``` shell
sh test-banchmark.sh
```

step5: 运行demo
``` shell
./build/src/boot/demo06-pikamain/pikascript_demo06-pikamain
```

# 7.Demo展示

## Demo 01 万物起源点个灯

![mmexport1631351506111](https://user-images.githubusercontent.com/88232613/132943903-b3558929-a107-4a99-bdc4-1b3fd3f7172b.png)

![Hnet-image (2)](https://user-images.githubusercontent.com/88232613/132943428-f2b365ca-140e-42f4-936c-db6a7d9f8dee.gif)

## Demo 02 串口测试

![mmexport1631351902469](https://user-images.githubusercontent.com/88232613/132944132-90898355-de94-4d81-990b-7b85d4a4d08a.png)


![Hnet-image (3)](https://user-images.githubusercontent.com/88232613/132943365-0f7059b3-4f9d-4989-a5ec-2cce72b0cc96.gif)


## Demo 03 读个ADC试试

![mmexport1631351527609](https://user-images.githubusercontent.com/88232613/132944180-a805c8f8-40d5-45ff-ae2a-a0fe8f9db1ab.png)


![mmexport1631351523907](https://user-images.githubusercontent.com/88232613/132944185-0a01b1ba-8cf7-4f9f-9d73-fe9cbcd52f0b.png)


## Demo 04 PWM输出

![image](https://user-images.githubusercontent.com/88232613/134461673-975498f4-09de-4f3a-866d-53e6fa1307b1.png)

## Demo 05 RGB流水灯

![image](https://user-images.githubusercontent.com/88232613/134461950-2153c738-0661-452f-956a-65a88fb71592.png)

这几个Demo占用的RAM最大值只有3.56K，把1K的堆栈也算上就是4.56K，Flash最大占用是30.4K，以STM32F103C8T6的20K RAM和64K Flash为标准，RAM才用掉不到25%，Flash才用掉不到50%。

我们可以简单对比一下micropython的常用芯片STM32F405RG和这次跑PikaScript的芯片STM32G070CB

## RAM资源对比
![image](https://user-images.githubusercontent.com/88232613/132944731-a55ece1d-061f-4b91-ba87-bd6547be96a7.png)

## Flash资源对比
![image](https://user-images.githubusercontent.com/88232613/132944745-e9cf598d-e75f-40bb-873e-911819d535b7.png)

## 参考价对比(以2021年9月11日立创商城10片售价为参考）
![image](https://user-images.githubusercontent.com/88232613/132944757-2b5cfda8-f93f-4456-8d7f-4e4767954056.png)

## 拓展能力如何呢？

除了设备驱动之外，为mcu开发自定义的python脚本绑定在pikascript的开发框架下非常轻松，下面两个Demo就是自定义的C模块拓展，这个Demo基于ARM-2D图像驱动库开发了一些python脚本接口。

## 几个小方块~
![Hnet-image (7)](https://user-images.githubusercontent.com/88232613/132945282-bfd310df-8063-456d-b90c-6b798a2c8ed5.gif)

## 几个旋转太阳~
![Hnet-image (6)](https://user-images.githubusercontent.com/88232613/132945107-e473a2cc-9fbc-47f9-aaed-a28d3ad1048c.gif)

