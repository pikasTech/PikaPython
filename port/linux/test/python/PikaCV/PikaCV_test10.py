import PikaCV as cv
img = cv.Image()
img.read("test/assets/test.jpg")
cv.Converter.toGray(img)
cv.Transforms.adaptiveThreshold(img,255,3,0,0)
cv.Converter.toBMP(img)
img.write("test/out/test10.bmp")
