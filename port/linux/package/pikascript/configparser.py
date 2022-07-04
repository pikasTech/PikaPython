from PikaStdData import String


class ConfigParser():
    content = String('')
    config_dict = {}

    def read(self, file_name):
        print('Error: read() method not implemented')
        raise
        self._parse()

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
    def __set__(self, __key, __val):
        self.config_dict[__key] = __val

    # support val = config[key]
    def __get__(self, __key):
        return self.config_dict[__key]

    def items(self, section):
        section_dict = self.config_dict[section]
        section_keys = section_dict.keys()
        items = []
        for key in section_keys:
            val = section_dict[key]
            items.append([key, val])
        return items
