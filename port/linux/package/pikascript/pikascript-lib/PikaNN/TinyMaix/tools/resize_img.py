# Copyright 2022 Sipeed Technology Co., Ltd. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ==============================================================================

from PIL import Image
import os,sys
import numpy as np

def mkdir(path):
    path = path.strip()
    path = path.rstrip("\\")
    isExists = os.path.exists(path)
    if not isExists:
        os.makedirs(path)
        return True
    else:
        return False

def print_usage():
    print("Usage: python3 resize_img.py in_dir out_dir size")
    print("       size: 224,224,3")

# python3 resize_img.py imagenet quant_imagenet 224,224,3
if __name__ == '__main__':
    if len(sys.argv) != 4:
        print_usage()
        exit()

    in_dir  = sys.argv[1]
    out_dir = sys.argv[2]
    size    = sys.argv[3]
    size    = size.split(",")
    size    = [int(i) for i in size]
    dst_h   = size[0]
    dst_w   = size[1]
    dst_ch  = size[2]
    
    files = os.listdir(in_dir)
    mkdir(out_dir)

    for f in files:
        if f[-4:] != ".jpg" and f[-4:] != ".png" and f[-4:] != ".bmp" and f[-4:] != ".ppm" and f[-4:] != ".pgm" and f[-5:] != ".JPEG":
            continue 
        f1    = in_dir+"/"+f  
        img   = Image.open(f1)
        src_w = img.size[0]
        src_h = img.size[1]
        src_ch= img.layers
        #if dst_w/dst_h > src_w/src_h:
        img   = img.resize((dst_w, dst_h), Image.ANTIALIAS)
        if dst_ch != src_ch:
            if dst_ch==3 and src_ch==1:
                img0 = np.array(img)
                img1 = np.zeros((dst_h,dst_w,dst_ch))
                img1[:,:,0]=img0;img1[:,:,1]=img0;img1[:,:,2]=img0
                img = Image.fromarray(img1.astype(np.uint8))
            elif dst_ch==1 and src_ch==3:
                img = np.array(img)
                img = img.mean(axis=2)
                img = Image.fromarray(img.astype(np.uint8))
            else:
                print("can't support ch %d -> %d"%(src_ch, dst_ch))
                exit()
        img.save(out_dir+"/"+f.split(".")[-2]+".jpg")
        print(out_dir+"/"+f.split(".")[-2]+".jpg")
    print("Done!")





