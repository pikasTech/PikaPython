## 开发手册
[PikaScript-三分钟快速上手](doc/1.%E4%B8%89%E5%88%86%E9%92%9F%E5%BF%AB%E9%80%9F%E4%B8%8A%E6%89%8B.md)

## stm32例程
[pikascript-stm32-demo](../../../pikascript-demo-stm32)

# 1.简介
pikascript可以为mcu裸机c开发提供面向对象的脚本绑定。

支持裸机运行，可运行于内存8Kb以上的mcu中，如stm32f103c8t6，esp32。

Pika预编译器

[pikascript-compiler-Rust](https://github.com/mimilib/pikascript-compiler-rust)

支持跨平台，可在linux环境开发、测试内核。

开箱即用，零配置，仅使用C标准库，几乎不使用宏，几乎不使用全局变量。

完整的单元测试。

栈空间少量占用，内存信息可观测。

尽可能的结构清晰（尽我所能）。

# 2.stm32教程视频

https://www.bilibili.com/video/BV1mg411L72e

# 3.原理解析：

## 架构示意图
![K9C)%CZO)X`8VFBKCE8(9@W](https://user-images.githubusercontent.com/88232613/127806449-b476b2fd-9f40-4c53-94a0-e1e965c046c3.png)

## 内核源码

https://github.com/mimilib/pikascript-core

## 解析文章
[单片机运行python脚本，不移植micropython也行吗？](https://mp.weixin.qq.com/s?__biz=MzU4NzUzMDc1OA==&mid=2247484127&idx=1&sn=f66cff49c488e48c52570c7bb570328f&chksm=fdebd5b6ca9c5ca0707fd221c32f3ad63e94aeb6f917a92774b89ea042381ea261990f5cca3c&token=2045971639&lang=zh_CN#rd)

## 微信交流群
![微信图片_20210825171046](https://user-images.githubusercontent.com/88232613/130763024-c57106f5-0d46-43d8-99e3-c331ae2594b5.jpg)

## 官方公众号
![image](https://user-images.githubusercontent.com/88232613/128301451-f0cdecea-6457-4925-b084-42e7796a856e.png)

# 4.内核测试与开发

## windows下测试PikaScript内核：

step1: 下载源码
``` 
https://github.com/mimilib/PikaScript/releases/download/v1.0/PikaScript-v1.0.zip
```
step2: 下载安装codeblocks
```
https://download.fosshub.com/Protected/expiretime=1628119786;badurl=aHR0cHM6Ly93d3cuZm9zc2h1Yi5jb20vQ29kZS1CbG9ja3MuaHRtbA==/9bc1258414546cedb7b0774846ec773613a89ed715964c9a3239fab0465f014d/5b85805cf9ee5a5c3e979f1b/5e80624f7d74bb810359a46c/codeblocks-20.03-setup.exe
```

step3: 打开winproject文件夹下的demoXX/demoXX.cbs编译运行


## linux下测试pikascript内核(推荐ubuntu20.04)：

step1: 拉取项目
``` shell
git clone https://github.com/mimilib/pikascript
cd PikaScript
git submodule init && git submodule update
```
step2: 安装编译链
``` shell
sudo apt update
sudo apt install -y clang llvm cmake zlib1g-dev
```

step3: 编译项目
```
cd mimiproject
sh init.sh # 第一次编译前运行，之后就不用运行了 
sh make.sh # 编译项目
```

step4: 运行demo
``` shell	
bin/demo01-led.bin
```

