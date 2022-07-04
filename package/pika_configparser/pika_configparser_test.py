import pika_configparser as configparser
from PikaStdLib import MemChecker
config = configparser.ConfigParser()

config_string = '[DEFAULT]\n\
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
ForwardX11 = no'

config.read_string(config_string)

print('\n========== config_string ==========')
print(config_string)

print('\n===== config.sections() =====')
print(config.sections())

print('\n===== config.options("DEFAULT") =====')
print(config.options('DEFAULT'))
mem = MemChecker()

config.set('bitbucket.org', 'User', 'hhdd123')

print('\n========= config["bitbucket.org"] bitbucket.org ===========') 
print(config['bitbucket.org'])

section = config['bitbucket.org']
section['User'] = '3833qwe'

print('\n========= config["bitbucket.org"] ===========')
print(config['bitbucket.org'])

print('\n========= config.items("DEFAULT") ===========')
print(config.items('DEFAULT'))

print('\n=============== all ================')
print(config)

print("mem used now: %0.2f kB" % mem.getNow())
print("mem used max: %0.2f kB" % mem.getMax())
