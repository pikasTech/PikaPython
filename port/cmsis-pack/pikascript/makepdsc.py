import os

version = "1.10.3"
version_lvgl = "0.2.0"
date = "2022-08-26"


class Group:
    path: str
    files: list
    subfix = ".c"
    name: str

    def __init__(self, name: str, path: str, subfix: str, format='<file attr="config" category="sourceC"  name="%s" />', dvd="/"):
        self.path = path
        self.files = []
        self.subfix = subfix
        self.format = format
        self.name = name
        self.dvd = dvd
        self.update()

    def setformat(self, format: str):
        self.format = format

    def update(self):
        files = os.listdir(self.path)
        self.files = [file for file in files if file.endswith(self.subfix)]

    def __str__(self):
        res = ""
        for file in self.files:
            res += self.format % (self.path + self.dvd + file) + '\n'
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
lvgl_c = Group("libC", "pikascript-lib/pika_lvgl", ".c", format=formatc)
lvgl_h = Group("libH", "pikascript-lib/pika_lvgl", ".h", format=formath)

api_c = Group("apiC", "pikascript-api", ".c",   format=formatc)
api_h = Group("apiH", "pikascript-api", ".h", format=formath)

api_bat = Group("apiBat", "pikascript-api", "", format='@del "%s"', dvd="\\")

collect("kernal", [kernal_c, kernal_h, kernal_cfg])
collect("lib", [lib_c, lib_h, api_c, api_h])
collect('lib_lvgl', [lvgl_c, lvgl_h])
collect("clean", [api_bat], subfix=".bat")

pdsc_xml_str = open('PikaTech.PikaScript.xml', 'r').read()
kernal_xml_str = open('kernal.xml', 'r').read()
lib_xml_str = open('lib.xml', 'r').read()
lib_lvgl_xml_str = open('lib_lvgl.xml', 'r').read()

pdsc_gen_str = pdsc_xml_str\
    .replace('@LVGL_LIB', lib_lvgl_xml_str)\
    .replace('@LVGL_VERSION', version_lvgl)\
    .replace('@KERNAL', kernal_xml_str)\
    .replace('@LIB', lib_xml_str)\
    .replace('@VERSION', version)\
    .replace('@DATE', date)\

with open('PikaTech.PikaScript.pdsc', 'w') as f:
    f.write(pdsc_gen_str)

# rm kernal.xml
# rm lib.xml
os.remove('kernal.xml')
os.remove('lib.xml')
os.remove('lib_lvgl.xml')
