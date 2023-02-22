
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

class Screen(widget):
    name = "screen"

_screen = Screen(None)

class label(widget):
    name = "label"

def screen():
    return _screen
