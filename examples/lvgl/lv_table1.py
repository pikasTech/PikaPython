#!pika
import pika_lvgl as lv
import PikaStdLib
mem = PikaStdLib.MemChecker()

table = lv.table(lv.scr_act())

# Fill the first column
table.set_cell_value(0, 0, "Name")
table.set_cell_value(1, 0, "Apple")
table.set_cell_value(2, 0, "Banana")
table.set_cell_value(3, 0, "Lemon")
table.set_cell_value(4, 0, "Grape")
table.set_cell_value(5, 0, "Melon")
table.set_cell_value(6, 0, "Peach")
table.set_cell_value(7, 0, "Nuts")

# Fill the second column
table.set_cell_value(0, 1, "Price")
table.set_cell_value(1, 1, "$7")
table.set_cell_value(2, 1, "$4")
table.set_cell_value(3, 1, "$6")
table.set_cell_value(4, 1, "$2")
table.set_cell_value(5, 1, "$5")
table.set_cell_value(6, 1, "$1")
table.set_cell_value(7, 1, "$9")

# Set a smaller height to the table. It'll make it scrollable
table.set_height(200)
table.center()

print('mem used max: %0.2f kB' % (mem.getMax()))
print('mem used now: %0.2f kB' % (mem.getNow()))

#!pika