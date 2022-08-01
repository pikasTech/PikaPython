import os

# 函数功能：提取指定目录下的所有.c 文件
# dir:str  : 字符串类型，指定的路径
# gn_fd:os : BUILD.gn 文件的文件描述符
# 返回值：none
def extract_c_files_name(dir:str, gn_fd:os)->None:
  file_list = os.listdir(dir)
  for files in file_list:
    if ('.c' == os.path.splitext(files)[1]):
      print(files)
      gn_fd.write("""    \"""" + dir + files + "\",\r\n")

# 指定当前BULID.gn 文件的路径 
gn_file_path = "/home/wait/project/xr806/xr806_openharmony/device/xradio/xr806/ohosdemo/pikascript_test_prj/"
# make sure the current path is prj dir
if( os.getcwd() != gn_file_path):
  os.chdir(gn_file_path)

print("gn_file_path: ",gn_file_path)
# 前面只是给定了文件路径，这里要把字符串 "BUILD.gn" 也加进来
gn_file_path += "BUILD.gn"
# gn_file_path = gn_file_path + "/BUILD.gn"

#通过写方式打开，且每次会覆盖，这样每次运行脚本写进去的开头部分不会重复
gn_file = open(gn_file_path, 'w+') 

#这里面 main.c 的位置根据需要调整
gn_content =  """\
import("//device/xradio/xr806/liteos_m/config.gni")\r\n
static_library(\"app_pikascript\") { \r\n
  configs = []\r\n
  sources = [
    "main.c",
"""
gn_file.write(gn_content)

src_dir = ["pikascript/pikascript-api/", "pikascript/pikascript-core/", "pikascript/pikascript-lib/PikaStdLib/"]
for dir in src_dir:
  print("**********" + dir + "**********")
  extract_c_files_name(dir,gn_file)

gn_file.write("""  ]\r\n\r\ncflags = board_cflags

  include_dirs = board_include_dirs
  include_dirs += [
      "//kernel/liteos_m/kernel/arch/include",
      "pikascript/pikascript-api/",
      "pikascript/pikascript-core/",
      "pikascript/pikascript-lib/PikaStdLib/"
  ]
}""")

gn_file.close()


# file_list = []
# source_file_f = os.listdir("pikascript-api/")
# print("pikascript-api .c files")
# i = 0
# #source_file_fd = open(source_file_path,"r") #only read
# for files in source_file_f:
#   if (".c" == os.path.splitext(files)[1]):
#     file_list.append(files)
#     print(file_list[i])
#     gn_file.write("""    \"""" + "pikascript-api/" + file_list[i] + "\",\r\n")
#     i += 1

# file_list.clear()
