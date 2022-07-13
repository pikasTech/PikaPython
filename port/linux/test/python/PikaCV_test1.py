import PikaCV as cv
from PikaStdData import Utils
img = cv.Image()
img.read("test/assets/test.jpg")

cv.Converter.toRGB565(img)
print(str(Utils.b2a_hex(img.data())))
cv.Converter.toBMP(img)
img.write("test/assets/test.bmp")

