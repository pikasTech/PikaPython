import PikaStdLib
import pika_lvgl as lv
import time

global_dict = {}
global_list = []


class MyMeter:
    def __init__(self,
                 parent,
                 label_text="label",
                 value=0,
                 value_set=0,
                 unit="°C"):
        container = lv.obj(parent)
        container.set_size(200, 250)
        container.clear_flag(lv.obj.FLAG.SCROLLABLE)
        container.set_style_bg_opa(lv.OPA.TRANSP, 0)
        container.set_style_border_width(0, 0)
        meter = lv.meter(container)
        meter.align(lv.ALIGN.TOP_MID, 0, -20)
        meter.set_size(200, 200)

        # Add a scale first
        scale = meter.add_scale()
        meter.set_scale_ticks(
            scale, 51, 2, 10, lv.palette_main(lv.PALETTE.GREY))
        meter.set_scale_major_ticks(scale, 10, 4, 15, lv.color_black(), 10)

        indic = lv.meter_indicator_t()

        # Add a blue arc to the start
        indic = meter.add_arc(scale, 3, lv.palette_main(lv.PALETTE.BLUE), 0)
        meter.set_indicator_start_value(indic, 0)
        meter.set_indicator_end_value(indic, 20)

        # Make the tick lines blue at the start of the scale
        indic = meter.add_scale_lines(scale, lv.palette_main(
            lv.PALETTE.BLUE), lv.palette_main(lv.PALETTE.BLUE), False, 0)
        meter.set_indicator_start_value(indic, 0)
        meter.set_indicator_end_value(indic, 20)

        # Add a red arc to the end
        indic = meter.add_arc(scale, 3, lv.palette_main(lv.PALETTE.RED), 0)
        meter.set_indicator_start_value(indic, 80)
        meter.set_indicator_end_value(indic, 100)

        # Make the tick lines red at the end of the scale
        indic = meter.add_scale_lines(scale, lv.palette_main(
            lv.PALETTE.RED), lv.palette_main(lv.PALETTE.RED), False, 0)
        meter.set_indicator_start_value(indic, 80)
        meter.set_indicator_end_value(indic, 100)

        # Add a needle line indicator
        indic = meter.add_needle_line(
            scale, 4, lv.palette_main(lv.PALETTE.GREY), -10)
        label = lv.label(meter)
        label.set_text(label_text + unit)
        label.align(lv.ALIGN.CENTER, 0, -20)

        value_label = lv.label(meter)
        value_label.align(lv.ALIGN.CENTER, 0, 60)

        value_set_input = InputBox(container, label_text="given", unit=unit)
        value_set_input.align(lv.ALIGN.BOTTOM_MID, 0, 20)

        self.container = container
        self.lv_meter = meter
        self.value_label = value_label
        self.indic = indic
        self.unit = unit

        self.set_value(value)

    def align(self, align, x, y):
        self.container.align(align, x, y)

    def set_value(self, value):
        # str_value = "%.1f" % value
        str_value = str(value)
        self.value_label.set_text(str_value + " " + self.unit)
        self.lv_meter.set_indicator_end_value(self.indic, value)


class InputBox:
    def __init__(self, parent, label_text="label", value='0.0', unit="", box_num=1, box_width=80):
        self.input_box_list = []
        global_list.append(self)
        container = lv.obj(parent)
        container.set_size(110 + box_width * box_num, 50)
        container.clear_flag(lv.obj.FLAG.SCROLLABLE)
        container.set_style_bg_opa(lv.OPA.TRANSP, 0)
        container.set_style_border_width(0, 0)
        label = lv.label(container)
        label.set_text(label_text)
        label.align(lv.ALIGN.LEFT_MID, 0, 0)
        for i in range(box_num):
            input_box = lv.textarea(container)
            input_box.set_size(box_width, 40)
            input_box.align(lv.ALIGN.LEFT_MID, 40 + box_width*i, 0)
            input_box.clear_flag(lv.obj.FLAG.SCROLLABLE)
            input_box.set_one_line(True)
            self.input_box_list.append(input_box)
            input_box.set_text(value)
            input_box.add_event_cb(self.event_cb, lv.EVENT.FOCUSED, None)
        unit_label = lv.label(container)
        unit_label.set_text(unit)
        unit_label.align(lv.ALIGN.LEFT_MID, 50 + box_width*box_num, 0)
        self.container = container

    def align(self, align, x, y):
        self.container.align(align, x, y)

    def set_size(self, width, height):
        self.container.set_size(width, height)

    def event_cb(self, e):
        code = e.get_code()
        ta = e.get_target()
        kb: KeyBoard = global_dict["keyboard"]
        if code == lv.EVENT.FOCUSED:
            kb.set_hiden(False)
            kb.set_textarea(ta)
        if code == lv.EVENT.DEFOCUSED:
            kb.set_textarea(None)


