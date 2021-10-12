cmake_minimum_required(VERSION 3.0.0)
project(pikascript VERSION 0.1.0)

include_directories(package/pikascript/pikascript-core)
include_directories(package/pikascript/pikascript-api)
include_directories(package/pikascript/pikascript-lib/PikaRunExternContral)
include_directories(boot/demo01-led)

add_subdirectory(package/googletest)
add_subdirectory(package/pikascript)
add_subdirectory(test)
add_subdirectory(boot/demo01-led)
add_subdirectory(boot/demo02-add)
add_subdirectory(boot/demo03-usart)
add_subdirectory(boot/demo04-usarts)
add_subdirectory(boot/demo05-sysobject)
add_subdirectory(boot/demo06-pikamain)
add_subdirectory(boot/banchmark)

