import os
p = os.path
assert p.join('dir', 'file.txt') == 'dir/file.txt'
assert p.join('/home/user', 'dir', 'file.txt') == '/home/user/dir/file.txt'

# Test split method
assert p.split('dir/file.txt')[0] == 'dir'
assert p.split('dir/file.txt')[1] == 'file.txt'
assert p.split('/home/user/dir/file.txt')[0] == '/home/user/dir'
assert p.split('/home/user/dir/file.txt')[1] == 'file.txt'
    
# Test splitext method
assert p.splitext('file.txt')[0] == 'file'
assert p.splitext('file.txt')[1] == '.txt'
assert p.splitext('/home/user/file.tar.gz')[0] == '/home/user/file.tar'
assert p.splitext('/home/user/file.tar.gz')[1] == '.gz'

# Test basename method
assert p.basename('dir/file.txt') == 'file.txt'
assert p.basename('/home/user/dir/file.txt') == 'file.txt'

# Test dirname method
assert p.dirname('dir/file.txt') == 'dir'
assert p.dirname('/home/user/dir/file.txt') == '/home/user/dir'

# Test exists method
assert p.exists('config/pika_config_void') == False
assert p.exists('/root/go') == True

# Test isdir method
assert p.isdir('config/pika_config_void.h') == False
assert p.isdir('config') == True

# Test isfile method
assert p.isfile('config') == False
assert p.isfile('config/pika_config_void.h') == True

# Test isabs method
assert p.isabs('dir/file.txt') == False
assert p.isabs('/home/user/file.txt') == True

# Test abspath method
assert p.abspath('config/pika_config_void.h') == "/root/pikascript/port/linux/config/pika_config_void.h"
assert p.abspath('/root/go') == "/root/go"

print("PASS")
