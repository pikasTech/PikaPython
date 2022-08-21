import PikaCV as cv
img = cv.Image()
img.read("test/assets/test.jpg")
cv.Converter.toRGB888(img)
thre = cv.Transforms.getOTSUthre(img)
print(thre)
cv.Transforms.setOTSU(img)
cv.Converter.toBMP(img)
img.write("test/out/test2.bmp")
