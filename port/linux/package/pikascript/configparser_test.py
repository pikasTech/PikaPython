import configparser
from PikaStdLib import MemChecker
config = configparser.ConfigParser()
config.read_string('\
[DEFAULT]\n\
ServerAliveInterval = 45\n\
Compression = yes\n\
CompressionLevel = 9\n\
ForwardX11 = yes\n\
\n\
[bitbucket.org]\n\
User = hg\n\
\n\
[topsecret.server.com]\n\
Port = 50022\n\
ForwardX11 = no')

print(config.sections())
print(config.options('DEFAULT'))
mem = MemChecker()

config.set('bitbucket.org', 'User', 'hhdd123')
print(config['bitbucket.org'])

section = config['bitbucket.org']
section['User'] = '3833qwe'
print(config['bitbucket.org'])

print(config.items('DEFAULT'))

print("mem used now: %0.2f kB" % mem.getNow())
print("mem used max: %0.2f kB" % mem.getMax())
