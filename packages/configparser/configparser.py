def _strip(value):
    start = 0
    end = len(value)
    while start < end:
        character = value[start]
        if character != " " and character != "\t" and character != "\r":
            break
        start += 1
    while end > start:
        character = value[end - 1]
        if character != " " and character != "\t" and character != "\r":
            break
        end -= 1
    return value[start:end]


def _option_name(value):
    value = value + ""
    uppercase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    lowercase = "abcdefghijklmnopqrstuvwxyz"
    result = ""
    for character in value:
        replacement = character
        index = 0
        while index < len(uppercase):
            if character == uppercase[index]:
                replacement = lowercase[index]
                break
            index += 1
        result += replacement
    return result


class _SectionProxy:
    def __init__(self, parser, section):
        self._parser = parser
        self._section = section

    def __getitem__(self, option):
        return self._parser.get(self._section, option)

    def __setitem__(self, option, value):
        self._parser.set(self._section, option, value)

    def get(self, option, fallback=None):
        if self._parser.has_option(self._section, option):
            return self._parser.get(self._section, option)
        return fallback

    def keys(self):
        return self._parser.options(self._section)


def _lines(content):
    result = []
    line = ""
    for character in content:
        if character == "\n":
            result.append(line)
            line = ""
        elif character != "\r":
            line += character
    result.append(line)
    return result


def _separator(line):
    index = 0
    while index < len(line):
        character = line[index]
        if character == "=" or character == ":":
            return index
        index += 1
    return -1


class ConfigParser:
    def __init__(self):
        self._sections = {}
        self._section_order = []
        self._option_order = {}

    def read_string(self, content):
        if content is None:
            content = ""
        else:
            content = content + ""
        new_sections = {}
        new_section_order = []
        new_option_order = {}
        current_section = None
        for raw_line in _lines(content):
            line = _strip(raw_line)
            if line == "":
                continue
            first = line[0]
            if first == "#" or first == ";":
                continue
            if first == "[":
                if len(line) < 3 or line[len(line) - 1] != "]":
                    raise ValueError("invalid section header")
                section = _strip(line[1:len(line) - 1])
                if section == "":
                    raise ValueError("section name is empty")
                if section in new_sections:
                    raise ValueError("duplicate section")
                new_sections[section] = {}
                new_section_order.append(section)
                new_option_order[section] = []
                current_section = section
                continue
            if current_section is None:
                raise ValueError("option has no section")
            separator = _separator(line)
            if separator < 1:
                raise ValueError("invalid option")
            option = _option_name(_strip(line[0:separator]))
            if option == "":
                raise ValueError("option name is empty")
            value = _strip(line[separator + 1:len(line)])
            section_data = new_sections[current_section]
            if option in section_data:
                raise ValueError("duplicate option")
            section_data[option] = value
            option_names = new_option_order[current_section]
            option_names.append(option)
        self._sections = new_sections
        self._section_order = new_section_order
        self._option_order = new_option_order
        return None

    def sections(self):
        result = []
        for section in self._section_order:
            result.append(section)
        return result

    def optionxform(self, option):
        return _option_name(option)

    def has_section(self, section):
        return section in self._sections

    def has_option(self, section, option):
        sections = self._sections
        if section not in sections:
            return False
        return self.optionxform(option) in sections[section]

    def options(self, section):
        option_order = self._option_order
        if section not in option_order:
            raise ValueError("section does not exist")
        result = []
        for option in option_order[section]:
            result.append(option)
        return result

    def get(self, section, option):
        sections = self._sections
        if section not in sections:
            raise ValueError("section does not exist")
        section_data = sections[section]
        name = self.optionxform(option)
        if name not in section_data:
            raise ValueError("option does not exist")
        return section_data[name]

    def set(self, section, option, value):
        value = value + ""
        sections = self._sections
        if section not in sections:
            raise ValueError("section does not exist")
        name = self.optionxform(option)
        section_data = sections[section]
        if name not in section_data:
            option_order = self._option_order
            names = option_order[section]
            names.append(name)
        section_data[name] = value
        return None

    def remove_option(self, section, option):
        sections = self._sections
        if section not in sections:
            raise ValueError("section does not exist")
        name = self.optionxform(option)
        section_data = sections[section]
        if name not in section_data:
            return False
        del section_data[name]
        names = self._option_order[section]
        index = 0
        while index < len(names):
            if names[index] == name:
                names.pop(index)
                break
            index += 1
        return True

    def remove_section(self, section):
        sections = self._sections
        if section not in sections:
            return False
        del sections[section]
        del self._option_order[section]
        names = self._section_order
        index = 0
        while index < len(names):
            if names[index] == section:
                names.pop(index)
                break
            index += 1
        return True

    def items(self, section):
        sections = self._sections
        option_order = self._option_order
        if section not in sections:
            raise ValueError("section does not exist")
        result = []
        section_data = sections[section]
        for option in option_order[section]:
            result.append((option, section_data[option]))
        return result

    def __getitem__(self, section):
        sections = self._sections
        if section not in sections:
            raise ValueError("section does not exist")
        return _SectionProxy(self, section)

    def __setitem__(self, section, values):
        section = section + ""
        section_data = {}
        option_names = []
        for option in values:
            name = self.optionxform(option)
            if name in section_data:
                raise ValueError("duplicate option")
            value = values[option] + ""
            section_data[name] = value
            option_names.append(name)
        sections = self._sections
        option_order = self._option_order
        if section not in sections:
            section_order = self._section_order
            section_order.append(section)
        sections[section] = section_data
        option_order[section] = option_names

    def __str__(self):
        result = ""
        sections = self._sections
        option_order = self._option_order
        for section in self._section_order:
            result += "[" + section + "]\n"
            section_data = sections[section]
            for option in option_order[section]:
                result += option + " = " + section_data[option] + "\n"
            result += "\n"
        return result

    def read(self, file_name):
        raise RuntimeError("configparser file provider is unavailable")

    def write(self, file_object):
        raise RuntimeError("configparser file provider is unavailable")
