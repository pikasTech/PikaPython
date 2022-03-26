# CH582-PIKA

本项目为CH582芯片移植了pikaScript

使用说明：

1. 本项目并未使用MRS IDE,而是选择了CMake+Ninja的编译结构,工具链已经默认配置了MRS的默认安装位置,用户还需要手动安装Ninja,当然如果你想用其他编译方式只需要改一下cmake生成的目标即可
2. 快捷编译脚本是`build.bat`和`build.sh`,分别对应Windows平台和Linux平台,因为沁恒并未公开提供MACOS的工具链,所以请使用Windows或者Linux。
3. 目前尚未实现shell和文件系统



工程结构：

```shell
├─APP				# 程序入口
├─BLE_LIB			# BLE库
├─HAL				# HAL库
├─Ld				# 链接脚本
├─pikascript		# pikascript源码
├─printf			# printf移植源码
├─RVMSIS			# core-riscv源码
├─Startup			# 启动文件
├─StdPeriphDriver	# 外设标准库
├─USB_LIB			# USB库
├─build.bat			# Windows一键编译脚本
├─build.sh			# Linux一键编译脚本
└─CMakeLists.txt	# cmake文件	
```

