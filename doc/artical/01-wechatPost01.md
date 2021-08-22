# 使用PikaScript为你的mcu轻松添加面向对象脚本支持

## abstrict

PikaScript是一个使用c语言写成的脚本支持库，可以非侵入地为mcu提供对象化的脚本支持。

PikaScript能够在完全不修改原本工程代码的情况下（非侵入性），将c语言的函数以及变量绑定到脚本对象上，可以直接使用脚本调用。

PikaScript默认支持python语法绑定，也可以支持typescript语法绑定。

## 1.introduction

在嵌入式开发的IOT、智能终端等应用场景中，脚本开发是一个方便快捷的解决方案。

说到嵌入式使用脚本语言开发，可能首先想到的就是micropython，micropython可以让工程师使用脚本语言python进行mcu开发，极大地降低了开发门槛。

但是使用micropython开发能够直接使用的开发板并不多，为没有现成micropython固件的mcu移植micropython显然也是一件工程浩大且门槛很高的工作。

python的运行效率较低，在资源紧缺的mcu中显得尤为明显，而且使用python开发难以充分利用mcu的中断、dmp等硬件特性。在高实时性的信号处理、数据采集、实时控制等应用中，python难以成为真正落地于生产环境。

就目前而言，在mcu开发中，占80%左右的开发仍然是使用c语言，c++也仅占不到20%。

但是无疑脚本语言的便利性是非常明显的。

在IOT领域中，服务器端的开发者往往熟悉python和JavaScript等支持面向对象的脚本语言，如果能够直接用脚本语言调用mcu的功能，那么也将达到降低开发难度的效果。

也就是说，使用c语言进行mcu嵌入式开发，又向上位机或者服务器提供面向对象的脚本语言调用接口，不就可以兼顾mcu运行效率和开发效率了吗？

本文介绍的PikaScript库正是可以起到这样的作用。

PikaScript库可以为c语言开发的mcu工程提供面向对象的脚本语言调用接口。PikaScript有以下几个特点：

1. 支持裸机运行，可运行于内存40Kb以上的mcu中，如stm32f103，esp32。

2. 支持跨平台，可运行于linux环境。

3. 仅使用C标准库，尽可能的结构清晰（尽我所能），几乎不使用宏。

## 2. 用PikaScript点一个灯

鲁迅曾说过：“点灯是嵌入式领域的hellow world”。

那我们就以点灯为例，看一看PikaScript如何为mcu提供对象化的脚本支持。

我们以STM32的HAL库为例，假设在管脚PA8上连接了一个LED灯，我们称之为LED1，PA8拉高时灯亮，拉低时灯灭。

那么使用下面的c语言代码就可以点亮灯LED1：
``` c
HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,SET);
```
但是我们希望使用如下的面向对象的脚本语言优雅地点灯：
``` python
LED1.on()
```

下面我们看看如何使用PikaScript实现这个需求。

1. 编写一个on()函数。这个函数将会被作为一个方法注册到脚本对象里面，注册之后就不会再由开发者在c语言开发中调用了，只会在脚本运行时被脚本解释器调用。

    on()函数的入口参数有self和args，其中self是对象的指针，args是参数列表，args内部基于链表，可以传入任意个数、任意类型的参数。
    
    在PikaScript中，所有被绑定为方法的函数都使用这两个入口参数。
``` c
void on(PikaObj *self, Args *args)
{
    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,SET);
}
```
2. 然后编写LED1对象的构造器。
 
   构造器的入口参数是args，用于传入初始化对象所需的参数，返回的是一个脚本对象，所有的脚本对象都使用PikaObj结构体。

   class_defineMethod用于将编写的c语言函数绑定为脚本对象的方法。

   class_defineMethod的第一个入口参数是对象指针，第二个参数是绑定的接口定义，第三个入口参数是被绑定函数的函数指针。
