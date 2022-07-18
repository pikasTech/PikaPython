import PikaCV as cv
img = cv.Image()
img.read("test/assets/test.jpg")
cv.Converter.toRGB888(img)
cv.Transforms.setROI(img,5,5,5,5)
cv.Converter.toBMP(img)
img.write("test/out/test1.bmp")
