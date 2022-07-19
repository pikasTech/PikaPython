import PikaCV as cv
img = cv.Image()
img.read("test/assets/test.jpg")
cv.Converter.toRGB888(img)
thre = cv.Transforms.resize(img,25,25)
cv.Converter.toBMP(img)
img.write("test/out/test3.bmp")