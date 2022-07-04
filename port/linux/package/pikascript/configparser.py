from PikaStdData import String
from PikaStdLib import MemChecker


class ConfigParser():
    content = String('')
    config_dict = {}

    def read(self, file_name):
        pass

    def read_string(self, content):
        self.content = String(content)
        self._parse()

    def _parse(self):
        content = self.content
        lines = content.split('\n')
        for line in lines:
            line = String(line)
            if line.startwith('#'):
                continue
            if line.startwith('['):
                section = String(line.replace('[', ''))
                section = section.replace(']', '')
                self.config_dict[section] = {}
                continue

            if line.strip() == '':
                continue

            stmt = line.split('=')
            key = String(stmt[0])
            value = String(stmt[1])
            key = key.strip()
            value = value.strip()
            section_dict = self.config_dict[section]
            section_dict[key] = value
        print(self.config_dict)
        mem = MemChecker()
        mem.now()

    def sections(self):
        pass

    def options(self):
        str = String(self.content)
        print(str.split('\n'))

    def get(self, section, option):
        pass

    def set(self, section, option, value):
        pass

    def items(self):
        pass
