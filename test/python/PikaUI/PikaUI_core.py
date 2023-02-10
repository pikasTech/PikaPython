import weakref
# class ALIGN(_backend.ALIGN): (vm not pass)
#     pass


class _ALIGN:
    CENTER = 0
    TOP_MID = 1


ALIGN = _ALIGN
_backend = None


def set_backend(backend):
    global _backend
    global ALIGN
    global page
    _backend = backend
    ALIGN = _backend.ALIGN
    page = _Page()
    page._setPerent(page)
    page.isroot = True
    page.backend = _backend.scr_act()


class Widget:
    backend = None
    width = 0
    height = 0
    pos = None
    parent = None
    align = None
    text = None
    isroot = False
    _label = None
    _child = []

    def __init__(self,
                 width=100,
                 height=100,
                 pos=None,
                 text=None,
                 align=ALIGN.TOP_MID):
        self.width = width
        self.height = height
        self.pos = pos
        self.align = align
        self.text = text

    def _setPerent(self, parent):
        self.parent = weakref.ref(parent)

    def update(self):
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
        return _backend.lv_obj(parent.backend)

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


class _Page(Widget):
    def _createBackend(self, parent: Widget):
        return _backend.scr_act()


class Button(Widget):
    def _createBackend(self, parent: Widget):
        return _backend.btn(parent.backend)


class Text(Widget):
    def _createBackend(self, parent: Widget):
        return _backend.label(parent.backend)

    def _updateText(self, text):
        self.backend.set_text(text)


page = _Page()


def Page():
    return page
