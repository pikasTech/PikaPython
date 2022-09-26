import PikaCV as cv
img = cv.Image()
img.read("test/assets/test.jpg")

cv.Converter.toGray(img)
cv.Transforms.threshold(img,120,255,0)
cv.Converter.toBMP(img)
img.write("test/out/test.bmp")

