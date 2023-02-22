import PikaUI as ui

page1 = ui.Page().add(
    ui.Text('Hello Page1')
)


class Page2(ui.Page):
    def build(self):
        _ = self.add(
            ui.Text('Hello Page2')
        )


app = ui.App()
app.pageManager.enter(page1)
app.pageManager.enter(Page2())
app.pageManager.back()
