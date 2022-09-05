
class ConfigParser():
    content = ''
    config_dict = {}

    def _parse(self):
        self.content = self.content.replace('\r', '')
        content = self.content
        lines = content.split('\n')
        for line in lines:
            if line.startswith('#'):
                continue
            if line.startswith(';'):
                continue
            if line.startswith('['):
                section = line.replace('[', '')
                section = section.replace(']', '')
                self.config_dict[section] = {}
                continue

            if line.strip() == '':
                continue

            stmt = line.split('=')
            key = stmt[0].strip()
            value = _getvalue(stmt).strip()
            section_dict = self.config_dict[section]
            section_dict[key] = value

    def sections(self):
        section_keys = self.config_dict.keys()
        sections = []
        for section_item in section_keys:
            sections.append(section_item)
        return sections

    def options(self, section):
        section_dict = self.config_dict[section]
        option_keys = section_dict.keys()
        options = []
        for option_item in option_keys:
            options.append(option_item)
        return options

    def get(self, section, option):
        section_dict = self.config_dict[section]
        return section_dict[option]

    def set(self, section, option, value):
        section_dict = self.config_dict[section]
        section_dict[option] = value

    # support config[key] = val
    def __setitem__(self, __key, __val):
        self.config_dict[__key] = __val

    # support val = config[key]
    def __getitem__(self, __key):
        return self.config_dict[__key]

    def items(self, section):
        section_dict = self.config_dict[section]
        section_keys = section_dict.keys()
        items = []
        for key in section_keys:
            val = section_dict[key]
            items.append([key, val])
        return items

    def __str__(self):
        content = ''
        section_keys = self.config_dict.keys()
        for section_item in section_keys:
            content += '[' + section_item + ']\n'
            section_dict = self.config_dict[section_item]
            section_keys = section_dict.keys()
            for key in section_keys:
                val = section_dict[key]
                content += key + ' = ' + val + '\n'
            content += '\n'
        return content

    def write(self, file_name):
        print('Error: write() method not implemented')
        raise
        self.content = self.__str__(self)
        print(self.content)

    def read_string(self, content):
        self.content = content
        self._parse()

    def read(self, file_name):
        print('Error: read() method not implemented')
        raise
        content = ''
        self.content = content
        self._parse()

def _getvalue(stmt):
    index = 0
    val = ''
    for item in stmt:
        if index > 0:
            if val != '':
                val += ('=' + item)
            else:
                val += item
        index = index + 1
    return val
