
class lv_obj:
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
        print('%s.set_text(%s)' % (self.name, text))

class Screen:
    name = "screen"

_screen = Screen(None)

class label(lv_obj):
    name = "label"

def scr_act():
    return _screen
