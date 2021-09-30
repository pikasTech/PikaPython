cmake_minimum_required(VERSION 3.0.0)
project(pikascript VERSION 0.1.0)

include_directories(coreDevelopment/package/pikascript/pikascript-core)
include_directories(coreDevelopment/package/pikascript/pikascript-api)
include_directories(coreDevelopment/boot/demo01-led)

add_subdirectory(coreDevelopment/package/googletest)
add_subdirectory(coreDevelopment/package/pikascript)
add_subdirectory(coreDevelopment/test)
add_subdirectory(coreDevelopment/boot/demo01-led)
add_subdirectory(coreDevelopment/boot/demo02-add)
add_subdirectory(coreDevelopment/boot/demo03-usart)
add_subdirectory(coreDevelopment/boot/demo04-usarts)
add_subdirectory(coreDevelopment/boot/demo05-sysobject)
add_subdirectory(coreDevelopment/boot/demo06-pikamain)
add_subdirectory(coreDevelopment/boot/banchmark)

