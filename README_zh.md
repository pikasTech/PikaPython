<p align="center">
	<img alt="logo" src="https://user-images.githubusercontent.com/88232613/144221063-a85b2cb0-0416-493f-9afb-56cff31e977d.jpg" width="300">
</p>
<h1 align="center" style="margin: 30px 0 30px; font-weight: bold;">PikaScript</h1>
<h4 align="center">跨平台的超轻量级嵌入式 Python 引擎</h4>
<p align="center">
	<a href="https://gitee.com/lyon1998/pikascript/stargazers"><img src="https://gitee.com/lyon1998/pikascript/badge/star.svg?theme=gvp"></a>
	<a href="https://gitee.com/lyon1998/pikascript/members"><img src="https://gitee.com/lyon1998/pikascript/badge/fork.svg?theme=gvp"></a>
	<a href="https://github.com/pikastech/pikascript/stargazers"><img src="https://img.shields.io/github/stars/pikastech/pikascript?style=flat-square&logo=GitHub"></a>
	<a href="https://github.com/pikastech/pikascript/network/members"><img src="https://img.shields.io/github/forks/pikastech/pikascript?style=flat-square&logo=GitHub"></a>
	<a href="https://github.com/pikastech/pikascript/blob/master/LICENSE"><img src="https://img.shields.io/github/license/pikastech/pikascript.svg?style=flat-square"></a>
        <a href="https://github.com/pikasTech/pikascript/actions/workflows/CI.yml"><img src="https://github.com/pikasTech/pikascript/actions/workflows/CI.yml/badge.svg"> </a>
</p>

<p align="center">
	<a href="https://whycan.com/f_55.html" >论坛</a> |
	<a href="https://www.yuque.com/liang-mltek/pikascript/nz7dgl" >文档中心</a> |
	<a href="https://space.bilibili.com/5365336/channel/seriesdetail?sid=1034902">视频</a> |
        <a href="https://m.tb.cn/h.fjS6hpu?tk=LIpQ2b3G2Sf">开发板</a> |
        <a href="../../tree/master/bsp#platform-support">BSP</a> |
	<a href="https://www.yuque.com/liang-mltek/pikascript/of8izq">包管理器</a> |
	<a href="https://www.yuque.com/liang-mltek/pikascript/wcc36r" >RT-Thread 软件包</a> |
	<a href="https://www.yuque.com/liang-mltek/pikascript/il3an0" >参与贡献</a> |
	<a href="https://www.yuque.com/liang-mltek/pikascript/cp1k24">商业合作</a> 
</p>

