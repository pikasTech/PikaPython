import PikaCV as cv
import binascii
img = cv.Image()
img.read("test/assets/test2.jpg")

cv.Converter.toGray(img)
data = img.data()
print(binascii.b2a_hex(img.data()).decode())
cv.Converter.toBMP(img)
img.write("test/out/test.bmp")

