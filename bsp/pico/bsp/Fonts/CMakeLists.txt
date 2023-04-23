# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_Fonts_SRCS 变量
aux_source_directory(. DIR_Fonts_SRCS)

# 生成链接库
add_library(Fonts ${DIR_Fonts_SRCS})