import PikaCV as cv
from PikaStdData import Utils
img = cv.Image()
img.read("test/assets/test.jpg")

conv = cv.Converter()
conv.toRGB565(img)

ut = Utils()

print(str(ut.b2a_hex(img.data())))
