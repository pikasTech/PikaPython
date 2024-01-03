import lvgl as lv

f = open('/pikafs/1.png', 'rb')
img_data = f.read()
f.close()  

   
img_dsc = lv.img_dsc_t({
    "data_size": len(img_data),
    "data": img_data,
})

img = lv.img(lv.scr_act())
img.set_zoom(500)
img.align(lv.ALIGN.CENTER, 0, -20)
img.set_src(img_dsc)
