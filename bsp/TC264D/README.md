TC264
bsp使用Aurix Development studio  IDE
导入后工程名为 UART_Demo2 ，我试过把工程名改为 TC264D_Pika 但是编译失败emm

串口初始化部分采用逐飞的串口例程，所以工程内包含逐飞的库

内核与库采用lts2021
2022-02-06测试编译（魔改内核）通过，运行通过

--------------------
魔改内核（不魔改编译不过）
<br>
编译器采用默认配置 #define PIKA_WEAK  
<img src=".\README\1.png">
<br>
重写 __platform_getchar() 函数（我在 Cpu0_Main.c 中重写的，重定向到串口），并把PikaPlatform.c中的此函数注释掉
<br>
PikaPlatform.c
<img src=".\README\2.png">
<br><br>
Cpu0_Main.c
<img src=".\README\3.png">
