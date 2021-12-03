<p align="center">
	<img alt="logo" src="https://user-images.githubusercontent.com/88232613/144221063-a85b2cb0-0416-493f-9afb-56cff31e977d.jpg" width="300">
</p>
<h1 align="center" style="margin: 30px 0 30px; font-weight: bold;">PikaScript</h1>
<h4 align="center">Cross platform ultra lightweight embedded Python engine</h4>
<p align="center">
	<a href="https://gitee.com/lyon1998/pikascript/stargazers"><img src="https://gitee.com/lyon1998/pikascript/badge/star.svg?theme=gvp"></a>
	<a href="https://gitee.com/lyon1998/pikascript/members"><img src="https://gitee.com/lyon1998/pikascript/badge/fork.svg?theme=gvp"></a>
	<a href="https://github.com/pikastech/pikascript/stargazers"><img src="https://img.shields.io/github/stars/pikastech/pikascript?style=flat-square&logo=GitHub"></a>
	<a href="https://github.com/pikastech/pikascript/network/members"><img src="https://img.shields.io/github/forks/pikastech/pikascript?style=flat-square&logo=GitHub"></a>
	<a href="https://github.com/pikastech/pikascript/watchers"><img src="https://img.shields.io/github/watchers/pikastech/pikascript?style=flat-square&logo=GitHub"></a>
	<a href="https://github.com/pikastech/pikascript/issues"><img src="https://img.shields.io/github/issues/pikastech/pikascript.svg?style=flat-square&logo=GitHub"></a>
	<a href="https://github.com/pikastech/pikascript/blob/master/LICENSE"><img src="https://img.shields.io/github/license/pikastech/pikascript.svg?style=flat-square"></a>
  <a href="https://github.com/pikasTech/pikascript/actions/workflows/CI.yml"><img src="https://github.com/pikasTech/pikascript/actions/workflows/CI.yml/badge.svg"> </a>
</p>

<p align="center">
	<a href="README_zh.md" >中文页</a> |
	<a href="https://whycan.com/f_55.html" >Forum</a> |
	<a href="https://www.yuque.com/liang-mltek/pikascript" >Documents</a> |
	<a href="https://space.bilibili.com/5365336/channel/detail?cid=202329">Videos</a> |
	<a href="https://gitee.com/Lyon1998/pikascript/raw/master/tools/pikaPackageManager/pikaPackage.exe">Package Manager</a> 
</p>

# 1. Abstract

PikaScript is an ultra-lightweight Python engine with zero dependencies and zero-configuration, that can run with 4KB of RAM and 32KB of flash (such as STM32G030C8 and STM32F103C8).

<img src="https://user-images.githubusercontent.com/88232613/138018674-93a7991a-5367-4fbd-92ee-95c7f0f157fc.png" width="500"/>

It's very easy to bind C function to python module with the help of Pika Pre-compiler.

<img src="https://user-images.githubusercontent.com/88232613/138021445-13643518-b0a3-4872-a290-5dec27697837.png" width="700"/>

# [【Video】PikaScript hand by hand](https://www.bilibili.com/video/BV1mg411L72e)

# Get PikaScript:

## Use Pika package manager [PikaPackage.exe](../../raw/master/tools/pikaPackageManager/pikaPackage.exe)

1. Run the [PikaPackage.exe](../../raw/master/tools/pikaPackageManager/pikaPackage.exe) directly, then the pikascript repo would be downloaded auto in the /tmp/pikascript folder of your current disc partition.

2. Add [requestment.txt](/bsp/stm32g030c8/pikascript/requestment.txt) to the same folder of PikaPackage.exe then run [PikaPackage.exe](../../raw/master/tools/pikaPackageManager/pikaPackage.exe). The [run-time core](../../tree/master/src), [pre-compiler](../../tree/master/tools/pikaCompiler) and [moudles](../../tree/master/package) would be installed in the current folder.

3. Released modules：[packages.toml](packages.toml)

# Quick Start

