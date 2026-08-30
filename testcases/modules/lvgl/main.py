import lvgl as lv

lv.init()
screen = lv.scr_act()
button = lv.btn(screen)
button.set_x(12)
button.set_y(24)
assert button.get_x() == 12
assert button.get_y() == 24
button.align(screen, lv.ALIGN_CENTER, 4, 8)
assert button.get_x() == 4
assert button.get_y() == 8
label = lv.label(button)
label.set_text("LVGL v9")
assert label.get_text() == "LVGL v9"
color = lv.color_t({"ch": {"red": 255, "green": 1, "blue": 2, "alpha": 255}})
assert color.ch["red"] == 255
events = []
button.add_event_cb(lambda code, user_data: events.append(
    (code, user_data)
), 7, 11)
button.trigger_event(7)
assert events == [(7, 11)]
label.delete()
button.delete()

timer_hits = []
timer = lv.timer(lambda period, user_data: timer_hits.append(
    (period, user_data)
), 25, 13)
assert lv.timer_handler() == 1
assert timer_hits == [(25, 13)]
timer.delete()

framebuffer = bytearray(8)
framebuffer[0] = 3
lv.display_register(framebuffer)
assert framebuffer[0] == 4
lv.input_register(2)
assert lv.driver_step() == 10
print("[LVGL][PASS] phase1-core")
