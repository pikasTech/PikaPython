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

import os,sys
import numpy as np
import tensorflow as tf
import time, struct
from PIL import Image
try:
    from .tflite_reader import read_tflite
except:
    from tflite_reader import read_tflite


# constant
TM_MDL_INT8  = 0
TM_MDL_INT16 = 1
TM_MDL_FP32  = 2
TM_MDL_FP16  = 3
TM_MDL_FP8_143  = 4
TM_MDL_FP8_152  = 5

TM_FP8_143_BIAS = 9
TM_FP8_152_BIAS = 15


TML_CONV2D    = 0
TML_GAP       = 1
TML_FC        = 2
TML_SOFTMAX   = 3
TML_RESHAPE   = 4
TML_DWCONV2D  = 5

TM_PAD_VALID  = 0
TM_PAD_SAME   = 1

TM_ACT_NONE   = 0
TM_ACT_RELU   = 1

layername2type={\
    "CONV_2D"          :TML_CONV2D, 
    "MEAN"             :TML_GAP, 
    "FULLY_CONNECTED"  :TML_FC, 
    "SOFTMAX"          :TML_SOFTMAX,
    "RESHAPE"          :TML_RESHAPE,
    "DEPTHWISE_CONV_2D":TML_DWCONV2D,
}

MDLBINHEAD_SIZE=64
LAYERHEAD_SIZE= 48

#e is fp16 https://docs.python.org/3/library/struct.html
#fp8 https://arxiv.org/abs/2206.02915
#s.e.m and extra exponents bias

mdl_type = TM_MDL_INT8
unit_sizes= [1,2,4,2,1,1]
unit_size = unit_sizes[mdl_type]
w_types   = ["b","h","f","e","fp8_143","fp8_152"]  
w_type    = w_types[mdl_type]
b_types   = ["i","i","f","e","fp8_143","fp8_152"]
b_types_np= [np.int32,np.int32,np.float32,np.float16,np.uint8,np.uint8]
b_type    = b_types[mdl_type]
b_type_np = b_types_np[mdl_type]
bunit_sizes= [4,4,4,2,1,1]
bunit_size = bunit_sizes[mdl_type]


############################### UTILS FUNCTIONS #####################################
def is_mdl_float(mdl_type):
    return ((mdl_type == TM_MDL_FP32) or (mdl_type == TM_MDL_FP16) or (mdl_type == TM_MDL_FP8_143) or (mdl_type == TM_MDL_FP8_152))

def is_mdl_int(mdl_type):
    return ((mdl_type == TM_MDL_INT8) or (mdl_type == TM_MDL_INT16)) 

def align8(x):
    return (x+7)//8*8

def cal_buf_size(layers, mdl_type, out_deq):
    buf_sizes  = []
    global unit_sizes 
    unit_size  = unit_sizes[mdl_type]
    for l in layers:
        if l["is_output"] and out_deq and (mdl_type != TM_MDL_FP32) :  #fp16/fp8 also need deq
            buf_size = align8(np.prod(l["in_shape"])*unit_size)+align8(np.prod(l["out_shape"])*unit_size)+align8(np.prod(l["out_shape"])*4)
        elif (l["name"] == "SOFTMAX"): #reserve float place for deq or  middle
            buf_size = align8(np.prod(l["in_shape"])*unit_size)+align8(np.prod(l["out_shape"])*4)
        else:
            buf_size = align8(np.prod(l["in_shape"])*unit_size)+align8(np.prod(l["out_shape"])*unit_size)

        if (l["name"] == "RESHAPE"):
            buf_size -= align8(np.prod(l["in_shape"])*unit_size) #as reshape is inplace
        #print("%s: %d"%(l["name"], buf_size))
        buf_sizes.append(buf_size)
    buf_size = max(buf_sizes)
    #print(buf_size)
    return buf_size

#dims: 3,h,w,c; 2,1,w,c; 1,1,1,c 
def shape2dims(shape):
    dims = [len(shape)] + [1]*(3-len(shape))
    dims.extend(shape)
    return dims