``` c
PikaObj * New_LED1(Args *args)
{
    // 继承自PikaObj基本类
    PikaObj *self = New_PikaObj(args);
    // 为LED1对象绑定on()方法
    class_defineMethod(self, "on()", on);
    return self;
}
```
3. 创建对象并监听串口的输入数据。当获得整行数据后直接当作脚本执行。
``` c
int uartReciveOk; //串口单行接收完成的标志位
char uartReciveBuff[256];// 串口接收到的单行数据
int main()
{
    // 硬件的初始化代码略

    // 创建根对象
    PikaObj *root = newRootObj("root",New_PikaObj_sys);
    // 新建LED1对象，LED1对象会挂载在root对象下
    obj_newObj(root, "LED1", New_LED1);
    while(1)
    {
        // 串口已经接收到单行数据
        if(uartReciveOk)
        {
            // 执行串口输入的单行数据
            obj_run(root, uartReciveBuff);
            // 清除串口接收标志位
            uartReciveOk = 0;
        }
    }
}

```

4. 这时只要向mcu的串口发送``` led.on()```，灯就亮了~

## 3. 用PikaScript实现一个加法函数。

上面的例子中的方法没有输入输出，下面的例子中，我们会定义一个test类，并为test类添加一个add函数，实现加法功能。

``` c
#include "sysObj.h"

/* 
    被绑定的方法 
    self 是对象指针，指向执行方法的对象
    args 是参数列表，用于传入传出参数
    （所有被绑定的方法均使用此形参）
*/
void add(PikaObj *self, Args *args) 
{
    /* 
        参数传递 
        从参数列表中取出输入参数val1和val2
    */
    int val1 = args_getInt(args, "val1");
    int val2 = args_getInt(args, "val2");
    
    /* 实现方法的功能 */
    int res = val1 + val2;
    
    /* 将返回值传回参数列表 */
    method_returnInt(args, res);
}

/* 
    定义测试类的构造器，一个构造器对应一个类
    通过构造器即可新建对象
    args是构造器的初始化参数列表
    PikaObj*是新建对象的指针
    （所有构造器均使用此形参）
*/
PikaObj *New_PikaObj_test(Args *args)
{
    /* 
        继承sys类
        只需要直接调用父类的构造器即可
    */
    PikaObj *self = New_PikaObj_sys(args);
    
    /* 
        为test类绑定一个方法（支持重载）
        1.入口参数self：对象指针，指向当前对象
        2.传入的第二参数是被绑定方法的接口定义
        （此处使用typescript语法，简单的修改即可支持python格式）
        3.传入的第三个参数是被绑定方法的函数指针
    */
    class_defineMethod(self, "add(val1:int, val2:int):int", add); 


    /* 返回对象 */
    return self;
}

void main()
{
    /* 
        新建根对象，对象名为“sys”
        传入对象名和构造器的函数指针
    */
    PikaObj *sys = newRootObj("sys", New_PikaObj_sys);

    /* 
        新建test对象
        test对象作为子对象挂载在sys对象下（对象树）
    */
    obj_newObj(sys, "test", New_PikaObj_test);
    
    /*  
        运行单行脚本。
        因为test对象挂在在sys对象下，
        因此可以通过test.add调用test对象的方法
        运行后会动态新建res属性，该属性属于sys对象
    */
    obj_run(sys, "res = test.add(val1 = 1, val2 = 2)");
    /*
        (也支持 "res = test.add(1, 2)"的调用方式)
    */
    
    /* 从sys对象中取出属性值res */
    int res = obj_getInt(sys, "res");
    
    /* 
        析构对象
        所有挂载在sys对象下的子对象都会被自动析构
        本例中挂载了test对象，因此在析构sys对象前，
        test对象会被自动析构
    */
    obj_deinit(sys);
    
    /* 打印返回值 res = 3*/
    printf("%d\r\n", res);    
}
```

# 结构摘要：

## 1.数据结构 mimidata
datamemory：内存管理

datalink：非侵入式双向链表

dataArg：数据容器，支持int、float、string、object类型和自定义类型

dataArgs：基于双向链表的数据列表，常用作动态参数列表，并用来存储对象的属性和方法

dataSting，字符串处理

## 2.对象支持 (PikaObject) 
dataObject：对象构造、对象析构、对象树、创建属性、绑定属性、绑定方法、python接口绑定、python接口解析

## 3.命令行交互层 (mimishell) 
mimiSH：用于调用python接口

## 4.基于发布-订阅模型的事件机制（mimiEvnet）
支持同步事件回调

## 5.单元测试 (mimitest) 
几乎完全覆盖的单元测试

