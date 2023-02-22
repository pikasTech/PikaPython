import PikaUI as ui
from PikaStdLib import MemChecker as mem

page1 = ui.Page().add(
    ui.Text('Hello Page1')
)


class Page2(ui.Page):
    def build(self):
        return ui.Text('Hello Page2')


app = ui.App()
app.pageManager.enter(page1)
app.pageManager.enter(Page2())
app.pageManager.back()
