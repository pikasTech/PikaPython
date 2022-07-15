import PikaCV as cv
import binascii
img = cv.Image()
img.read("test/assets/test.jpg")
cv.Converter.toRGB888(img)

Channel = img.split()
R = Channel[0]
G = Channel[1]
B = Channel[2]

cv.Converter.toBMP(R)
cv.Converter.toBMP(G)
cv.Converter.toBMP(B)
R.write("test/out/R.bmp")
G.write("test/out/G.bmp")
B.write("test/out/B.bmp")

# img1 = cv.Image()
# cv.Converter.toRGB888(img1)
# img1.merge(R,G,B)
# cv.Converter.toBMP(img1)
# img1.write("test/assets/merge.bmp")

# print("Merge done")