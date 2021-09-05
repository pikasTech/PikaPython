#!/usr/bin/python
# -*- coding: utf-8 -*-

# *************************************************************************************************
#  Arm 2D project
#  @file        img2c.py
#  @brief       image to C-array converter
#
# *************************************************************************************************

import sys
from PIL import Image
import numpy as np
import time;



def usage():

    print("""
\033[4musage\033[0m : \033[31;1m img2c.py\033[00m <image> <output> <array_name> [resizeX] [resizeY]
""")
    print(' where : ')
    print(' image              : image file (png, bmp, etc..)')
    print(' output             : C file containing 2 arrays with RGB56 and alpha values')
    print(' array_name         : arrays prefix ')
    print(' resizeX  resizeY   : resized dimensions')
    exit(2)


hdr="""
/* Generated on {0} from {1} */
/* Re-sized : {2} */


#include "arm_2d.h"

"""

tail="""

extern const arm_2d_tile_t c_tile{0};
const arm_2d_tile_t c_tile{0} = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {2},
        }},
    }},
    .tInfo.bIsRoot = true,
    .phwBuffer = (uint16_t *)c_bmp{0},
}};

extern const arm_2d_tile_t c_tile{0}AlphaMask;
const arm_2d_tile_t c_tile{0}AlphaMask = {{
    .tRegion = {{
        .tSize = {{
            .iWidth = {1},
            .iHeight = {2},
        }},
    }},
    .tInfo = {{
        .bIsRoot = true,
        .bHasEnforcedColour = true,
        .tColourInfo = {{
            .chScheme = ARM_2D_COLOUR_BIN,
        }},
    }},
    .pchBuffer = (uint8_t *)c_bmp{0}Alpha,
}};
"""

def main(argv):

	if len(argv) < 3:
		usage()

	try:
		image = Image.open(argv[0])
	except FileNotFoundError:
		print("Cannot open image file %s" % (argv[0]))
		sys.exit(2)

	
	# resizing
	resized = False
	if len(argv) == 5:
		image = image.resize((int(argv[3]), int(argv[4])))
		resized = True


	(row, col) = image.size
	data = np.asarray(image)

	arr_name = argv[2]

	with open(argv[1],"w") as o:

		# insert header 		
		print(hdr.format(time.asctime( time.localtime(time.time())), argv[0], resized), file=o)		

		# alpha channel array
		print('static const uint8_t c_bmp%sAlpha[%d*%d] = {' % (arr_name, row, col),file=o)
		cnt = 0
		for eachRow in data:
			lineWidth=0
			print("/* -%d- */" % (cnt), file=o)
			for eachPix in eachRow:
				alpha = eachPix[3]
				if lineWidth % 16 == 15:
					print("0x%02x," %(alpha) ,file=o)
				else:
					print("0x%02x" %(alpha), end =", ",file=o)
				lineWidth+=1	
			cnt+=1	
			print('',file=o)
		print('};', file=o)	


		# RGB565 channel array
		R = (data[...,0]>>3).astype(np.uint16) << 11
		G = (data[...,1]>>2).astype(np.uint16) << 5
		B = (data[...,2]>>3).astype(np.uint16)
		# merge
		RGB565 = R | G | B

		print('',file=o)
		print('static const uint16_t c_bmp%s[%d*%d] = {' % (arr_name, row, col), file=o)
		cnt = 0
		for eachRow in RGB565:
			lineWidth=0
			print("/* -%d- */" % (cnt), file=o)
			for eachPix in eachRow:
				if lineWidth % 16 == 15:
					print("0x%04x," %(eachPix) ,file=o)
				else:
					print("0x%04x" %(eachPix), end =", ", file=o)
				lineWidth+=1	
			print('',file=o)
			cnt+=1
		print('};', file=o)	

		# insert tail		
		print(tail.format(arr_name, str(row), str(col)), file=o)	

if __name__ == '__main__':
    main(sys.argv[1:])	