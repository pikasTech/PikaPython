import PikaUI as ui
from PikaStdLib import MemChecker as mem

class Page1(ui.Page):
    def onclick_next(self, event):
        print('Page1: onclick_next')
        app.pageManager.enter(Page2())
        mem.now()

    def build(self):
        return ui.Widget(
            width=300,
            height=200,
            pos=(0, 50)
        ).add(
            ui.Text(
                text='Hello Page1',
                align=ui.ALIGN.CENTER
            ),
            ui.Button(
                text='Next',
                align=ui.ALIGN.CENTER,
                pos=(0, 50),
                height=30,
                width=80,
                onclick=self.onclick_next
            )
        )


class Page2(ui.Page):
    def on_click_back(self, event):
        app.pageManager.back()
        mem.now()

    def build(self):
        return ui.Widget(
            width= 400,
            height= 200,
            pos=(0, 50)
        ).add(
            ui.Text(
                text='Hello Page2',
                align=ui.ALIGN.CENTER
            ),
            ui.Button(
                text='Back',
                align=ui.ALIGN.CENTER,
                pos=(0, 50),
                height=30,
                width=80,
                onclick=self.on_click_back
            )
        )

app = ui.App()
app.pageManager.enter(Page1())
app.timer.cb(0)
mem.now()


# for i in range(100):
#     app.pageManager.enter(Page2())
#     app.timer.cb(0)
#     mem.now()
#     app.pageManager.back()
#     app.timer.cb(0)
#     mem.now()

