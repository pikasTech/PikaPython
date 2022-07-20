import PikaCV as cv
img = cv.Image()
img.read("test/assets/test.jpg")
cv.Converter.toRGB888(img)
cv.Filter.medianFilter(img)
cv.Converter.toBMP(img)
img.write("test/out/test9.bmp")