You can use [simulation project](document/1.三分钟快速上手.md) to quick start without hardware，or use the out-of-the-box develop board [Pika-Pi—Zero](https://item.taobao.com/item.htm?spm=a1z10.3-c.w4023-23991764790.10.16f97c58fsLjVk&id=654947372034).

![image](https://user-images.githubusercontent.com/88232613/141252834-93011ca7-f84b-4192-8e61-0e242796f62c.png)

The board based on STM32G030C8T6 MCU, only 64kB Flash, 8kB RAM can run PikaScript with total peripheral device (GPIO、TIME、IIC、RGB、KEY、LCD、RGB). 
CH340 is deployed to support USB to serial with Type-C USB, support download python script py serial, there are 4 RGB on the board and support the [LCD](https://item.taobao.com/item.htm?spm=a1z10.3-c.w4002-23991764791.12.16f97c58fsLjVk&id=660745643102).

# Document

[1. PikaScript-Quick start within 3 minutes](document/1.三分钟快速上手.md)

[2. PikaScript-Architecture and principle](https://mp.weixin.qq.com/s?__biz=MzU4NzUzMDc1OA==&mid=2247484127&idx=1&sn=f66cff49c488e48c52570c7bb570328f&chksm=fdebd5b6ca9c5ca0707fd221c32f3ad63e94aeb6f917a92774b89ea042381ea261990f5cca3c&token=2045971639&lang=zh_CN#rd)

[3. PikaScript-Deploy within 10 minutes](document/2.十分钟快速部署.md)

[4. PikaScript-Standard develop workflow](document/3.PikaScript标准开发流程.md)

[5. PikaScript-Demo show](document/5.我就要用最便宜的单片机来跑python，还要用Keil开发，怎么地吧.md)

[6. Arm-2D simulation](document/6.用Python玩ARM%202D，认真的嘛.md)

## Folders
[src](../../tree/master/src) - core code

[bsp](../../tree/master/bsp) - mcu/board support

[port](../../tree/master/port) - OS support

[document](../../tree/master/document) - developt document

[examples](../../tree/master/examples) - example scripts

[package](../../tree/master/package) - packages and moudles

[pikaCompiler](../../tree/master/tools/pikaCompiler) - pre-compiler write by Rust, used to bind C function to python moudle.

[pikaPackageManager](../../tree/master/tools/pikaPackageManager) - pacakge manager

# 2.Platform support
|MCU/Board|bsp|gpio|uart|pwm|adc|i2c|spi|rgb|lcd|arm-2d|
|---|---|---|---|---|---|---|---|---|---|---|
|[PikaPi Zero](https://item.taobao.com/item.htm?spm=a230r.1.14.1.4f2e27a8R0qWJn&id=654947372034&ns=1&abbucket=15#detail)|√|√|√|√|√|√| |√|√|√|
|stm32g030c8|√|√|√|√|√|√| | | | |
|stm32g070cB|√|√|√|√|√| | | | | |
|stm32f103c8|√|√|√|√|√| | | | | |
|ch32v103r8t6|√|√| | | | | | | | |
|QEMU|√| | | | | | | | |√|
|cm32m101a|√| | | | | | | | | |
|w801|√| | | | | | | | | |
|apm32f030r8|√| | | | | | | | | |
|apm32e103vb|√| | | | | | | | | |
|bl-706|√| | | | | | | | | |

# 3.Characteristic
### (1)Run environment

Support run in mcu without OS or file system. Can run in everywhere with **RAM ≥ 4kB** and **FLASH ≥ 32kB**，such as stm32g030, stm32f103c8t6，esp8266.

### (2)Develop enviroment
Support run and program python scripts by serial.

<img src="https://user-images.githubusercontent.com/88232613/134841230-85de6734-8467-4245-93a5-d452b5022b42.gif" width="400" alt="微信交流群"/><br/>

Support IDEs like Keil, IAR, rt-thread studio and segger embedded studio to develop C moudle.

Support build tools like CMake, makeFile and Scons.

Zero dependencies, zero configuration,  out-of-the-box, easy to integrated into privious C projcet.

Eazy to extern customized C moudles.

Support linux.

### (3)Syntax 

Support subaggregate of python 3 standard syntax.

Support class and method define, encapsulation-inheritance-polymorphism and moudles in compile time - Based on [Pika Pre-compiler](../../tree/master/tools/pikaCompiler).

Support method invoke, argument define, new object, free object, control flow(if\while) and operator ( + - * / < == > ). - Based on [Pika Runtime Core](../../tree/master/src/package/pikascript/pikascript-core).

|Syntax|Compile-Time|Run-Time|
|---|---|---|
|Class Define     |√| |
|Class Inherit    |√| |
|Method Define    |√|√|
|Method Override  |√|√|
|Method Invoke    |√|√|
|Module Import    |√|√|
|Argument Define  |√|√|
|Argument Assignment  |√|√|
|Object New       |√|√|
|Object Free      |√|√|
|Object Nest      |√|√|

#### Operator

| + | - | * | / | == | > | < | >= | <= | % | ** | // | != | & | >> | << | and | or | not | += | -= | *= | /= |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|√|√|√|√|√|√|√|√|√|√|√|√|√|√|√|√|√|√|√| | | | |

#### Control flow

| if | while | for | else | elif | break | continue |
| --- | --- | --- | --- | --- | --- | --- |
|√|√| | | | | |
### (4)Develop standard.

Readability first, nearly never use macro function and global argument.

Complete unit testes based on google test.

# 4.Both talk and show the code：

## E-mail: liang6516@outlook.com

## Tencent QQ： 645275593

## QQ group:

<img src="https://user-images.githubusercontent.com/88232613/136304186-e49610e4-ce01-4524-99b6-b56842d56411.png" width="300" alt="微信交流群"/><br/>

# 5.Contribute
| Content | Contributer |
| --- | --- |
| QEMU simulation with ARM-2D | [liuduanfei](https://github.com/liuduanfei) |
| Donate：GD32E103TB muc 2 pic | 信息牛(WeChat) |
| Rt-thread bsp template| [Meco Jianting Man](https://github.com/mysterywolf) |
| Donate：EC600S-CN 4G module | 移远模块 |
| Donate：BL706 board|博流智能 [bouffalolab](https://github.com/bouffalolab)|
| Donate：CM32M101A board| 孟巍(WeChat) |
| Donate：APM32F030R8 board | 极海半导体 陈成 |
| Donate：APM32E103VB board | 极海半导体 陈成 |
| Donate：APEX-Link debugger | 极海半导体 陈成 |
| format code，add gitattribute | [Meco Jianting Man](https://github.com/mysterywolf) |
| demo/simulation-keil | 千帆(WeChat) |
| demo/stm32f103zet6/demo01-led-stm32f103zet6 | [甜航](https://github.com/easyzoom) |
| demo/stm32f103zet6/demo02-led-stm32f103zet6_tworoot | [甜航](https://github.com/easyzoom) |
| demo/stm32f407zgt/demo01-led-stm32f407vgt | [甜航](https://github.com/easyzoom) |

# 6.Test and develop Core

## Test core in linux(ubuntu20.04 is recommend)：

step1: Clone the repo
``` shell
git clone https://github.com/pikastech/pikascript
cd pikascript/port/linux
```

step2: Compile the project
```
sh init.sh # Only nedded in the first time.
sh make.sh # Compile the project.
```

step3: Run the unit tests.
``` shell	
sh gtest.sh
```

step4: Run the banchmark
``` shell
sh test-banchmark.sh
```

step5: Run the demo
``` shell
./build/src/boot/demo06-pikamain/pikascript_demo06-pikamain
```

# 7.Demo 

The scripts in demos are in the [examples](examples) folder.

## Demo 01 GPIO

![mmexport1631351506111](https://user-images.githubusercontent.com/88232613/132943903-b3558929-a107-4a99-bdc4-1b3fd3f7172b.png)

![Hnet-image (2)](https://user-images.githubusercontent.com/88232613/132943428-f2b365ca-140e-42f4-936c-db6a7d9f8dee.gif)

## Demo 02 USART

![mmexport1631351902469](https://user-images.githubusercontent.com/88232613/132944132-90898355-de94-4d81-990b-7b85d4a4d08a.png)


![Hnet-image (3)](https://user-images.githubusercontent.com/88232613/132943365-0f7059b3-4f9d-4989-a5ec-2cce72b0cc96.gif)


## Demo 03 ADC

![mmexport1631351527609](https://user-images.githubusercontent.com/88232613/132944180-a805c8f8-40d5-45ff-ae2a-a0fe8f9db1ab.png)


![mmexport1631351523907](https://user-images.githubusercontent.com/88232613/132944185-0a01b1ba-8cf7-4f9f-9d73-fe9cbcd52f0b.png)


## Demo 04 PWM output

![image](https://user-images.githubusercontent.com/88232613/134461673-975498f4-09de-4f3a-866d-53e6fa1307b1.png)

## Demo 05 RGB

![image](https://user-images.githubusercontent.com/88232613/134461950-2153c738-0661-452f-956a-65a88fb71592.png)

The maximum RAM usage of these demos is only 3.56K, which is 4.56K if the 1K stack is included, and the maximum Flash usage is 30.4K. Refer to the 20K RAM and 64K Flash of STM32F103C8T6, less than 25% RAM and less than 50% Flash are used.

A quick comparison can be made between the microPython chip STM32F405RG and the PikaScript chip STM32G070CB.

## RAM resource comparison
![image](https://user-images.githubusercontent.com/88232613/132944731-a55ece1d-061f-4b91-ba87-bd6547be96a7.png)

## Flash Resource Comparison
![image](https://user-images.githubusercontent.com/88232613/132944745-e9cf598d-e75f-40bb-873e-911819d535b7.png)

## Reference price comparison (take the selling price of 10 pieces in Lichuang Mall on September 11, 2021 as reference)
![image](https://user-images.githubusercontent.com/88232613/132944757-2b5cfda8-f93f-4456-8d7f-4e4767954056.png)

## About the custom extern module ？

In addition to device drivers, developing custom Python modules for MCU is very easy. The following two demos are extensions of the custom module, which developed python interfaces for the ARM-2D image driver library.

## Boxes~
![Hnet-image (7)](https://user-images.githubusercontent.com/88232613/132945282-bfd310df-8063-456d-b90c-6b798a2c8ed5.gif)

## Rotating suns~
![Hnet-image (6)](https://user-images.githubusercontent.com/88232613/132945107-e473a2cc-9fbc-47f9-aaed-a28d3ad1048c.gif)

