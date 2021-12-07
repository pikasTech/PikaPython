# RT-Thread building script for bridge

import os, shutil
import subprocess
from building import *


def pikascript_init():
    cwd = GetCurrentDir()
    def mycopyfile(srcfile,dstfile):
        if not os.path.isfile(srcfile):
            pass
        else:
            fpath,fname=os.path.split(dstfile) 
            if not os.path.exists(fpath):
                os.makedirs(fpath)             
            shutil.copyfile(srcfile,dstfile)   

    mycopyfile(cwd + '/port/rt-thread/main.py', cwd + '/main.py')
    mycopyfile(cwd + '/port/rt-thread/pikaPackage.exe', cwd + '/pikaPackage.exe')
    mycopyfile(cwd + '/port/rt-thread/requestment.txt', cwd + '/requestment.txt')

    def myremove(path):
        all_path = cwd + '\\' + path
        os.popen('del ' + all_path + ' /F /Q /S')
        os.popen('rd ' + all_path + ' /Q /S')

    # myremove('bsp')
    # myremove('document')
    # myremove('examples')
    # myremove('package')
    # myremove('port')
    # myremove('src')
    # myremove('tools')
    # myremove('.github')
    # myremove('.git')
    # myremove('.vscode')
    # myremove('.gitattributes')
    # myremove('.gitignore')
    # myremove('logo.txt')
    # myremove('packages.toml')
    # myremove('README.md')
    # myremove('README_zh.md')

    os.system(cwd + '/' + 'pikaPackage.exe')
    os.system(cwd + '/' + 'rust-msc-latest-win10.exe')

# init pikascript
pikascript_init()

# init argument
cwd = GetCurrentDir()
group = []
src = []
inc = []

# add file and path
for root, dirs, files in os.walk(cwd):
    for dir in dirs:
        src = src + Glob(os.path.join(root,dir,'*.c'))
        inc = inc + [os.path.join(root,dir)]

group = DefineGroup('pikascript', src, depend = ['PKG_USING_PIKASCRIPT'], CPPPATH = inc)

Return('group')

