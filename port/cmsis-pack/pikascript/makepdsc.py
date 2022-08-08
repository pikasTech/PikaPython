import os
from sys import prefix
from xmlrpc.server import list_public_methods


class Group:
    path: str
    files: list
    subfix = ".c"
    name: str

    def __init__(self, name: str, path: str, subfix: str, format='<file attr="config" category="sourceC"  name="%s" />'):
        self.path = path
        self.files = []
        self.subfix = subfix
        self.format = format
        self.name = name
        self.update()

    def setformat(self, format: str):
        self.format = format

    def update(self):
        files = os.listdir(self.path)
        self.files = [file for file in files if file.endswith(self.subfix)]

    def __str__(self):
        res = ""
        for file in self.files:
            res += self.format % (self.path + '/' + file) + '\n'
        return res

    def write(self):
        with open(self.name + ".xml", 'w') as f:
            f.write(str(self))


def collect(name, groupList: list[Group], subfix=".xml"):
    with open(name + subfix, 'w') as f:
        for group in groupList:
            print(group)
            f.write(str(group))


formatc = '<file attr="config" category="sourceC"  name="%s" />'
formath = '<file attr="config" category="header"  name="%s" />'
formatcfg = '<file attr="config" category="source"  name="%s" />'

kernal_c = Group("kernalH", "pikascript-core", ".c")
kernal_h = Group("kernalC", "pikascript-core", ".h",
                 format=formath)
kernal_cfg = Group("kernalCfg", "pikascript-core", ".cfg",
                   format=formatcfg)
lib_c = Group("libC", "pikascript-lib/PikaStdLib", ".c", format=formatc)
lib_h = Group("libH", "pikascript-lib/PikaStdLib", ".h", format=formath)

api_c = Group("apiC", "pikascript-api", ".c",   format=formatc)
api_h = Group("apiH", "pikascript-api", ".h", format=formath)

api_bat = Group("apiBat", "pikascript-api", "", format='@del %s')

collect("kernal", [kernal_c, kernal_h, kernal_cfg])
collect("lib", [lib_c, lib_h, api_c, api_h])
collect("clean", [api_bat], subfix=".bat")