############################### FP8 FUNCTIONS #####################################
# fp32:1.8.23_127, fp16: 1.5.10_15, bf16: 1.8.7_127
# fp8: 1.4.3, 1.5.2
def fp32_to_fp8(fp32, ecnt,mcnt,bias):
    buf = np.array(fp32).astype(np.float32).tobytes()
    data = np.frombuffer(buf, dtype=np.uint32)[0]
    fp32_s = data>>31
    fp32_e = (data>>23)&0x0ff
    fp32_m = (data&0x07fffff)
    #print("fp32 s.e.m=%d, %d, %d"%(fp32_s, fp32_e, fp32_m))
    fp8_s = fp32_s
    fp8_e = fp32_e-127+bias
    fp8_m = ((fp32_m>>(22-mcnt))+1)>>1
    if fp8_m>= (1<<mcnt):
        fp8_m=0
        fp8_e+=1
    if fp8_e > (1<<ecnt)-1:
        fp8_e = (1<<ecnt)-1
    elif fp8_e < 0:
        fp8_e = 0
    #print("fp8 s.e.m=%d, %d, %d"%(fp8_s, fp8_e, fp8_m))
    fp8 = int((fp8_s<<7)|(fp8_e<<mcnt)|fp8_m)
    return fp8

def fp32_to_fp8_batch(fp32s, ecnt,mcnt,bias):
    buf = fp32s.astype(np.float32).tobytes()
    datas = np.frombuffer(buf, dtype=np.uint32)
    fp8s = np.zeros(len(fp32s)).astype(np.uint8)
    for i in range(len(datas)):
        data = datas[i]
        fp32_s = data>>31
        fp32_e = (data>>23)&0x0ff
        fp32_m = (data&0x07fffff)
        #print("fp32 s.e.m=%d, %d, %d"%(fp32_s, fp32_e, fp32_m))
        fp8_s = fp32_s
        fp8_e = fp32_e-127+bias
        fp8_m = ((fp32_m>>(22-mcnt))+1)>>1
        if fp8_m>= (1<<mcnt):
            fp8_m=0
            fp8_e+=1
        if fp8_e > (1<<ecnt)-1:
            fp8_e = (1<<ecnt)-1
        elif fp8_e < 0:
            fp8_e = 0
        #print("fp8 s.e.m=%d, %d, %d"%(fp8_s, fp8_e, fp8_m))
        fp8 = (fp8_s<<7)|(fp8_e<<mcnt)|fp8_m
        fp8s[i] = fp8
    return fp8s

def fp8_to_fp32(fp8, ecnt,mcnt,bias):
    fp8_s = fp8>>7
    fp8_e = (fp8&0x7f)>>mcnt
    fp8_m = fp8&((1<<mcnt)-1)

    fp32_s = fp8_s
    fp32_e = fp8_e-bias+127
    fp32_m = fp8_m<<(23-mcnt)

    fp32 = (fp32_s<<31)|(fp32_e<<23)|fp32_m
    buf = np.array(fp32).astype(np.uint32).tobytes()
    fp32 = np.frombuffer(buf, dtype=np.float32)[0]
    return fp32

def fill_fp8_data(mdl_type, lbody, data):
    if mdl_type ==TM_MDL_FP8_143:
        fp8s = fp32_to_fp8_batch(data, 4,3,TM_FP8_143_BIAS)
        lbody += struct.pack("%dB"%(fp8s.size),  *fp8s)
    elif mdl_type ==TM_MDL_FP8_152:
        fp8s = fp32_to_fp8_batch(data, 5,2,TM_FP8_152_BIAS)
        lbody += struct.pack("%dB"%(fp8s.size),  *fp8s)
    else:
        print("err mdl_type!")
        assert 0
    return lbody


