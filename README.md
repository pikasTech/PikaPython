# 简介
mimiscript可以为mcu裸机c开发提供面向对象的脚本绑定。

支持裸机运行，可运行于内存40Kb以上的mcu中，如stm32f103，esp32。

支持跨平台，可运行于linux环境。

仅使用C标准库，尽可能的结构清晰（尽我所能），几乎不使用宏。

## 架构示意图
![K9C)%CZO)X`8VFBKCE8(9@W](https://user-images.githubusercontent.com/88232613/127806449-b476b2fd-9f40-4c53-94a0-e1e965c046c3.png)

## windows下运行demo：

step1: 下载源码
``` 
https://github.com/mimilib/mimiscript/releases/download/v1.0/mimiscript-v1.0.zip
```
step2: 下载安装codeblocks
```
https://download.fosshub.com/Protected/expiretime=1628119786;badurl=aHR0cHM6Ly93d3cuZm9zc2h1Yi5jb20vQ29kZS1CbG9ja3MuaHRtbA==/9bc1258414546cedb7b0774846ec773613a89ed715964c9a3239fab0465f014d/5b85805cf9ee5a5c3e979f1b/5e80624f7d74bb810359a46c/codeblocks-20.03-setup.exe
```

step3: 打开winproject文件夹下的demoXX/demoXX.cbs编译运行


## linux下运行demo(推荐ubuntu20.04)：

step1: 拉取项目
``` shell
git clone https://github.com/mimilib/mimiscript
cd mimiscript
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

## 测试例：
``` c
#include "sysObj.h"

/* 
    被绑定的方法 
    self 是对象指针，指向执行方法的对象
    args 是参数列表，用于传入传出参数
    （所有被绑定的方法均使用此形参）
*/
void add(MimiObj *self, Args *args) 
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
    MimiObj*是新建对象的指针
    （所有构造器均使用此形参）
*/
MimiObj *New_MimiObj_test(Args *args)
{
    /* 
        继承sys类
        只需要直接调用父类的构造器即可
    */
    MimiObj *self = New_MimiObj_sys(args);
    
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
    MimiObj *sys = newRootObj("sys", New_MimiObj_sys);

    /* 
        新建test对象
        test对象作为子对象挂载在sys对象下（对象树）
    */
    obj_newObj(sys, "test", New_MimiObj_test);
    
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
