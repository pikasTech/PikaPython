import PikaUI as ui
from PikaStdLib import MemChecker as mem


class MainContainer(ui.Container):
    def onclick_next(self, event):
        print('Page1: onclick_next')
        app.pageManager.enter(Page2())
        mem.now()

    def build(self):
        return [
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
        ]


class Page1(ui.Page):
    def build(self):
        return [
            MainContainer(
                width=300,
                height=200,
                pos=(0, 50)
            ),
            ui.Text("Title")
        ]


class Page2(ui.Page):
    def build(self):
        return ui.Container(
            width=400,
            height=200,
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
            )
        )


app = ui.App()
app.pageManager.enter(Page2())
app.timer.cb(0)
mem.now()
app.pageManager.enter(Page2())
app.timer.cb(0)
mem.now()
app.pageManager.back()
app.timer.cb(0)
mem.now()

gcdump()

# for i in range(100):
#     app.pageManager.enter(Page2())
#     app.timer.cb(0)
#     mem.now()
#     app.pageManager.back()
#     app.timer.cb(0)
#     mem.now()
