class _Object:
    def __init__(self, parent=None):
        self.parent = parent
        self.x = 0
        self.y = 0
        self.text = ""
        self.events = []

    def delete(self):
        self.events = []

    def set_x(self, value):
        self.x = value

    def set_y(self, value):
        self.y = value

    def get_x(self):
        return self.x

    def get_y(self):
        return self.y

    def align(self, target, align, x, y):
        self.x = x
        self.y = y

    def add_event_cb(self, callback, code, user_data=0):
        self.events.append((callback, code, user_data))

    def trigger_event(self, code):
        for callback, expected, user_data in self.events:
            if expected == code:
                callback(code, user_data)


class obj(_Object):
    pass


class btn(_Object):
    pass


class label(_Object):
    def set_text(self, value):
        self.text = value

    def get_text(self):
        return self.text


class color_t:
    def __init__(self, value=None):
        self.ch = {"red": 0, "green": 0, "blue": 0, "alpha": 255}
        if value is not None:
            self.ch = value["ch"]


class timer:
    timers = []

    def __init__(self, callback, period_ms, user_data=0):
        self.callback = callback
        self.period_ms = period_ms
        self.user_data = user_data
        self.active = True
        self.timers.append(self)

    def trigger(self):
        if self.active:
            self.callback(self.period_ms, self.user_data)

    def delete(self):
        self.active = False


ALIGN_CENTER = 0
ALIGN_TOP_LEFT = 1
_screen = obj()
_framebuffer_length = 0
_input_state = 0


def init():
    global _screen
    _screen = obj()
    timer.timers = []


def scr_act():
    return _screen


def timer_handler():
    count = 0
    for value in list(timer.timers):
        if value.active:
            value.trigger()
            count += 1
    return count


def display_register(buffer):
    global _framebuffer_length
    _framebuffer_length = len(buffer)
    buffer[0] += 1


def input_register(state):
    global _input_state
    _input_state = state


def driver_step():
    return _framebuffer_length + _input_state
