"""Small deterministic gettext-style translation API."""


class Translator:
    def __init__(self, translations=None, locale='C', domain='messages'):
        self.locale = locale
        self.domain = domain
        self.translations = translations if translations is not None else {}

    def gettext(self, message):
        translated = self.translations.get(message)
        if translated is None:
            return message
        return translated

    def ngettext(self, singular, plural, count):
        if count == 1:
            return self.gettext(singular)
        return self.gettext(plural)

    def set_locale(self, locale):
        self.locale = locale
        return locale

    def add(self, message, translated):
        self.translations[message] = translated


_default = Translator()


def gettext(message):
    return _default.gettext(message)


def ngettext(singular, plural, count):
    return _default.ngettext(singular, plural, count)


def _(message):
    return gettext(message)


def install(translations=None, locale='C', domain='messages'):
    global _default
    _default = Translator(translations, locale, domain)
    return _


def set_locale(locale):
    return _default.set_locale(locale)


def add(message, translated):
    _default.add(message, translated)
