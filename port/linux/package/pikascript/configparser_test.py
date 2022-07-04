import configparser
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
