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
	<a href="https://www.yuque.com/liang-mltek/pikascript/nz7dgl?translate=en" >Documents</a> |
	<a href="https://space.bilibili.com/5365336/channel/detail?cid=202329">Videos</a> |
        <a href="../../tree/master/bsp#platform-support">BSP</a> |
	<a href="https://www.yuque.com/liang-mltek/pikascript/of8izq?translate=en">Package Manager</a> |
	<a href="https://www.yuque.com/liang-mltek/pikascript/wcc36r?translate=en" >RT-Thread package</a> |
	<a href="https://www.yuque.com/liang-mltek/pikascript/il3an0?translate=en" >Contribute</a> |
	<a href="https://www.yuque.com/liang-mltek/pikascript/cp1k24?translate=en">Business</a> 
</p>

[![image](https://user-images.githubusercontent.com/88232613/147997370-ff37b6e7-25b2-4174-aa64-c1fb92cede04.png)](https://pikastech.github.io/pikascript/dev/bench/)

# 1. Abstract

PikaScript is an ultra-lightweight Python engine with zero dependencies and zero-configuration, that can run with 4KB of RAM and 32KB of flash (such as STM32G030C8 and STM32F103C8).

<img src="https://user-images.githubusercontent.com/88232613/147799704-d79345da-a2b9-4593-b1eb-ff9de127429a.png" width="500"/>

It's very easy to bind C function to python module with the help of Pika Pre-compiler.

<img src="https://user-images.githubusercontent.com/88232613/138021445-13643518-b0a3-4872-a290-5dec27697837.png" width="700"/>

# [【Video】PikaScript hand by hand](https://www.bilibili.com/video/BV1mg411L72e)

# Get PikaScript:

## Use Online Project Generator
Generator url:
http://pikascript.com
![image](https://user-images.githubusercontent.com/88232613/152632673-019a7f92-7b7f-4fb5-92b7-4ae4c26ec865.png)


## Use Pika package manager [PikaPackage.exe](https://gitee.com/Lyon1998/pikascript/attach_files/907478/download)

1. Run the [PikaPackage.exe](https://gitee.com/Lyon1998/pikascript/attach_files/907478/download) directly, then the pikascript repo would be downloaded auto in the /tmp/pikascript folder of your current disc partition.

2. Add [requestment.txt](/bsp/stm32g030c8/pikascript/requestment.txt) to the same folder of PikaPackage.exe then run [PikaPackage.exe](../../raw/master/tools/pikaPackageManager/pikaPackage.exe). The [run-time core](../../tree/master/src), [pre-compiler](../../tree/master/tools/pikaCompiler) and [moudles](../../tree/master/package) would be installed in the current folder.

3. Released modules：[packages.toml](packages.toml)

# Quick Start

You can use [simulation project](document/1.三分钟快速上手.md) to quick start without hardware，or use the out-of-the-box develop board [Pika-Pi—Zero](https://item.taobao.com/item.htm?spm=a2126o.success.result.1.76224831Y0X1gO&id=654947372034).

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

## MCU support
|MCU|bsp|gpio|uart|pwm|adc|i2c|
|---|---|---|---|---|---|---|
|stm32g030c8|√|√|√|√|√|√|
|stm32g070cB|√|√|√|√|√|√|
|stm32f103c8|√|√|√|√|√|√|
|stm32f103rb|√|√|√|√|√|√|
|stm32f103rc|√|√|√|√|√|√|
|stm32f407ze|√| | | | | |
|stm32f051r8|√| | | | | |
|ch32v103r8t6|√|√| | | | |
|cm32m101a|√| | | | | |
|w801|√|√| | | | | | | |
|w806|√|√| | | | | | | |
|apm32f030r8|√| | | | | |
|apm32e103vb|√| | | | | |
|bl-706|√| | | | | |
|Raspberry Pico|√| | | | | |
|ESP32C3|√| | | | | |
|TC264D|√| | | | | |
|devc|√| | | | | |

## Board support
|Board|bsp|gpio|uart|pwm|adc|i2c|rgb|lcd|arm-2d|
|---|---|---|---|---|---|---|---|---|---|
|[Pika-Pi-Zero](https://item.taobao.com/item.htm?spm=a230r.1.14.1.4f2e27a8R0qWJn&id=654947372034&ns=1&abbucket=15#detail)|√|√|√|√|√|√|√|√|√|

|Board|bsp|arm-2d|
|---|---|---|
|QEMU-arm2d|√|√|

|Board|bsp|LED|KEY|
|---|---|---|---|
|SmartLoong|√|√|√|√|

## OS support

|OS|port|GPIO|TIME|
|---|---|---|---|
|RT-Thread|√|√|√|
|vsf|√|√| |

|OS|port|Google Test|Benchmark|
|---|---|---|---|
|linux|√|√|√|

# 3.Characteristic
### (1)Run environment

Support run in mcu without OS or file system. Can run in everywhere with **RAM ≥ 4kB** and **FLASH ≥ 32kB**，such as stm32g030, stm32f103c8t6，esp8266.

### (2)Develop enviroment
Support run and program python scripts by serial.

<img src="https://user-images.githubusercontent.com/88232613/134841230-85de6734-8467-4245-93a5-d452b5022b42.gif" width="400" alt="微信交流群"/><br/>

Support IDEs like Keil, IAR, RT-Thread studio and segger embedded studio to develop C moudle.

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
|Module Define    |√|-|
|Module Import    |√|-|
|Class Define     |√|-|
|Class Inherit    |√|-|
|Method Define    |√|√|
|Method Override  |√|√|
|Method Invoke    |√|√|
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
|√|√|√|√|√|√|√|
### (4)Develop standard.

Readability first, nearly never use macro function and global argument.

Complete unit testes based on google test.

# 4.Both talk and show the code：

## E-mail: liang6516@outlook.com

## Tencent QQ： 645275593

# 5.Contribute
| Content | Contributer |
| --- | --- |
| Performance Point: 900->1400 | [GorgonMeducer](https://github.com/GorgonMeducer) |
| stm32f051r8 BSP |[unsigned](https://gitee.com/unsigned0)|
| stm32f407ze BSP |[unsigned](https://gitee.com/unsigned0)|
| devc BSP |[unsigned](https://gitee.com/unsigned0)|
| TC264D BSP |[unsigned](https://gitee.com/unsigned0)|
| PikaVM improvement | [GorgonMeducer](https://github.com/GorgonMeducer) |
| W801Device package | 刘延(微信名) |
| W806 bsp | 刘延(微信名) |
| 捐赠：ESP32C3 5pic，调试器 |启明云端 沧御|
| 捐赠：LS1C101芯片10pic, LS1c101开发板，调试器|龙芯俱乐部 石南|
| PikaVSF OS package | [versaloon](https://github.com/versaloon) |
| ESP32C3 BSP |沧御|
| 捐赠：ESP32开发板、墨水屏*4 | name(微信名) |
| 捐赠：智龙开发板 | 龙芯俱乐部 石南 |
| package/STM32F1 | [sjy](https://gitee.com/shanjiayang) |
| package/STM32F103RBBooter | [sjy](https://gitee.com/shanjiayang) |
| bsp/stm32f103rb | [sjy](https://gitee.com/shanjiayang) |
| 基于QEMU的ARM-2D仿真工程 | [liuduanfei](https://github.com/liuduanfei) |
| 捐赠：GD32E103TB芯片2片 | 信息牛(微信名) |
| Rt-thread 支持包、适配层、模块| [Meco Jianting Man](https://github.com/mysterywolf) |
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

# 6.Test and develop Core

## Test core in linux(ubuntu20.04 is recommend)：

step1: Clone the repo
``` shell
git clone https://github.com/pikastech/pikascript
cd pikascript/port/linux
```

step2: Compile the project
```
sh install_dependency.sh # install dependency 
sh init.sh # Only nedded in the first time.
sh make.sh # Compile the project.
```

step3: Run the unit tests.
``` shell	
sh gtest.sh
```

step4: Run the banchmark
``` shell
sh ci_benchmark.sh
```

# 7.Demo 

The scripts in demos are in the [examples](examples) folder.

## Demo 01 GPIO

``` python
import PikaStdLib
import STM32G0

mem = PikaStdLib.MemChecker()
io1 = STM32G0.GPIO()
time = STM32G0.Time()

io1.init()
io1.setPin('PA8')
io1.setMode('out')
io1.enable()
io1.low()

print('hello pikascript')
print('mem.max :')
mem.max()
print('mem.now :')
mem.now()

while True:
    io1.low()
    time.sleep_ms(500)
    io1.high()
    time.sleep_ms(500)

```

![Hnet-image (2)](https://user-images.githubusercontent.com/88232613/132943428-f2b365ca-140e-42f4-936c-db6a7d9f8dee.gif)

## Demo 02 USART

``` python
import PikaStdLib
import STM32G0

time = STM32G0.Time()
uart = STM32G0.UART()
uart.init()
uart.setId(1)
uart.setBaudRate(115200)
uart.enable()

while True:
    time.sleep_ms(500)
    readBuff = uart.read(2)
    print('read 2 char:')
    print(readBuff)

```
![Hnet-image (3)](https://user-images.githubusercontent.com/88232613/132943365-0f7059b3-4f9d-4989-a5ec-2cce72b0cc96.gif)


## Demo 03 ADC

``` python
import PikaStdLib
import STM32G0

time = STM32G0.Time()
adc1 = STM32G0.ADC()

adc1.init()
adc1.setPin('PA1')
adc1.enable()

while True:
    val = adc1.read()
    print('adc1 value:')
    print(val)
    time.sleep_ms(500)

 ```

![mmexport1631351523907](https://user-images.githubusercontent.com/88232613/132944185-0a01b1ba-8cf7-4f9f-9d73-fe9cbcd52f0b.png)


## Demo 04 PWM output

``` python
import PikaStdLib
import STM32G0

time = STM32G0.Time()
pwm = STM32G0.PWM()
pwm.setPin('PA8')
pwm.setFrequency(2000)
pwm.setDuty(0.5)
pwm.enable()

while True:
    time.sleep_ms(500)
    pwm.setDuty(0.5)
    time.sleep_ms(500)
    pwm.setDuty(0.001)
    
```

## Demo 05 RGB

``` python
import STM32G0
import PikaPiZero
import PikaStdLib

rgb = PikaPiZero.RGB()
mem = PikaStdLib.MemChecker()

rgb.init()
rgb.enable()

print('hello 2')
print('mem used max:')
mem.max()

while True:
    print('flowing')
    rgb.flow()

```

## Demo 06 Snake(Need LCD)

``` python
from PikaObj import *
import PikaStdLib
import PikaPiZero
import STM32G0

# hardware init
lcd = PikaPiZero.LCD()
lcd.init()
lcd.clear('white')
key = PikaPiZero.KEY()
key.init()
time = STM32G0.Time()
x_max = 120
y_max = 150

# snake init
s = PikaPiZero.Point()
w = 9
h = 9
s.x = 50
s.y = 10
len = 0
while len < 3:
    b = s
    i = 0
    while i < len:
        b = b.next
        i = i + 1
    b.next = PikaPiZero.Point()
    b.next.x = b.x - 10
    b.next.y = b.y
    b.next.prev = b
    len = len + 1
# ring link
b.next = s
s.prev = b

i = 0
b = s
while i < len:
    lcd.fill(b.x, b.y, w, h, 'blue')
    b = b.next
    i = i + 1

print('snake lengh')
print(len)

# fruit init
f = PikaPiZero.Point()
f.x = 30
f.y = 20
lcd.fill(f.x, f.y, w, h, 'green')

# memory check
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()

# main loop
d = 0
isUpdate = 1
isEat = 0
while True:
    if isUpdate:
        # isUpdate = 0
        # check eat fruit
        if f.x == s.x and f.y == s.y:
            # have eat fruit
            isEat = 1
            f.x = f.x + 30
            if f.x > x_max:
                f.x = f.x - x_max
            f.y = f.y + 30
            if f.y > y_max:
                f.y = f.y - y_max
            lcd.fill(f.x, f.y, w, h, 'green')
        # move snake by the direction
        if d == 0:
            x_new = s.x + 10
            y_new = s.y
            if x_new > x_max:
                x_new = 0
        elif d == 1:
            x_new = s.x
            y_new = s.y - 10
            if y_new < 0:
                y_new = y_max
        elif d == 2:
            x_new = s.x
            y_new = s.y + 10
            if y_new > y_max:
                y_new = 0
        elif d == 3:
            x_new = s.x - 10
            y_new = s.y
            if x_new < 0:
                x_new = x_max
        if isEat:
            isEat = 0
            b_new = PikaPiZero.Point()
            b_new.x = x_new
            b_new.y = y_new
            b_new.prev = s.prev
            b_new.next = s
            s.prev.next = b_new
            s.prev = b_new
            s = b_new
            len = len + 1
            print('snake lengh')
            print(len)
            print('mem used max:')
            mem.max()
        # drow the snake and fruit
        # clear last body
        lcd.fill(s.prev.x, s.prev.y, w, h, 'white')
        # new body
        s.prev.x = x_new
        s.prev.y = y_new
        # head is last body
        s = s.prev
        lcd.fill(s.x, s.y, w, h, 'blue')
        b = s
        i = 0
    # scan key
    key_val = key.get()
    if key_val == 0:
        d = 0
        isUpdate = 1
    elif key_val == 1:
        d = 1
        isUpdate = 1
    elif key_val == 2:
        d = 2
        isUpdate = 1
    elif key_val == 3:
        d = 3
        isUpdate = 1
	
```

![image](https://user-images.githubusercontent.com/88232613/150269059-1fe3d62d-0c1b-40f4-9298-0d787f476a20.png)

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

