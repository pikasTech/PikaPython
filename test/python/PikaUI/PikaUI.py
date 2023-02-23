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
                 align=ALIGN.TOP_MID,
                 onclick=None):
        self.width = width
        self.height = height
        self.pos = pos
        self.align = align
        self.text = text
        self.onclick = onclick

    def build(self):
        pass

    def _set_perent(self, parent):
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
            self.backend = self._create_backend(self.parent)

        if not self.isroot:
            self._update_align(self.align)
            self._update_attr(self.width, self.height, self.pos)
            self._update_text(self.text)
            self._update_event()

        for c in self._child:
            c.update()

    def _create_backend(self, parent: "Widget"):
        return _backend.widget(parent.backend)

    def _update_attr(self,
                     width,
                     height,
                     pos):
        self.backend.set_width(width)
        self.backend.set_height(height)
        if not pos is None:
            self.backend.set_pos(pos[0], pos[1])

    def _update_align(self, align):
        self.backend.align(align, 0, 0)

    def _update_text(self, text):
        if not None is text:
            self._label = _backend.label(self.backend)
            self._label.set_text(self.text)
            self._label.align(_backend.ALIGN.CENTER, 0, 0)

    def _update_event(self):
        if not None is self.onclick:
            self.backend.add_event_cb(self.onclick, _backend.EVENT.CLICKED, 0)

    def add(self, *child):
        for c in child:
            c._set_perent(self)
            self._child.append(c)
        return self

    def clean(self):
        if self.backend is None:
            return
        if self.needbuild:
            return
        # self.backend.clean()
        self.needbuild = True
        self._child = []


class Page(Widget):
    def __init__(self):
        super().__init__()
        self._set_perent(self)
        self.isroot = True
        self.backend = _backend.screen()


class Button(Widget):
    def _create_backend(self, parent: Widget):
        return _backend.btn(parent.backend)


class Text(Widget):
    def _create_backend(self, parent: Widget):
        return _backend.label(parent.backend)

    def _update_text(self, text):
        self.backend.set_text(text)


class PageManager:
    pageThis: Page = None
    pageList = []
    pageNeedEnter = None
    pageNeedBack = False

    def enter(self, page: Page):
        if not self.pageNeedEnter is None:
            return
        self.pageNeedEnter = page

    def back(self):
        if self.pageNeedBack:
            return
        self.pageNeedBack = True
    
    def handle_timer(self):
        if not self.pageNeedEnter is None:
            page = self.pageNeedEnter
            self.clean()
            self.pageThis = page
            self.pageList.append(page)
            self.update()
            self.pageNeedEnter = None
        
        if self.pageNeedBack:
            if len(self.pageList) <= 1:
                return
            self.clean()
            _ = self.pageList.pop()
            self.pageThis = self.pageList[-1]
            self.update()
            self.pageNeedBack = False

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
    timer = None
    def cb_timer(self, src):
        self.pageManager.handle_timer()

    def __init__(self):
        self.timer = _backend.timer()
        self.timer.set_period(50)
        self.timer.set_cb(self.cb_timer)

    def update(self):
        self.pageManager.update()
    
    def show(self, page: Page):
        self.pageManager.enter(page)


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

app = _App()


def App():
    return app
