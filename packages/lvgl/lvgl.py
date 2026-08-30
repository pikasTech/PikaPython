from _lvgl import (
    ALIGN_CENTER,
    ALIGN_TOP_LEFT,
    btn,
    display_register,
    driver_step,
    init,
    input_register,
    label,
    obj,
    scr_act,
    timer,
    timer_handler,
)


class color_t:
    def __init__(self, value=None):
        self.ch = {"red": 0, "green": 0, "blue": 0, "alpha": 255}
        if value is not None:
            self.ch = value["ch"]
