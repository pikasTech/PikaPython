
class widget:
    name = "widget"

    def __init__(self, parent):
        if parent == None:
            name = 'None'
        else:
            name = parent.name
        print('%s.__init__(%s)' % (self.name, name))

    def align(self, align, posx, posy):
        print('%s.align(%d, %d, %d)' % (self.name, align, posx, posy))

    def set_width(self, width):
        print('%s.set_wdith(%d)' % (self.name, width))

    def set_height(self, height):
        print('%s.set_height(%d)' % (self.name, height))
    
    def set_text(self, text):
        print('%s.set_text("%s")' % (self.name, text))
    
    def clean(self):
        print("%s.clean()" % (self.name))
    
    def del_(self):
        print("%s.del()" % (self.name))
    
    def set_pos(self, x, y):
        print("%s.set_pos(%d, %d)" % (self.name, x, y))
    
    def add_event_cb(self, fn, event, data):
        print("%s.add_event_cb(%s, %d, %d)" % (self.name, str(fn), event, data))

class Screen(widget):
    name = "screen"

_screen = Screen(None)

class label(widget):
    name = "label"

class btn(widget):
    name = "btn"

class timer:
    cb = None
    def set_period(self, period):
        print('timer.set_period(%d)' % period)
    
    def set_cb(self, cb):
        self.cb = cb
        print('timer.set_cb(%s)' % str(cb))

def screen():
    return _screen



class ALIGN:
    CENTER = 0
    TOP_MID = 1


class EVENT:
    CLICKED = 0