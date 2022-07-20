import PikaCV as cv
img = cv.Image()
img.read("test/assets/test.jpg")
cv.Converter.toRGB888(img)
thre = cv.Filter.meanFilter(img,3,3)
cv.Converter.toBMP(img)
img.write("test/out/test4.bmp")