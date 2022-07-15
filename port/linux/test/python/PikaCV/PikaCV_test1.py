import PikaCV as cv
import binascii
img = cv.Image()
img.read("test/assets/test.jpg")

cv.Converter.toRGB565(img)
print(str(binascii.b2a_hex(img.data())))
cv.Converter.toBMP(img)
img.write("test/assets/test.bmp")

