import configparser

fd = open('test/assets/widget_config.ini', 'r')
s = fd.read(-1)
print(s)
config = configparser.ConfigParser()
config.read_string(s)
print(config)
font_color = config.get('文本控件_20030101', 'font_color')
type(font_color)
print(font_color)
fd.close()
