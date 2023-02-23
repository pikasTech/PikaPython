import weakref


class _ALIGN:
    CENTER = 0
    TOP_MID = 1


ALIGN = _ALIGN
_backend = None


def set_backend(backend):
    global _backend
    global ALIGN
    _backend = backend
    ALIGN = _backend.ALIGN


class Widget:
    backend = None
    width = 0
    height = 0
    pos = None
    parent = None
    align = None
    text = None
    isroot = False
    needbuild = True
    _label = None
    _child = []

    def __init__(self,
                 text=None,
                 width=100,
                 height=100,
                 pos=None,
                 align=ALIGN.TOP_MID):
        self.width = width
        self.height = height
        self.pos = pos
        self.align = align
        self.text = text

    def build(self):
        pass

    def _setPerent(self, parent):
        # use weakref to avoid circular reference
        self.parent = weakref.ref(parent)

    def update(self):
        if self.needbuild:
            built = self.build()
            if not built is None:
                self = self.add(built)
            self.needbuild = False

        if self.parent is None:
            print('self.parent is None')
            return

        if self.parent.backend is None:
            print('self.parent.backend is None')
            return

        if self.backend is None:
            self.backend = self._createBackend(self.parent)

        if not self.isroot:
            self._updateAlign(self.align)
            self._updateAttr(self.width, self.height, self.pos)
            self._updateText(self.text)

        for c in self._child:
            c.update()

    def _createBackend(self, parent: "Widget"):
        return _backend.widget(parent.backend)

    def _updateAttr(self,
                    width,
                    height,
                    pos):
        self.backend.set_width(width)
        self.backend.set_height(height)
        if not pos is None:
            self.backend.set_pos(pos[0], pos[1])

    def _updateAlign(self, align):
        self.backend.align(align, 0, 0)

    def _updateText(self, text):
        if not None is text:
            self._label = _backend.label(self.backend)
            self._label.set_text(self.text)
            self._label.align(_backend.ALIGN.CENTER, 0, 0)

    def add(self, *child):
        for c in child:
            c._setPerent(self)
            self._child.append(c)
        return self

    def clean(self):
        if self.backend is None:
            return
        if self.needbuild:
            return
        self.backend.clean()
        self.needbuild = True
        self._child = []


class Page(Widget):
    def __init__(self):
        super().__init__()
        self._setPerent(self)
        self.isroot = True
        self.backend = _backend.screen()


class Button(Widget):
    def _createBackend(self, parent: Widget):
        return _backend.btn(parent.backend)


class Text(Widget):
    def _createBackend(self, parent: Widget):
        return _backend.label(parent.backend)

    def _updateText(self, text):
        self.backend.set_text(text)


class PageManager:
    pageThis: Page = None
    pageList = []

    def enter(self, page: Page):
        self.clean()
        self.pageThis = page
        self.pageList.append(page)
        self.update()

    def back(self):
        if len(self.pageList) <= 1:
            return
        self.clean()
        _ = self.pageList.pop()
        self.pageThis = self.pageList[-1]
        self.update()

    def update(self):
        if self.pageThis is None:
            return
        self.pageThis.update()

    def clean(self):
        if self.pageThis is None:
            return
        self.pageThis.clean()


class _App:
    pageManager = PageManager()

    def update(self):
        self.pageManager.update()


app = _App()


def App():
    return app


try:
    import PikaUI_backend_lvgl
    set_backend(PikaUI_backend_lvgl)
    print('PikaUI: Using lvgl backend')
except:
    pass

try:
    import PikaUI_backend_text
    set_backend(PikaUI_backend_text)
    print('PikaUI: Using text backend')
except:
    pass