############################### PACK FUNCTIONS #####################################
def pack_conv2d_dwconv2d(l, mdl_type):  #conv2d and dwconv2d
    lbody = b''
    if is_mdl_int(mdl_type):
        ms = l["i_scale"]
        mzp= l["i_zeropoint"]
        _os= l["o_scale"]
        _ozp=l["o_zeropoint"]
    w = l["weight"].transpose(0,3,1,2).flatten()  #co,ci,h,w
    b = l["bias"].copy() if 'bias' in l else np.zeros((l["out_shape"][-1],))
    kw= l["weight"].shape[2]
    kh= l["weight"].shape[1]
    # fuse mzp to bias #need deal with pad==same
    if is_mdl_int(mdl_type):
        maxk = l["weight"].shape[1]*l["weight"].shape[2]
        mi_c = l["in_shape"][-1] if l["name"] == "CONV_2D" else 1
        mo_c = l["out_shape"][-1]
        tmp = np.array([np.sum(w[c*mi_c*maxk:(c+1)*mi_c*maxk]) for c in range(mo_c)])
        b += (-mzp*tmp)
    lbody += struct.pack('B',  kw);             #kernel_w
    lbody += struct.pack('B',  kh);             #kernel_h
    lbody += struct.pack('B',  l["stride_w"]);  #stride_w
    lbody += struct.pack('B',  l["stride_h"]);  #stride_h
    lbody += struct.pack('B',  l["dilation_w_factor"]);#dilation_w
    lbody += struct.pack('B',  l["dilation_h_factor"]);#dilation_h
    lbody += struct.pack('H',  l["fused_activation_function"]); #0 none, 1 relu, 2 relu1, 3 relu6, 4 tanh, 5 sign_bit
    # padding: 0,same; 1,valid
    kernel_extent_w = l["dilation_w_factor"] * (kw - 1) + 1;
    kernel_extent_h = l["dilation_h_factor"] * (kh - 1) + 1;
    if l["padding"] == 0: #same
        wpad = kernel_extent_w + (l["in_shape"][2] - 1) // l["stride_w"] * l["stride_w"] - l["in_shape"][2];
        hpad = kernel_extent_h + (l["in_shape"][1] - 1) // l["stride_h"] * l["stride_h"] - l["in_shape"][1];
        #print("%d,%d,%d; %d; %d,%d\n"%(kernel_extent_w, (l["in_shape"][2] - 1) // l["stride_w"] * l["stride_w"], l["in_shape"][3], wpad,wpad//2,wpad - wpad//2))
        assert (wpad>=0 and hpad>=0)
        print("    padding same(T,B,L,R): %d,%d,%d,%d"%(hpad//2, hpad - hpad//2, wpad//2, wpad - wpad//2))
        lbody += struct.pack('B',  int(hpad//2));       #top
        lbody += struct.pack('B',  int(hpad - hpad//2));#bottom
        lbody += struct.pack('B',  int(wpad//2));          #left
        lbody += struct.pack('B',  int(wpad - wpad//2));#right
    elif l["padding"] == 1: #valid
        lbody += struct.pack('I',  0)
        print("    padding valid")
    elif l["padding"] == 2: #valid & fuse pad
        last_pad = l["pad"]
        print("    Fusing PAD and CONV/DWCONV valid")
        lbody += struct.pack('BBBB', last_pad[0], last_pad[1], last_pad[2], last_pad[3])
        print("    padding (T,B,L,R): %d,%d,%d,%d"%(last_pad[0], last_pad[1], last_pad[2], last_pad[3]))
    else:                  #same  #top,bottom,left,right
        print("unsupport padding!")
        assert 0

    lbody += struct.pack('I',  0 if l["name"] == "CONV_2D" else l["depth_multiplier"]);
    lbody += struct.pack('I',  0);#pad
    # cal ws&w&b oft
    ws_oft = LAYERHEAD_SIZE + len(lbody) + 12  #add 4 uint32 oft
    ws_size= (mo_c*4+7)//8*8 if is_mdl_int(mdl_type) else 0
    w_oft  = ws_oft + ws_size
    w_size = (w.size*unit_size+7)//8*8
    b_oft  = w_oft+w_size
    b_size = (b.size*bunit_size+7)//8*8

    lbody += struct.pack('I',  ws_oft);   #ws_oft
    lbody += struct.pack('I',  w_oft);    #w_oft
    lbody += struct.pack('I',  b_oft);    #b_oft
    assert len(lbody)%8 == 0
    # weight scale
    if is_mdl_int(mdl_type):
        ws = l["w_scale"] 
        lbody += struct.pack("%df"%(ws.size), *ws)
        if ws_size!= ws.size*4:
            lbody += bytes(ws_size-ws.size*4) #align to 8bytes
        assert len(lbody)%8 == 0
    # weight
    if mdl_type == TM_MDL_INT8:
        lbody += w.astype(np.int8).tobytes()
    elif mdl_type == TM_MDL_INT16:
        print("INT16 TODO")
        assert 0
    elif mdl_type == TM_MDL_FP32:
        lbody += struct.pack("%df"%(w.size),  *w)
    elif mdl_type == TM_MDL_FP16:
        lbody += struct.pack("%de"%(w.size),  *w)
    elif mdl_type == TM_MDL_FP8_143:
        lbody = fill_fp8_data(mdl_type, lbody, w)
    elif mdl_type == TM_MDL_FP8_152:
        lbody = fill_fp8_data(mdl_type, lbody, w)
    else:
        print("unsupport mdl type %d"%mdl_type)
        assert 0
    if w_size!= w.size*unit_size:
        lbody += bytes(w_size-w.size*unit_size) #align to 8bytes
    assert len(lbody)%8 == 0
    #bias
    if (mdl_type == TM_MDL_FP8_143) or (mdl_type == TM_MDL_FP8_152):
        lbody = fill_fp8_data(mdl_type, lbody, b)
    else:
        lbody += struct.pack("%d"%(b.size)+b_type,  *b)
    if b_size!= b.size*bunit_size:
        lbody += bytes(b_size-b.size*bunit_size) #align to 8bytes
    assert len(lbody)%8 == 0
    return lbody

def pack_gap(l, mdl_type):
    if list(l["reduce_idx"]) != [0,1]:
        print("only support gap now")
        assert 0
    else:
        return b''

def pack_fc(l, mdl_type):
    lbody = b''
    if is_mdl_int(mdl_type):
        ms = l["i_scale"]
        mzp= l["i_zeropoint"]
        _os= l["o_scale"]
        _ozp=l["o_zeropoint"]
    mi_c = l["in_shape"][-1]
    mo_c = l["out_shape"][-1]
    w = l["weight"].flatten()  #co,ci
    b = l["bias"].copy() if 'bias' in l else np.zeros((mo_c,))
    if is_mdl_int(mdl_type):
        tmp = np.array([np.sum(w[c*mi_c:(c+1)*mi_c]) for c in range(mo_c)])
        b += (-mzp*tmp)
    # cal ws&w&b oft
    ws_oft = LAYERHEAD_SIZE + len(lbody) + 16  #add 4 uint32 oft
    ws_size= (mo_c*4+7)//8*8 if is_mdl_int(mdl_type) else 0
    w_oft  = ws_oft + ws_size
    w_size= (w.size*unit_size+7)//8*8
    b_oft = w_oft+w_size
    b_size= (b.size*bunit_size+7)//8*8

    lbody += struct.pack('I',  ws_oft);   #ws_oft
    lbody += struct.pack('I',  w_oft);    #w_oft
    lbody += struct.pack('I',  b_oft);    #b_oft
    lbody += struct.pack('I',  0);        #reserve, align 8
    assert len(lbody)%8 == 0

    # weight scale
    if is_mdl_int(mdl_type):
        ws = l["w_scale"] 
        lbody += struct.pack("%df"%(ws.size), *ws)
        if ws_size!= ws.size*4:
            lbody += bytes(ws_size-ws.size*4) #align to 8bytes
        assert len(lbody)%8 == 0
    # weight
    if mdl_type == TM_MDL_INT8:
        lbody += w.astype(np.int8).tobytes()
    elif mdl_type == TM_MDL_INT16:
        print("INT16 TODO")
        assert mdl_type!=TM_MDL_INT16
    elif mdl_type == TM_MDL_FP32:
        lbody += struct.pack("%df"%(w.size),  *w)
    elif mdl_type == TM_MDL_FP16:
        lbody += struct.pack("%de"%(w.size),  *w)
    elif mdl_type == TM_MDL_FP8_143:
        lbody = fill_fp8_data(mdl_type, lbody, w)
    elif mdl_type == TM_MDL_FP8_152:
        lbody = fill_fp8_data(mdl_type, lbody, w)
    else:
        print("unsupport mdl type %d"%mdl_type)
        assert 0
    if w_size!= w.size*unit_size:
        lbody += bytes(w_size-w.size*unit_size) #align to 8bytes
    assert len(lbody)%8 == 0
    #bias
    if (mdl_type == TM_MDL_FP8_143) or (mdl_type == TM_MDL_FP8_152):
        lbody = fill_fp8_data(mdl_type, lbody, b)
    else:
        lbody += struct.pack("%d"%(b.size)+b_type,  *b)
    if b_size!= b.size*bunit_size:
        lbody += bytes(b_size-b.size*bunit_size) #align to 8bytes

    assert len(lbody)%8 == 0
    return lbody

def pack_softmax(l, mdl_type):
    return b''

def pack_reshape(l, mdl_type):
    return b''

############################### PACK FUNCTIONS #####################################
def pack_tmdl(layers, mdl_name, mdl_type, out_deq, in_dims, out_dims, write_c_header=True):
    global unit_size,w_type,b_type,b_type_np,bunit_size
    #mdl_name = "mnist.tmodel"
    fw = open(mdl_name, "wb")
    if is_mdl_float(mdl_type) and layers[0]["quant"]:
        print("quant type unmatch with tflite type!")
        exit()
    elif is_mdl_int(mdl_type) and not layers[0]["quant"]:
        print("quant type unmatch with tflite type!")
        exit()
    # cfg
    #mdl_type  = quant_type
    #out_deq   = 1  #output do dequant
    input_cnt = 1
    output_cnt= 1
    layer_cnt = len(layers) 
    buf_size  = cal_buf_size(layers, mdl_type, out_deq)
    sub_size  = 0
    in_dims   = shape2dims(in_dims)
    out_dims  = shape2dims(out_dims)

    unit_size = unit_sizes[mdl_type]
    w_type    = w_types[mdl_type]
    b_type    = b_types[mdl_type]
    b_type_np = b_types_np[mdl_type]
    bunit_size= bunit_sizes[mdl_type]

    # head
    print("================ pack model head ================")
    model_size = 0
    head  = b'MAIX'
    head += struct.pack('B',  mdl_type);  print("mdl_type   =%d"%mdl_type)
    head += struct.pack('B',  out_deq);   print("out_deq    =%d"%out_deq)
    head += struct.pack('H',  input_cnt); print("input_cnt  =%d"%input_cnt)
    head += struct.pack('H',  output_cnt);print("output_cnt =%d"%output_cnt)
    head += struct.pack('H',  layer_cnt); print("layer_cnt  =%d"%layer_cnt)
    head += struct.pack('I',  buf_size);  print("buf_size   =%d"%buf_size)
    head += struct.pack('I',  sub_size);  print("sub_size   =%d"%sub_size)
    head += struct.pack('4H', *in_dims);  print("in_dims    =",in_dims)
    head += struct.pack('4H', *out_dims); print("out_dims   =",out_dims)
    head += bytes(28)
    #print(head)
    assert len(head)%8 == 0   #print("head not align on 8byte!")
    fw.write(head)
    model_size += len(head)

    # layers
    print("================   pack layers   ================")
    out_oft = 0
    out_flag = 0
    out_size = np.prod(in_dims[1:])*unit_size
    layer_sizes = []
    for index in range(len(layers)):
        l  = layers[index]
        print(l["name"])
        assert l["name"] in layername2type  #print("layertype not support!")
        tmp = l["in_shape"][1:]; in_dims = [len(tmp)] + [1]*(3-len(tmp)); in_dims.extend(tmp)
        tmp = l["out_shape"][1:]; out_dims = [len(tmp)] + [1]*(3-len(tmp)); out_dims.extend(tmp)
        print("   ",in_dims, out_dims)

        # layer head
        layer_type = layername2type[l["name"]]
        if l["is_output"]:
            if out_flag == 0:
                print("    OUTPUT!"); out_flag = 1
            else:
                print("only support 1 output")
                assert 0

        layer_size = 0  #dummy
        in_size = out_size
        if (mdl_type != TM_MDL_FP32 and l["is_output"] and out_deq):  #reserve float place for deq #fp16 also need "deq"
            out_size = align8(np.prod(out_dims[1:])*unit_size) + align8(np.prod(out_dims[1:])*4)
        elif (l["name"] == "SOFTMAX"):    #reserve float place for middle
            out_size = align8(np.prod(out_dims[1:])*4)
        else:
            out_size = align8(np.prod(out_dims[1:])*unit_size)

        in_oft     = out_oft 
        if (l["name"] == "RESHAPE"): # inplace
            out_oft= in_oft
        else:
            out_oft    = 0 if out_oft != 0 else buf_size-out_size
        print("    in_oft:%d, size:%d;  out_oft:%d, size:%d"%(in_oft, in_size, out_oft, out_size))
        if mdl_type == TM_MDL_INT8 or mdl_type == TM_MDL_INT16:
            in_s  = l["i_scale"]; in_zp = int(l["i_zeropoint"])
            out_s = l["o_scale"]; out_zp= int(l["o_zeropoint"])
        else:
            in_s  = 1; in_zp = 0;
            out_s = 1; out_zp= 0;

        lh = bytearray(0)
        lh += struct.pack('H',  layer_type);
        lh += struct.pack('H',  l["is_output"]);
        lh += struct.pack('I',  layer_size);
        lh += struct.pack('I',  in_oft);
        lh += struct.pack('I',  out_oft);
        lh += struct.pack('4H', *in_dims);
        lh += struct.pack('4H', *out_dims);
        lh += struct.pack('f',  in_s);  #TODO: fast scale
        lh += struct.pack('f' if is_mdl_float(mdl_type) else 'i',  in_zp)
        lh += struct.pack('f',  out_s);
        lh += struct.pack('f' if is_mdl_float(mdl_type) else 'i',  out_zp); 
        #print(lh)
        assert len(lh) == LAYERHEAD_SIZE   #print("layer head not align on 8byte!")

        # layer body
        if l["name"] == "CONV_2D" or l["name"] == "DEPTHWISE_CONV_2D":
            lbody = pack_conv2d_dwconv2d(l, mdl_type)
        elif l["name"] == "MEAN":
            lbody = pack_gap(l, mdl_type)
        elif l["name"] == "FULLY_CONNECTED":
            lbody = pack_fc(l, mdl_type)
        elif l["name"] == "SOFTMAX":
            lbody = pack_softmax(l, mdl_type)
        elif l["name"] == "RESHAPE":
            lbody = pack_reshape(l, mdl_type)
        else:
            print("unsupport layer type %s"%l["name"])
            assert 0

        # fill layer size
        layer_size = len(lh)+len(lbody)
        lh[4:8] = struct.pack("I", layer_size)
        print("    layer_size=%d"%layer_size)
        layer_sizes.append(layer_size)
        #write to file
        fw.write(lh)
        fw.write(lbody)
        model_size += (len(lh)+len(lbody))

    print("================    pack done!   ================")
    fw.close()
    # write c header file
    if write_c_header:
        hmdl = ".".join(mdl_name.split(".")[:-1])+".h"
        fr=open(mdl_name, "rb")
        data = fr.read()
        fr.close()
        with open(hmdl, "w", encoding="utf-8") as fw:
            fw.writelines("#ifndef __MODEL_FILE__H\r\n")
            fw.writelines("#define __MODEL_FILE__H\r\n\r\n")
            fw.writelines("#include <stdint.h>\r\n")
            fw.writelines("#define MDL_BUF_LEN (%d)\r\n"%buf_size)
            fw.writelines("#define LBUF_LEN (%d)\r\n"%(MDLBINHEAD_SIZE+max(layer_sizes)))
            fw.writelines("const uint8_t mdl_data[%d]={\\\r\n\t"%(len(data)))
            for i in range(len(data)):
                fw.writelines("0x%02x, "%(data[i]))
                if i%16 == 15:
                    fw.writelines("\r\n\t")
            fw.writelines("\r};\r\n")
            fw.writelines("\r\n#endif\r\n")

    print("    model  size %.1fKB (%d B) FLASH"%(model_size/1024, model_size))
    print("    buffer size %.1fKB (%d B) RAM"%(buf_size/1024, buf_size))
    print("    single layer mode subbuff size %.1fKB (%d+%d=%d B) RAM"%\
        ((MDLBINHEAD_SIZE+max(layer_sizes))/1024, MDLBINHEAD_SIZE, max(layer_sizes), MDLBINHEAD_SIZE+max(layer_sizes)))
    print("Saved to tinymaix model to %s" % mdl_name)
    if write_c_header:
        print("Saved to tinymaix model header to %s" % hmdl)
    #!ls -lh $mdl_name

def tflite2tmdl(tflite_name, tmdl_name, mdl_type, out_deq, in_dims, out_dims, write_c_header=True):
    layers = read_tflite(tflite_name)
    pack_tmdl(layers, tmdl_name, mdl_type, out_deq, in_dims, out_dims, write_c_header=write_c_header)

def print_usage():
    print("Usage: python3 tflite2tmdl.py tflite_name tmdl_name mdl_type out_deq in_dims out_dims")
    print("       mdl_type: fp32, int8, int16, fp16, fp8_143, fp8_152")
    print("       out_deq: if enable output dequant")
    print("       in_dims,out_dims: dims except batch dim, max 3dims")
    print("       currently only support single input/output convert")


# python3 tflite2tmdl.py tflite/mnist_dw_f.tflite tmdl/mnist_dw_fp16.tmdl fp16 1 28,28,1 10
# python3 tflite2tmdl.py tflite/mnist_dw_q.tflite tmdl/mnist_dw_q.tmdl int8 1 28,28,1 10
# python3 tflite2tmdl.py tflite/mbnet_f.tflite tmdl/mbnet_f.tmdl fp32 1 128,128,3 1000
# python3 tflite2tmdl.py tflite/mbnet_q.tflite tmdl/mbnet_q.tmdl int8 1 128,128,3 1000
# python3 tflite2tmdl.py tflite/mbnet_f.tflite tmdl/mbnet_fp16.tmdl fp16 1 128,128,3 1000
# python3 tflite2tmdl.py tflite/mnist_valid_f.tflite tmdl/mbnet_fp8.tmdl fp8_152 1 28,28,1 10
# python3 tflite2tmdl.py tflite/mbnet_f.tflite tmdl/mbnet_fp8.tmdl fp8_152 1 128,128,3 1000
mdl_type_dict={"fp32":TM_MDL_FP32, "int8":TM_MDL_INT8, "int16":TM_MDL_INT16, "fp16":TM_MDL_FP16, "fp8_143":TM_MDL_FP8_143, "fp8_152":TM_MDL_FP8_152}
if __name__ == '__main__':
    if len(sys.argv) != 7:
        print_usage()
        exit()

    tflite_name = sys.argv[1]
    tmdl_name   = sys.argv[2]
    mdl_type    = mdl_type_dict[sys.argv[3]]
    out_deq     = int(sys.argv[4])
    in_dims     = sys.argv[5]
    out_dims    = sys.argv[6]

    in_dims  = in_dims.split(",")
    in_dims  = [int(i) for i in in_dims]
    out_dims = out_dims.split(",")
    out_dims = [int(i) for i in out_dims]
    tflite2tmdl(tflite_name, tmdl_name, mdl_type, out_deq, in_dims, out_dims)



