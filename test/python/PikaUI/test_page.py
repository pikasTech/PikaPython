import PikaUI as ui

page1 = ui.Page().add(
    ui.Text('Hello Page1')
)


class Page2(ui.Page):
    def build(self):
        self.add(
            ui.Text('Hello Page2')
        )


app = ui.App()
app.pageManager.open(page1)
app.pageManager.open(Page2())
app.pageManager.pop()