class ProcessCount:
    def __init__(self, parent, label_text="label", value=0, totle=0):
        container = lv.obj(parent)
        container.set_size(150, 40)
        container.clear_flag(lv.obj.FLAG.SCROLLABLE)
        label = lv.label(container)
        label.set_text(label_text)
        label.align(lv.ALIGN.BOTTOM_MID, 0, 0)
        value_label = lv.label(container)
        value_label.align(lv.ALIGN.TOP_MID, 0, 0)
        self.container = container
        self.value_label = value_label
        self.set_value(value, totle)

    def align(self, align, x, y):
        self.container.align(align, x, y)

    def set_value(self, value, totle):
        self.value_label.set_text(str(value) + " / " + str(totle))


class TimerView:
    def __init__(self,
                 parent,
                 label_text="label",
                 seconds_this=0,
                 seconds_totle=0):
        self.seconds_this = seconds_this
        self.seconds_totle = seconds_totle
        container = lv.obj(parent)
        container.set_size(200, 80)
        container.clear_flag(lv.obj.FLAG.SCROLLABLE)
        label = lv.label(container)
        label.set_text(label_text)
        label.align(lv.ALIGN.TOP_MID, 0, -10)
        label_this = lv.label(container)
        label_this.align(lv.ALIGN.TOP_MID, 0, 10)
        label_totle = lv.label(container)
        label_totle.align(lv.ALIGN.TOP_MID, 0, 30)
        self.container = container
        self.label_this = label_this
        self.label_totle = label_totle
        self.set_value(seconds_this, seconds_totle)

    def align(self, align, x, y):
        self.container.align(align, x, y)

    def set_value(self, seconds_this, seconds_totle):
        self.label_this.set_text(
            "time now" + str(seconds_this//60) + " min " + str(seconds_this % 60) + " s")
        self.label_totle.set_text(
            "time total " + str(seconds_totle//60) + " min " + str(seconds_totle % 60) + " s")


class PIDPanel:
    def __init__(self, parent, label_text="label"):
        container = lv.obj(parent)
        container.set_size(300, 300)
        container.clear_flag(lv.obj.FLAG.SCROLLABLE)
        label = lv.label(container)
        label.set_text(label_text)
        label.align(lv.ALIGN.TOP_MID, 0, 0)
        label_partition = InputBox(container, label_text="partition")
        label_partition.align(lv.ALIGN.TOP_LEFT, 20, 20)
        label_top = lv.label(container)
        label_top.set_text("top partition")
        label_bottom = lv.label(container)
        label_bottom.set_text("bottom partition")
        label_top.align(lv.ALIGN.TOP_MID, 0, 80)
        label_bottom.align(lv.ALIGN.TOP_MID, 0, 160)
        input_box_P_top = InputBox(container, label_text="P")
        input_box_P_top.align(lv.ALIGN.TOP_LEFT, -20, 100)
        input_box_I_top = InputBox(container, label_text="I")
        input_box_I_top.align(lv.ALIGN.TOP_LEFT, 110, 100)
        input_box_P_bottom = InputBox(container, label_text="P")
        input_box_P_bottom.align(lv.ALIGN.TOP_LEFT, -20, 180)
        input_box_I_bottom = InputBox(container, label_text="I")
        input_box_I_bottom.align(lv.ALIGN.TOP_LEFT, 110, 180)
        self.container = container

    def align(self, align, x, y):
        self.container.align(align, x, y)

    def set_size(self, width, height):
        self.container.set_size(width, height)


class Button:
    def __init__(self, parent, label_text="label"):
        btn = lv.btn(parent)
        btn.set_size(100, 50)
        btn.clear_flag(lv.obj.FLAG.SCROLLABLE)
        btn.set_style_bg_color(lv.color_white(), 0)
        btn.set_style_text_color(lv.color_black(), 0)
        btn.set_style_border_color(lv.color_black(), 0)
        label = lv.label(btn)
        label.set_text(label_text)
        label.align(lv.ALIGN.CENTER, 0, 0)
        self.is_on = False
        self.btn = btn
        self.label = label

    def align(self, align, x, y):
        self.btn.align(align, x, y)

    def set_text(self, text):
        self.label.set_text(text)

    def set_is_on(self, is_on):
        self.is_on = is_on

    def _event_cb_adapter(self, e):
        self.event_cb(self, e)

    def add_event_cb(self, event_cb, event_type, user_data=None):
        self.event_cb = event_cb
        self.btn.add_event_cb(self._event_cb_adapter, event_type, user_data)

    def set_size(self, width, height):
        self.btn.set_size(width, height)



class ProcessBar:
    def __init__(self, parent, value=0):
        bar = lv.bar(parent)
        bar.remove_style_all()   # To have a clean start
        bar.add_style(bar_style_bg, 0)
        bar.add_style(bar_style_indic, lv.PART.INDICATOR)
        bar.set_size(200, 40)
        self.bar = bar
        self.style_bg = bar_style_bg
        self.style_indic = bar_style_indic
        self.set_value(value)

    def align(self, align, x, y):
        self.bar.align(align, x, y)

    def set_value(self, value, anim=lv.ANIM.ON):
        self.bar.set_value(value, anim)


def tab1_btnbar1_event_cb(self: "BtnProcessBar", e):
    if self.btn.is_on:
        self.btn.set_is_on(False)
        self.btn.set_text("start")
        self.bar.set_value(0)
    else:
        self.btn.set_is_on(True)
        self.btn.set_text("stop")
        self.bar.set_value(100)


class BtnProcessBar:
    def __init__(self, parent, label_text="label", value=0):
        container = lv.obj(parent)
        container.set_size(350, 80)
        container.clear_flag(lv.obj.FLAG.SCROLLABLE)
        container.set_style_bg_opa(lv.OPA.TRANSP, 0)
        # 无边框
        container.set_style_border_width(0, 0)
        btn = Button(container, label_text=label_text)
        bar = ProcessBar(container, value=value)
        btn.align(lv.ALIGN.LEFT_MID, 0, 0)
        bar.align(lv.ALIGN.RIGHT_MID, 0, 0)
        self.container = container
        self.btn = btn
        self.bar = bar

    def align(self, align, x, y):
        self.container.align(align, x, y)

    def _event_cb_adapter(self, e):
        self.event_cb(self, e)

    def add_event_cb(self, event_cb, event_type, user_data=None):
        self.event_cb = event_cb
        self.btn.btn.add_event_cb(
            self._event_cb_adapter, event_type, user_data)


class Chart:
    def __init__(self, parent, label_text="label"):
        container = lv.obj(parent)
        container.set_size(900, 450)
        container.clear_flag(lv.obj.FLAG.SCROLLABLE)
        container.set_style_bg_opa(lv.OPA.TRANSP, 0)
        container.set_style_border_width(0, 0)
        label = lv.label(container)
        label.set_text(label_text)
        label.align(lv.ALIGN.TOP_MID, 0, -20)
        chart = lv.chart(container)
        chart.set_size(800, 400)
        chart.align(lv.ALIGN.CENTER, 0, 0)
        chart.set_type(lv.chart.TYPE.LINE)   # Show lines and points too
        # Add two data series
        ser1 = chart.add_series(lv.palette_main(
            lv.PALETTE.RED), lv.chart.AXIS.PRIMARY_Y)
        chart.set_axis_tick(lv.chart.AXIS.PRIMARY_Y, 10, 5, 6, 2, True, 50)
        chart.set_axis_tick(lv.chart.AXIS.PRIMARY_X, 10, 5, 6, 2, True, 50)
        # Set next points on ser1
        chart.set_next_value(ser1, 10)
        chart.set_next_value(ser1, 10)
        chart.set_next_value(ser1, 10)
        chart.set_next_value(ser1, 10)
        chart.set_next_value(ser1, 10)
        chart.set_next_value(ser1, 10)
        chart.set_next_value(ser1, 10)
        chart.set_next_value(ser1, 30)
        chart.set_next_value(ser1, 70)
        chart.set_next_value(ser1, 90)
        chart.refresh()
        self.container = container

    def align(self, align, x, y):
        self.container.align(align, x, y)


class KeyBoard:
    def __init__(self, parent, label_text="label", is_number=True):
        container = lv.obj(parent)
        if is_number:
            width = 300
        else:
            width = 600
        container.set_size(width, 360)
        container.clear_flag(lv.obj.FLAG.SCROLLABLE)
        # container.set_style_bg_opa(lv.OPA.TRANSP, 0)
        # container.set_style_border_width(0, 0)
        label = lv.label(container)
        label.set_text(label_text)
        label.align(lv.ALIGN.TOP_MID, 0, 0)
        kb = lv.keyboard(container)
        kb.set_size(width, 300)
        kb.align(lv.ALIGN.BOTTOM_MID, 0, 20)
        # kb.add_event_cb(self.event_cb, lv.EVENT.ALL, None)
        if is_number:
            kb.set_mode(lv.keyboard.MODE.NUMBER)
        else:
            kb.set_mode(lv.keyboard.MODE.TEXT_LOWER)
        global_dict["keyboard"] = self
        close_btn = Button(container, label_text="close")
        close_btn.set_size(40, 40)
        close_btn.align(lv.ALIGN.TOP_RIGHT, 0, -10)
        close_btn.btn.add_event_cb(
            self.close_btn_event_cb, lv.EVENT.CLICKED, None)
        container.add_event_cb(
            self.drag_event_cb, lv.EVENT.ALL, None)
        self.is_hidden = True
        self.container = container
        self.close_btn = close_btn
        self.kb = kb

    def close_btn_event_cb(self, e):
        self.set_hiden(True)

    def drag_event_cb(self, e):
        code = e.get_code()
        if code == lv.EVENT.PRESSED:
            indev = lv.indev_get_act()
            self.drag_start_point = indev.get_point()
            self.container_start_x = self.container.get_x()
            self.container_start_y = self.container.get_y()
            self.container.set_align(lv.ALIGN.TOP_LEFT)
            # print("drag start", self.drag_start_point.x, self.drag_start_point.y)
        if code == lv.EVENT.PRESSING:
            indev = lv.indev_get_act()
            drag_point = indev.get_point()
            # print("drag", drag_point.x, drag_point.y)
            self.container.set_pos(
                self.container_start_x + drag_point.x - self.drag_start_point.x, self.container_start_y + drag_point.y - self.drag_start_point.y)

    def event_cb(self, e):
        code = e.get_code()
        ta = e.get_target()
        # if code == lv.EVENT.FOCUSED:
        #     self.kb.move_foreground()
        #     self.kb.set_textarea(ta)
        #     self.kb.clear_flag(lv.obj.FLAG.HIDDEN)

        # if code == lv.EVENT.DEFOCUSED:
        #     self.kb.set_textarea(None)
        #     self.kb.add_flag(lv.obj.FLAG.HIDDEN)

    def align(self, align, x, y):
        self.container.align(align, x, y)

    def move_foreground(self):
        self.container.move_foreground()

    def move_background(self):
        self.container.move_background()

    def set_hiden(self, is_hidden):
        # print("set_hiden", is_hidden)
        self.is_hidden = is_hidden
        if is_hidden:
            self.container.add_flag(lv.obj.FLAG.HIDDEN)
        else:
            self.container.clear_flag(lv.obj.FLAG.HIDDEN)
            self.move_foreground()

    def switch_hidden(self):
        self.set_hiden(not self.is_hidden)

    def set_textarea(self, ta):
        self.kb.set_textarea(ta)


def kb_btn_event_cb(self, e):
    # print("kb_btn_event_cb")
    kb: KeyBoard = global_dict["keyboard"]
    kb.switch_hidden()


def date_time_update_cb(timer):
    datetime_label = global_dict["datetime_label"]
    asctime = time.asctime()
    datetime_label.set_text(asctime)
    print(asctime)


def main():
    global bar_style_bg
    global bar_style_indic
    bar_style_bg = lv.style_t()
    bar_style_indic = lv.style_t()

    bar_style_bg.init()
    bar_style_bg.set_border_color(lv.palette_main(lv.PALETTE.BLUE))
    bar_style_bg.set_border_width(2)
    bar_style_bg.set_pad_all(6)            # To make the indicator smaller
    bar_style_bg.set_radius(6)
    bar_style_bg.set_anim_time(1000)

    bar_style_indic.init()
    bar_style_indic.set_bg_opa(lv.OPA.COVER)
    bar_style_indic.set_bg_color(lv.palette_main(lv.PALETTE.BLUE))
    bar_style_indic.set_radius(3)

    # print('hello pikapython!')
    mem = PikaStdLib.MemChecker()

    STATUS_BAR_HEIGHT = 40
    LOGO_WIDTH = 100
    TABVIEW_HEIGHT = 40

    lv.lock()
    status_bar = lv.obj(lv.scr_act())
    status_bar.set_size(lv.pct(100), STATUS_BAR_HEIGHT)
    status_bar.clear_flag(lv.obj.FLAG.SCROLLABLE)
    lv.scr_act().clear_flag(lv.obj.FLAG.SCROLLABLE)

    # logo label
    logo = lv.obj(status_bar)
    logo.set_size(LOGO_WIDTH, STATUS_BAR_HEIGHT)
    logo_label = lv.label(logo)
    logo_label.set_text("logo")
    logo_label.align(lv.ALIGN.CENTER, 0, 0)
    logo.align(lv.ALIGN.LEFT_MID, -20, 0)
    logo.clear_flag(lv.obj.FLAG.SCROLLABLE)

    datetime_label = lv.label(status_bar)
    datetime_label.set_text("2023.12.20 16:20:08")
    datetime_label.align(lv.ALIGN.LEFT_MID, LOGO_WIDTH, 0)
    global_dict["datetime_label"] = datetime_label

    timer = lv.timer_create_basic()
    global_list.append(timer)
    timer.set_period(1)
    timer.set_cb(date_time_update_cb)

    # Keyboard
    keyboard = KeyBoard(lv.scr_act(), label_text="", is_number=False)
    keyboard.align(lv.ALIGN.BOTTOM_MID, 0, 0)

    keyboard_btn = Button(status_bar, label_text="keyboard")
    keyboard_btn.align(lv.ALIGN.RIGHT_MID, -20, 0)
    keyboard_btn.set_size(60, 30)
    global_dict["keyboard_btn"] = keyboard_btn
    keyboard_btn.add_event_cb(kb_btn_event_cb, lv.EVENT.CLICKED, None)

    # 创建一个tabview对象
    tabview = lv.tabview(lv.scr_act(), lv.DIR.TOP, TABVIEW_HEIGHT)
    tabview.clear_flag(lv.obj.FLAG.SCROLLABLE)
    tabview.align(lv.ALIGN.TOP_LEFT, 0, STATUS_BAR_HEIGHT)
    tabview.set_size(lv.pct(100), lv.pct(100) - 10)

    # 添加标签到tabview
    tab1 = tabview.add_tab("run")
    tab2 = tabview.add_tab("debug")
    tab3 = tabview.add_tab("log")
    tab4 = tabview.add_tab("alert")
    tab5 = tabview.add_tab("help")
    tab6 = tabview.add_tab("program")
    tab7 = tabview.add_tab("edit")


    # tab1
    meter_temp_box = MyMeter(tab1, label_text="box temp", unit="°C")
    meter_temp_box.align(lv.ALIGN.TOP_LEFT, 20, 0)

    meter_humi = MyMeter(tab1, label_text="meter humi", unit="%")
    meter_humi.align(lv.ALIGN.TOP_RIGHT, -20, 0)

    meter_temp_tube = MyMeter(tab1, label_text="tube temp", unit="°C")
    meter_temp_tube.align(lv.ALIGN.TOP_MID, 0, 0)

    meter_temp_box.set_value(20)
    meter_humi.set_value(50)
    meter_temp_tube.set_value(30)

    process_count_segment = ProcessCount(
        tab1, label_text="tim", value=0, totle=10)
    process_count_segment.align(lv.ALIGN.BOTTOM_LEFT, 0, -100)

    process_count_loop = ProcessCount(tab1, label_text="loop", value=0, totle=10)
    process_count_loop.align(lv.ALIGN.BOTTOM_LEFT, 160, -100)

    timer_segment = TimerView(tab1, label_text="segment",
                              seconds_this=5, seconds_totle=10)
    timer_totle = TimerView(tab1, label_text="timer totle",
                            seconds_this=0, seconds_totle=200)

    tab1_process_bar = BtnProcessBar(tab1, label_text="start")
    tab1_process_bar.align(lv.ALIGN.BOTTOM_LEFT, 0, 0)
    tab1_process_bar.add_event_cb(tab1_btnbar1_event_cb, lv.EVENT.CLICKED)
    global_dict["tab1_process_bar"] = tab1_process_bar

    # tab2
    timer_segment.align(lv.ALIGN.BOTTOM_RIGHT, -220, -20)
    timer_totle.align(lv.ALIGN.BOTTOM_RIGHT, 0, -20)

    pid_circle = PIDPanel(tab2, label_text="loop PID")
    pid_circle.align(lv.ALIGN.TOP_LEFT, 0, 0)

    pid_plate = PIDPanel(tab2, label_text="plate PID")
    pid_plate.align(lv.ALIGN.TOP_LEFT, 300, 0)

    pid_hit = PIDPanel(tab2, label_text="hit PID")
    pid_hit.align(lv.ALIGN.TOP_LEFT, 600, 0)
    pid_hit.set_size(350, 400)
    # 低气压PID
    label_low_pressure = lv.label(pid_hit.container)
    label_low_pressure.set_text("lo pressure PID")
    label_low_pressure.align(lv.ALIGN.BOTTOM_MID, 0, -100)
    input_box_in = InputBox(pid_hit.container, label_text="in", box_num=2)
    input_box_in.align(lv.ALIGN.BOTTOM_LEFT, 0, -50)
    input_box_out = InputBox(pid_hit.container, label_text="out", box_num=2)
    input_box_out.align(lv.ALIGN.BOTTOM_LEFT, 0, 0)
    tab2_btn1 = Button(tab2, label_text="btn1")
    tab2_btn1.align(lv.ALIGN.BOTTOM_RIGHT, -150, 0)
    tab2_btn2 = Button(tab2, label_text="btn2")
    tab2_btn2.align(lv.ALIGN.BOTTOM_RIGHT, -20, 0)

    # tab3
    chart_temp = Chart(tab3, label_text="chart temp")
    chart_temp.align(lv.ALIGN.CENTER, 0, 0)
    tabview.set_act(0, lv.ANIM.OFF)

    lv.unlock()

    mem.max()


main()

for i in range(10):
    time.sleep(0.1)
    lv.task_handler()

lv.deinit()
