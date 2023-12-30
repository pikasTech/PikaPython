import PikaStdLib
import pika_lvgl as lv
print('hello pikapython!')
mem = PikaStdLib.MemChecker()
print('mem used max:')
mem.max()

# Create a Tab view object
tabview = lv.tabview(lv.scr_act(), lv.DIR.TOP, 50)

# Add 3 tabs (the tabs are page (lv_page) and can be scrolled
tab1 = tabview.add_tab("Tab 1")
tab2 = tabview.add_tab("Tab 2")
tab3 = tabview.add_tab("Tab 3")

# Add content to the tabs
label = lv.label(tab1)
label.set_text("This the first tab\n\
\n\
If the content\n\
of a tab\n\
becomes too\n\
longer\n\
than the\n\
container\n\
then it\n\
automatically\n\
becomes\n\
scrollable.\n\
\n\
\n\
\n\
Can you see it?")

label = lv.label(tab2)
label.set_text("Second tab")

label = lv.label(tab3)
label.set_text("Third tab");

label.scroll_to_view_recursive(lv.ANIM.ON)

