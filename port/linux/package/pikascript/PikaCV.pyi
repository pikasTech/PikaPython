from PikaObj import *


class ImageFormat(TinyObj):
    """The ImageFormat class is used to 
    store the image format enum of an image."""
    RGB888: int
    RGB565: int
    GRAY: int
    JPEG: int
    def __init__(self): ...


class Image(TinyObj):
    """Create a empty image. The image can be 
    filled with data by read a file e.g.: `read()` 
    or load bytes e.g:. `loadRGB888`, `loadRGB565`, `loadGray` or `loadJpeg`"""

    def __init__(self): ...

    def read(self, path: str):
        """Read the image from the specified path, 
        Need implement the   `__platform_fopen()`, `__platform_fread()`
          and `__platform_fclose()`"""
        ...

    def write(self, path: str):
        """Write the image to the specified path, 
        Need implement the   `__platform_fopen()`, `__platform_fwrite()` 
        and `__platform_fclose()`"""
        ...

    def loadJpeg(self, bytes: any):
        """Load the image from bytes"""

    def loadRGB888(self, width: int, height: int, bytes: bytes):
        """Load the image from bytes"""

    def loadRGB565(self, width: int, hight: int, bytes: bytes):
        """Load the image from bytes"""

    def loadGray(self, width: int, hight: int, bytes: bytes):
        """Load the image from bytes"""

    def width(self) -> int:
        """Get the width of the image"""

    def hight(self) -> int:
        """Get the hight of the image"""

    def format(self) -> int:
        """Get the format of the image. 
        The format is one of the `ImageFormat` enum, 
        like `ImageFormat.RGB888`"""

    def data(self) -> bytes:
        """Get the data of the image"""

    def getPixel(self, x: int, y: int, channel: int) -> int:
        """Get the pixel value of the specified channel.
        For example, if the format of image is `RGB888`, 
        the channel `0`, `1`, `2`, means `R`, `G`, `B`, 
        and for the format of `GRAY8`, the channel is `0`
        """

    def setPixel(self, x: int, y: int, channel: int, value: int):
        """Set the pixel value of the specified channel.
        For example, if the format of image is `RGB888`, 
        the channel `0`, `1`, `2`, means `R`, `G`, `B`, 
        and for the format of `GRAY8`, the channel is `0`
        """

    def size(self) -> int:
        """Get the size of the image by bytes"""

    def add(self,image:Imgae):
        """Add two images"""

    def minus(self,image:Imgae):
        """Minus two images"""

    def split(self) -> List:
        """Split one 3-channels image to three 1-channel"""

    def merge(self,R:Image,G:Image,B:Image):
        """Merge three 1-channel image to 3-channels"""

class Converter(TinyObj):
    """The Converter class is used to 
    convert an image from one format to another."""

    @staticmethod
    def toRGB888(image: Image):
        """Convert the image to RGB888"""

    @staticmethod
    def toRGB565(image: Image):
        """Convert the image to RGB565"""

    @staticmethod
    def toGray(image: Image):
        """Convert the image to Gray"""

    @staticmethod
    def toBMP(image: Image):
        """Convert the image to BMP"""

    @staticmethod
    def toBGR888(image: Image):
        """Convert the image to BGR888"""


class Transforms(TinyObj):
    """The transforms class is used to 
    supply the rotate, flip, and crop operation for an image."""
    @staticmethod
    def rotateDown(image: Image):
        """Rotate the image """
    def threshold(image:Image,thre:int,maxval:int,thresholdType:int):
        """
        0:THRESH_BINARY 
        1:THRESH_BINARY_INV
        2:THRESH_TRUNC
        3:THRESH_TOZERO
        4:THRESH_TOZERO_INV
        5:OTSU
        """
    def setROI(image:Image,x:int,y:int,w:int,h:int) :
        """xywh"""
    def getOTSUthre(image:Image) -> int:
        """return otsu threshold"""
    def setOTSU(image:Image):
        """otsu"""
    def resize(image:Image,x:int,y:int,resizeType:int):
        """
        resize image
        0:NEAREST
        TODO:
        1:BILINEAR
        """

class Filter(TinyObj):
    """The Filter class is used to 
    supply some Image Filtering Algorithms ."""
    def meanFilter(image:Image,ksizex:int,ksizey:int):
        """ mean filter,ksize is odd"""