[![image](https://user-images.githubusercontent.com/88232613/147997370-ff37b6e7-25b2-4174-aa64-c1fb92cede04.png)](https://pikastech.github.io/pikascript/dev/bench/)

# 1.简介

PikaScript 是一个完全重写的超轻量级 python 引擎，零依赖，零配置，可以在少于 4KB 的 RAM 下运行(如 stm32g030c8 和 stm32f103c8 )，极易部署和扩展，具有大量的中文文档和视频资料。

PikaScript 具有框架式C模块开发工具，只要用 Python 写好调用 API ，就能够自动连接到C模块，非常方便快捷。不用手动处理任何全局表、宏定义、等等。

PikaScript 也支持 MDK、iar、RT-studio 等常见 ide 开发，能够轻松地调试C模块。

![output_Kgj52R](https://user-images.githubusercontent.com/88232613/132940452-d07d766c-5aa7-4187-96f2-66b02984e82c.gif)

<img src="https://user-images.githubusercontent.com/88232613/147799764-5db2cb30-ee74-4cde-a2bd-b91c358ae3d9.png" width="500"/>

![image](https://user-images.githubusercontent.com/88232613/138021445-13643518-b0a3-4872-a290-5dec27697837.png)

# 获取 PikaScript:

## 使用 Pika 包管理器[PikaPackage.exe](https://gitee.com/Lyon1998/pikascript/attach_files/907478/download)

1. 直接运行[PikaPackage.exe](https://gitee.com/Lyon1998/pikascript/attach_files/907478/download), 自动下载并更新pikaScript主仓库(文件在当前磁盘的/tmp/pikaScript文件夹)

2. 将[requestment.txt](/bsp/stm32g030c8/pikascript/requestment.txt)放在[PikaPackage.exe](../../raw/master/tools/pikaPackageManager/pikaPackage.exe)同一文件夹下, 运行[pikaPackage.exe](../../raw/master/tools/pikaPackageManager/pikaPackage.exe), 自动在当前目录下安装[内核](../../tree/master/src)、[预编译器](../../tree/master/tools/pikaCompiler)与[模块](../../tree/master/package)。

3. 已发布的模块列表：[packages.toml](/packages.toml)

# 快速上手

可使用[仿真工程](document/1.三分钟快速上手.md)快速上手，无需硬件，也可以使用官方支持的开发板[Pika派—Zero](https://item.taobao.com/item.htm?spm=a2126o.success.result.1.76224831Y0X1gO&id=654947372034)，上手即玩。

[Pika派python编程游玩指南](https://www.yuque.com/liang-mltek/pikascript/okdwig)

[![PikaPi-Zero](https://user-images.githubusercontent.com/88232613/147940229-a6b8ebf5-41db-4614-a525-448f47de0a67.png)](https://item.taobao.com/item.htm?spm=a2126o.success.result.1.76224831Y0X1gO&id=654947372034)

开发板基于 STM32G030C8T6 小资源 MCU，仅 64kB Flash，8kB RAM 即可运行完整解释器，和完整的外设驱动（GPIO、TIME、IIC、UART、ADC、PWM、RGB、KEY、LCD）。板载 CH340 USB 转串口芯片，Type-C 接口，支持串口下载 python 脚本，板载4颗 RGB 灯，可选配[LCD屏幕](https://item.taobao.com/item.htm?spm=a1z10.3-c.w4002-23991764791.12.16f97c58fsLjVk&id=660745643102)。

# 开发手册

### [点此进入文档中心](https://www.yuque.com/liang-mltek/pikascript)

![image](https://user-images.githubusercontent.com/88232613/144693400-99f9c038-76fd-4d95-b3d2-137bd972d580.png)

# 视频教程

### [点此进入视频中心](https://space.bilibili.com/5365336/channel/seriesdetail?sid=1034902)

![image](https://user-images.githubusercontent.com/88232613/142173892-35e33f36-413c-4422-8470-b873b7c3bd71.png)

# 交流论坛

### [点此进入论坛](https://whycan.com/f_55.html)

![image](https://user-images.githubusercontent.com/88232613/144693543-4aee46c8-b6c8-4282-99c4-e07271a4ba5f.png)

## 文件目录
[src](../../tree/master/src) - 内核源码

[bsp](../../tree/master/bsp) - 裸机芯片/板卡支持

[port](../../tree/master/port) - 操作系统支持

[document](../../tree/master/document) - 开发文档

[examples](../../tree/master/examples) - 示例脚本

[package](../../tree/master/package) - 模块目录

[pikaCompiler](../../tree/master/tools/pikaCompiler) - 使用 rust 编写的预编译器

[pikaPackageManager](../../tree/master/tools/pikaPackageManager) - 使用 go 编写的模块管理器

# 2.平台支持列表

## MCU support
|MCU|bsp|gpio|uart|pwm|adc|i2c|
|---|---|---|---|---|---|---|
|stm32g030c8|√|√|√|√|√|√|
|stm32g070cB|√|√|√|√|√|√|
|stm32f103c8|√|√|√|√|√|√|
|stm32f103rb|√|√|√|√|√|√|
|stm32f103rc|√|√|√|√|√|√|
|ch32v103r8t6|√|√| | | | |
|cm32m101a|√| | | | | |
|w801|√|√| | | | | | | |
|w806|√|√| | | | | | | |
|apm32f030r8|√| | | | | |
|apm32e103vb|√| | | | | |
|bl-706|√| | | | | |
|Raspberry Pico|√| | | | | |
|ESP32C3|√| | | | | |

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
|rt-thread|√|√|√|
|vsf|√|√| |

|OS|port|Google Test|Benchmark|
|---|---|---|---|
|linux|√|√|√|

# 3.特性
### (1)运行环境

支持裸机运行，可运行于 **RAM ≥ 4kB** ，**FLASH ≥ 32kB** 的mcu中，如stm32g030, stm32f103c8t6，esp8266。

### (2)开发环境
支持串口下载 Python 脚本。

<img src="https://user-images.githubusercontent.com/88232613/134841230-85de6734-8467-4245-93a5-d452b5022b42.gif" width="400" alt="微信交流群"/><br/>

支持 Keil、IAR、rt-thread studio、segger embedded studio 等IDE开发。

支持 CMake、makeFile、Scons 等构建工具

零依赖，零配置，开箱即用，极易集成进已有的C工程。

极易拓展自定义的C原生函数。

支持跨平台，可在 linux 环境开发内核。

### (3)语法特性

使用 python3 标准语法的子集。

在编译时支持 python 类和方法定义，完整支持封装、继承、多态、模块功能 - 基于 [Pika 预编译器](../../tree/master/tools/pikaCompiler)。

在运行时支持 python 方法调用、变量定义、对象构造、对象释放、控制流(if\while) - 基于 [Pika 运行时内核](../../tree/master/src/package/pikascript/pikascript-core)。

|语法|编译时|运行时|
|---|---|---|
|模块定义|√|-|
|模块导入|√|-|
|类定义|√|-|
|类继承|√|-|
|方法定义|√|√|
|方法重载|√|√|
|方法调用|√|√|
|参数定义|√|√|
|参数赋值|√|√|
|对象新建|√|√|
|对象销毁|√|√|
|对象嵌套|√|√|

#### 操作符

| + | - | * | / | == | > | < | >= | <= | % | ** | // | != | & | >> | << | and | or | not | += | -= | *= | /= |
|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|---|
|√|√|√|√|√|√|√|√|√|√|√|√|√|√|√|√|√|√|√| | | | |

#### 控制流

| if | while | for | else | elif | break | continue |
| --- | --- | --- | --- | --- | --- | --- |
|√|√|√|√|√|√|√|

#### 数据结构

| string | list | dict |
| --- | --- | --- |
|√|√|√|

### (4)源码规范

注重源码可读性，命名规范，标准统一，完全不使用宏，几乎不使用全局变量。

完整的 googletest 单元测试。

# 4.交流与技术支持：

## 微信交流群

![微信截图_20210917133247](https://user-images.githubusercontent.com/88232613/133729804-e1d0406b-a859-4ebc-b335-477e9c03810e.png)

### 群已经满啦，加我QQ拉你进群~ qq: 645275593

## Tencent QQ： 
645275593

## Tencent QQ Group:

<img src="https://user-images.githubusercontent.com/88232613/136304186-e49610e4-ce01-4524-99b6-b56842d56411.png" width="300" alt="微信交流群"/><br/>

## E-mail: 
645275593@qq.com

# 5.贡献者
| 内容 | 贡献者 |
| --- | --- |
| PikaVM 优化 | GorgonMeducer |
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
| 基于 QEMU 的 ARM-2D 仿真工程 | [liuduanfei](https://github.com/liuduanfei) |
| 捐赠：GD32E103TB 芯片2片 | 信息牛(微信名) |
| Rt-thread 支持包、适配层、模块| [Meco Jianting Man](https://github.com/mysterywolf) |
| 捐赠：移远 EC600S-CN 4G模块 | 移远模块 |
| 捐赠：博流 BL706 开发板|博流智能 [bouffalolab](https://github.com/bouffalolab)|
| 捐赠：中国移动 CM32M101A 开发板| 孟巍(微信名) |
| 捐赠：APM32F030R8 开发板 | 极海半导体 陈成 |
| 捐赠：APM32E103VB 开发板 | 极海半导体 陈成 |
| 捐赠：APEX-Link 仿真器 | 极海半导体 陈成 |
| 源码格式化，增加 git 属性文件 | [Meco Jianting Man](https://github.com/mysterywolf) |
| demo/simulation-keil | 千帆(微信名) |
| demo/stm32f103zet6/demo01-led-stm32f103zet6 | [甜航](https://github.com/easyzoom) |
| demo/stm32f103zet6/demo02-led-stm32f103zet6_tworoot | [甜航](https://github.com/easyzoom) |
| demo/stm32f407zgt/demo01-led-stm32f407vgt | [甜航](https://github.com/easyzoom) |

# 6.内核测试与开发

## linux 下测试 pikascript 内核(推荐ubuntu20.04)：

step1: 拉取项目
``` shell
git clone https://github.com/pikastech/pikascript
cd pikascript/port/linux
```

step2: 编译项目
```
sh install_dependency.sh # 安装依赖，第一次用时运行
sh init.sh # 第一次编译前运行，之后就不用运行了 
sh make.sh # 编译项目
```

step3: 运行单元测试
``` shell	
sh gtest.sh
```

step4: 运行基准测试
``` shell
sh ci_benchmark.sh
```

# 7.Demo展示

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

```
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
pin = STM32G0.GPIO()
pin.init()
pin.setPin('PA0')
pin.setMode('in')
pin.setPull('down')
pin.enable()
pin.setPin('PA15')
pin.setMode('in')
pin.setPull('up')
pin.enable()
pin.setPin('PC13')
pin.enable()
pin.setPin('PB6')
pin.enable()
remove('pin')
ll = STM32G0.lowLevel()
oled = PikaPiZero.OLED()
oled.init()
snake = PikaPiZero.Point()
snake.x = 7
snake.y = 4
snake_lengh = 0
while snake_lengh < 3:
    body = snake
    i = 0
    while i < snake_lengh:
        body = body.next
        i = i + 1
    body.next = PikaPiZero.Point()
    body.next.x = body.x - 1
    body.next.y = body.y
    body.next.prev = body
    snake_lengh = snake_lengh + 1
fruit = PikaPiZero.Point()
fruit.x = 13
fruit.y = 2
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()
direction = 0
isUpdate = 1
while True:
    if isUpdate:
        isUpdate = 0
        if fruit.x == snake.x:
            if fruit.y == snake.y:
                body = snake
                i = 0
                while i < snake_lengh:
                    body = body.next
                    i = i + 1
                body.next = PikaPiZero.Point()
                body.next.prev = body
                snake_lengh = snake_lengh + 1
                fruit.x = fruit.x + 3
                if fruit.x > 15:
                    fruit.x = fruit.x - 15
                fruit.y = fruit.y + 3
                if fruit.y > 7:
                    fruit.y = fruit.y - 7
        body = snake
        i = 0
        while i < snake_lengh:
            body = body.next
            i = i + 1
        i = 0
        while i < snake_lengh:
            body = body.prev
            body.next.x = body.x
            body.next.y = body.y
            i = i + 1
        if direction == 0:
            snake.x = snake.x + 1
            if snake.x > 15:
                snake.x = 0
        if direction == 1:
            snake.x = snake.x - 1
            if snake.x < 0:
                snake.x = 15
        if direction == 2:
            snake.y = snake.y - 1
            if snake.y < 0:
                snake.y = 7
        if direction == 3:
            snake.y = snake.y + 1
            if snake.y > 7:
                snake.y = 0
        body = snake
        i = 0
        oled.clear()
        oled.drawPoint(fruit.x, fruit.y)
        while i < snake_lengh:
            oled.drawPoint(body.x, body.y)
            body = body.next
            i = i + 1
        oled.refresh()
    if ll.readPin('PA0') == 1:
        direction = 0
        isUpdate = 1
    if ll.readPin('PC13') == 0:
        direction = 1
        isUpdate = 1
    if ll.readPin('PA15') == 0:
        direction = 2
        isUpdate = 1
    if ll.readPin('PB6') == 0:
        direction = 3
        isUpdate = 1

```

![输入图片说明](document/image/imagefwef.png)

这几个 Demo 占用的 RAM 最大值只有3.56K，把1K的堆栈也算上就是4.56K，Flash 最大占用是30.4K，以 STM32F103C8T6 的 20K RAM 和 64K Flash 为标准，RAM 才用掉不到25%，Flash 才用掉不到50%。

我们可以简单对比一下 micropython 的常用芯片 STM32F405RG 和这次跑 PikaScript 的芯片STM32G070CB

## RAM资源对比
![image](https://user-images.githubusercontent.com/88232613/132944731-a55ece1d-061f-4b91-ba87-bd6547be96a7.png)

## Flash资源对比
![image](https://user-images.githubusercontent.com/88232613/132944745-e9cf598d-e75f-40bb-873e-911819d535b7.png)

## 参考价对比(以2021年9月11日立创商城10片售价为参考）
![image](https://user-images.githubusercontent.com/88232613/132944757-2b5cfda8-f93f-4456-8d7f-4e4767954056.png)

## 拓展能力如何呢？

除了设备驱动之外，为 mcu 开发自定义的 python 脚本绑定在 pikascript 的开发框架下非常轻松，下面两个 Demo 就是自定义的C模块拓展，这个 Demo 基于 ARM-2D 图像驱动库开发了一些 python 脚本接口。

## 几个小方块~
![Hnet-image (7)](https://user-images.githubusercontent.com/88232613/132945282-bfd310df-8063-456d-b90c-6b798a2c8ed5.gif)

## 几个旋转太阳~
![Hnet-image (6)](https://user-images.githubusercontent.com/88232613/132945107-e473a2cc-9fbc-47f9-aaed-a28d3ad1048c.gif)

