<img src="https://user-images.githubusercontent.com/88232613/132158500-f0818be9-29b2-48a1-b1e8-3216c3b686b0.jpg" width="400" alt="微信交流群"/><br/>

# [点击获取PikaScript v0.7.0](https://github.com/mimilib/pikascript/releases/download/v0.7.0/pikascript.v0.7.0.zip)

# 1.简介

PikaScript是一个超轻量级python引擎，零依赖，零配置，可以在少于4KB的RAM下运行(如stm32g030c8和stm32f103c8)，极易部署和扩展。

PikaScript is an ultra lightweight Python engine with zero dependencies and zero configuration, that can run with 4KB of RAM (such as STM32G030C8 and STM32F103C8), and very easy to deploy and expand.

![output_Kgj52R](https://user-images.githubusercontent.com/88232613/132940452-d07d766c-5aa7-4187-96f2-66b02984e82c.gif)

# 2.驱动适配进度

# 3.特性
### (1)运行环境

支持裸机运行，可运行于 **RAM ≥ 4kB** ，**FLASH ≥ 32kB** 的mcu中，如stm32g030, stm32f103c8t6，esp8266。

### (2)开发环境

支持Keil、IAR、rt-thread studio、segger embedded studio等IDE开发。

支持CMake、makeFile、Scons等构建工具

零依赖，零配置，开箱即用，极易集成进已有的C工程。

极易拓展自定义的C原生函数。

支持跨平台，可在linux环境开发内核。

### (3)语法特性

使用python3标准语法的子集。

在编译时支持python类和方法定义，完整支持封装、继承、多态、模块功能 - 基于[Pika预编译器](../../tree/master/pikascript-compiler-rust)。

在运行时支持python方法调用、变量定义、对象构造、对象释放、控制流(if\while) - 基于[Pika运行时内核](../../tree/master/src/package/pikascript/pikascript-core)。

### (4)源码规范

注重源码可读性，命名规范，标准统一，完全不使用宏，几乎不使用全局变量。

完整的googletest单元测试。

### (5)stm32例程

[pikascript-stm32-demo](../../tree/master/demo)

### (6)开发手册

[1. PikaScript-三分钟快速上手](doc/1.三分钟快速上手.md)

[2. PikaScript-架构与原理](https://mp.weixin.qq.com/s?__biz=MzU4NzUzMDc1OA==&mid=2247484127&idx=1&sn=f66cff49c488e48c52570c7bb570328f&chksm=fdebd5b6ca9c5ca0707fd221c32f3ad63e94aeb6f917a92774b89ea042381ea261990f5cca3c&token=2045971639&lang=zh_CN#rd)

[3. PikaScript-十分钟快速部署](doc/2.十分钟快速部署.md)

[4. PikaScript-标准开发流程](doc/3.PikaScript标准开发流程.md)

[5. PikaScript-Demo展示](doc/5.我就要用最便宜的单片机来跑python，还要用Keil开发，怎么地吧.md)
### (7)架构示意图

<img src="https://user-images.githubusercontent.com/88232613/127806449-b476b2fd-9f40-4c53-94a0-e1e965c046c3.png" width="800" alt="微信交流群"/><br/>

# 2.stm32教程视频

[手把手PikaScript教程合集](https://www.bilibili.com/video/BV1mg411L72e)

# 4.交流与技术支持：

## 微信交流群

![微信截图_20210917133247](https://user-images.githubusercontent.com/88232613/133729804-e1d0406b-a859-4ebc-b335-477e9c03810e.png)

### 群已经满啦，加我QQ拉你进群~ qq: 645275593

## Tencent QQ： 645275593

## E-mail: 645275593@qq.com

# 5.贡献者
| 内容 | 贡献者 |
| --- | --- |
| 捐赠：APM32F030R8 开发板 | 极海半导体 陈成 |
| 捐赠：APM32E103VB 开发板 | 极海半导体 陈成 |
| 捐赠：APEX-Link仿真器 | 极海半导体 陈成 |
| 源码格式化 | [Meco Jianting Man](https://github.com/mysterywolf) |
| demo/simulation-keil | 千帆(微信名) |
| demo/stm32f103zet6/demo01-led-stm32f103zet6 | [甜航](https://github.com/easyzoom) |
| demo/stm32f103zet6/demo02-led-stm32f103zet6_tworoot | [甜航](https://github.com/easyzoom) |
| demo/stm32f407zgt/demo01-led-stm32f407vgt | [甜航](https://github.com/easyzoom) |



# 6.推广稿：我就要用最便宜的单片机来跑python，还要用MDK开发，怎么地吧！

我就想用单片机跑个Python，得用linux虚拟机+交叉编译工具链+命令行编译micropython固件，还得用DfuSe工具烧录固件，烧录完还不能用C的调试器来调试。

我想拓展个自己的C模块，还要学着用一些完全看不懂的宏函数，还得手动注册，还得编写makeFile，编译完照样不能调试C。

我穷，买不起STM32F4，想买个STM32F103C8T6的micropython开发板，淘宝一搜，好像没有。

现在C8T6贵了，我还想用F0，用G0，能行吗？

好像，给G0移植micropython，不是很容易。

那？有没有另一种玩法？

换句话说，我想用Keil开发，用Keil调试，我还想用最便宜的单片机。

这，能玩Python吗？

![output_Kgj52R](https://user-images.githubusercontent.com/88232613/132941900-985ebc9e-fb65-48f6-8677-d3ebc65422ee.gif)

要不，试试PikaScript？

啥是PikaScript？

PikaScript可以为资源受限的mcu提供极易部署和拓展的Python脚本支持。

PikaScript支持裸机运行，最低可运行于 RAM ≥ 4kB ，FLASH ≥ 32kB 的mcu中，推荐配置为 RAM ≥ 10kB， FLASH ≥ 64kB，像是stm32f103c8t6、stm32g070RBT6这些完全没有压力，甚至已经满足了推荐配置。

而且支持Keil、IAR、rt-thread studio、segger embedded studio等IDE开发，零依赖，零配置，开箱即用，极易集成进已有的C工程。

说了这么多，刘华强就有疑问了，你说这脚本，保熟吗？

![QQ截图20210911174834](https://user-images.githubusercontent.com/88232613/132943877-81031a9a-d152-4143-9cad-96db61e9b000.png)

我这开~~水果~~脚本摊儿的，能买你生脚本蛋子？

这就挑点儿Demo给哥儿几个瞧瞧。

这可都是STM32G070RBT6的Demo。

## Demo 01 万物起源点个灯

![mmexport1631351506111](https://user-images.githubusercontent.com/88232613/132943903-b3558929-a107-4a99-bdc4-1b3fd3f7172b.png)

看看这脚本，可都是如假包换的Python3标准语法。

这灯不就闪起来了吗。

![Hnet-image (2)](https://user-images.githubusercontent.com/88232613/132943428-f2b365ca-140e-42f4-936c-db6a7d9f8dee.gif)

## Demo 02 串口测试

![mmexport1631351902469](https://user-images.githubusercontent.com/88232613/132944132-90898355-de94-4d81-990b-7b85d4a4d08a.png)

开个串口，读俩字符试试

![Hnet-image (3)](https://user-images.githubusercontent.com/88232613/132943365-0f7059b3-4f9d-4989-a5ec-2cce72b0cc96.gif)

非常顺滑

## Demo 03 读个ADC试试

![mmexport1631351527609](https://user-images.githubusercontent.com/88232613/132944180-a805c8f8-40d5-45ff-ae2a-a0fe8f9db1ab.png)

同样几行脚本搞定。

![mmexport1631351523907](https://user-images.githubusercontent.com/88232613/132944185-0a01b1ba-8cf7-4f9f-9d73-fe9cbcd52f0b.png)

这是输出的结果。

这几个Demo占用的RAM最大值只有3.56K，把1K的堆栈也算上就是4.56K，Flash最大占用是30.4K，以STM32F103C8T6的20K RAM和64K Flash为标准，RAM才用掉不到25%，Flash才用掉不到50%，简直是资源多到不知道咋霍霍。

同样跑Python，我们可以简单对比一下micropython的常用芯片STM32F405RG和这次跑PikaScript的芯片STM32G070CB

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

## 那，PikaScript是开源的吗？
当然，这个就是PikaScript的github主页：
https://github.com/pikasTech/pikascript

## 开发难不难？
PikaScript为开发者准备了丰富的Demo和由浅入深的开发指南，指南还会持续完善和维护。
![image](https://user-images.githubusercontent.com/88232613/132945342-6ace05aa-50c4-4533-9129-ef131cd9fc1d.png)

## 可以商用吗？
当然！PikaScript采用MIT协议，允许修改和商用，但是要注意保留原作者的署名。

# 6.内核测试与开发

## linux下测试pikascript内核(推荐ubuntu20.04)：

step1: 拉取项目
``` shell
git clone https://github.com/mimilib/pikascript
cd pikascript
```

step2: 编译项目
```
sh init.sh # 第一次编译前运行，之后就不用运行了 
sh make.sh # 编译项目
```

step3: 运行单元测试
``` shell	
sh test
```

step4: 运行基准测试
``` shell
sh test-banchmark
```

step5: 运行demo
``` shell
./build/src/boot/demo06-pikamain/pikascript_demo06-pikamain
```

