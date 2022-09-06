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

import numpy as np
from keras.datasets import mnist
from tensorflow.python.keras.backend import set_session
from tensorflow.python.keras.models import load_model
from tensorflow.keras.models import Model, load_model, Sequential
from tensorflow.keras.layers import Conv2D, Dense, MaxPooling2D, Softmax, Activation, BatchNormalization, Flatten, Dropout, DepthwiseConv2D
from tensorflow.keras.layers import MaxPool2D, AvgPool2D, AveragePooling2D, GlobalAveragePooling2D,ZeroPadding2D,Input,Embedding,PReLU
from keras.callbacks import ModelCheckpoint
from keras.callbacks import TensorBoard
from keras.utils import np_utils
from keras.preprocessing.image import ImageDataGenerator
import keras.backend as K
import tensorflow as tf
import time

#/tensorflow/lite/tools/visualize.py
import re
from tensorflow.lite.python import schema_py_generated as schema_fb
 
def BuiltinCodeToName(code):
    """Converts a builtin op code enum to a readable name."""
    for name, value in schema_fb.BuiltinOperator.__dict__.items():
        if value == code:
            return name
    return None
def CamelCaseToSnakeCase(camel_case_input):
    """Converts an identifier in CamelCase to snake_case."""
    s1 = re.sub("(.)([A-Z][a-z]+)", r"\1_\2", camel_case_input)
    return re.sub("([a-z0-9])([A-Z])", r"\1_\2", s1).lower()
def FlatbufferToDict(fb, preserve_as_numpy):
    if isinstance(fb, int) or isinstance(fb, float) or isinstance(fb, str):
        return fb
    elif hasattr(fb, "__dict__"):
        result = {}
        for attribute_name in dir(fb):
            attribute = fb.__getattribute__(attribute_name)
            if not callable(attribute) and attribute_name[0] != "_":
                snake_name = CamelCaseToSnakeCase(attribute_name)
                preserve = True if attribute_name == "buffers" else preserve_as_numpy
                result[snake_name] = FlatbufferToDict(attribute, preserve)
        return result
    elif isinstance(fb, np.ndarray):
        return fb if preserve_as_numpy else fb.tolist()
    elif hasattr(fb, "__len__"):
        return [FlatbufferToDict(entry, preserve_as_numpy) for entry in fb]
    else:
        return fb
def CreateDictFromFlatbuffer(buffer_data):
    model_obj = schema_fb.Model.GetRootAsModel(buffer_data, 0)
    model = schema_fb.ModelT.InitFromObj(model_obj)
    return FlatbufferToDict(model, preserve_as_numpy=False)


def read_tflite(tflite_name, log_func=print):
    layers = []
    # Read the model.
    with open(tflite_name, 'rb') as f:
        model_buffer = f.read()

    interpreter = tf.lite.Interpreter(model_content=model_buffer)
    interpreter.allocate_tensors()

    data = CreateDictFromFlatbuffer(model_buffer)
    op_codes = data['operator_codes']
    subg = data['subgraphs'][0]
    tensors = subg['tensors']          #weight, bias here
    input_idxs = subg['inputs']
    output_idxs = subg['outputs']

    # convert name to utf readable
    for i in range(len(tensors)):
        tmp = tensors[i]["name"]
        tmp = bytearray(tmp)
        tensors[i]["name"] = tmp.decode('utf-8')

    # export layer param
    last_pad = None
    for idx in range(len(subg['operators'])):
        layer = subg['operators'][idx]
        l={}
        #layer name
        op_idx = layer['opcode_index']
        op_code = op_codes[op_idx]['builtin_code']
        layer_name = BuiltinCodeToName(op_code) 
        l["name"]=layer_name
        log_func(layer_name)

        #layer param
        layer_param = layer['builtin_options']
        log_func(layer_param)
        if layer_param is not None:
            l.update(layer_param)

        #layer input/output idx
        input_tensor_idx = layer['inputs']
        output_tensor_idx = layer['outputs']

        if len(output_tensor_idx)>1:
            raise Exception("Not support multi output yet")

        if output_tensor_idx[0] in output_idxs:
            l.update({"is_output":1})
            log_func("OUTPUT!")
        else:
            l.update({"is_output":0})

        #input
        input_idx = input_tensor_idx[0]
        if last_pad == None:
            l.update({"in_shape":tensors[input_idx]["shape"]})
        else:
            shape = tensors[input_idx]["shape"]
            shape[1] = shape[1] - last_pad[0] - last_pad[1]
            shape[2] = shape[2] - last_pad[2] - last_pad[3]
            l.update({"in_shape":shape})
        l.update({"in_name":tensors[input_idx]["name"]})
        log_func("    input: %s"%(tensors[input_idx]["name"]))
        if tensors[input_idx]["quantization"]['scale'] is not None:
            l.update({"i_scale":tensors[input_idx]['quantization' ]['scale'][0]})
            l.update({"i_zeropoint":tensors[input_idx]['quantization' ]['zero_point'][0]})
            l.update({"quant":1})
        else:
            l.update({"i_scale":1})
            l.update({"i_zeropoint":0})
            l.update({"quant":0})
        #output
        output_idx = output_tensor_idx[0]
        l.update({"out_shape":tensors[output_idx]["shape"]})
        l.update({"out_name":tensors[output_idx]["name"]})
        if tensors[output_idx]["quantization"]['scale'] is not None:
            l.update({"o_scale":tensors[output_idx]['quantization' ]['scale'][0]})
            l.update({"o_zeropoint":tensors[output_idx]['quantization' ]['zero_point'][0]})
        else:
            l.update({"o_scale":1})
            l.update({"o_zeropoint":0})

        if layer_name == "CONV_2D" or layer_name == "DEPTHWISE_CONV_2D":
            #filter weight
            weight_idx = input_tensor_idx[1]
            weight = interpreter.get_tensor(weight_idx) #用interpreter获取具体的权重数值
            filters = tensors[weight_idx]['shape'] #卷积核尺寸
            log_func("    filter %d: %s "%(weight_idx, tensors[weight_idx]["name"]))
            #print(weight.shape, filters)
            #print(tensors[weight_idx])
            l.update({"weight":weight})
            if tensors[weight_idx]["quantization"]['scale'] is not None:
                l.update({"w_scale":np.array(tensors[weight_idx]['quantization' ]['scale'])})
                l.update({"w_zeropoint":np.array(tensors[weight_idx]['quantization' ]['zero_point'])})
            else:
                l.update({"w_scale":1})
                l.update({"w_zeropoint":0})
            #filter bias
            bias_idx = input_tensor_idx[2]
            bias = interpreter.get_tensor(bias_idx)
            log_func("    bias %d: %s"%(bias_idx,tensors[bias_idx]["name"]))
            l.update({"bias":bias})
            if tensors[bias_idx]["quantization"]['scale'] is not None:
                l.update({"b_scale":np.array(tensors[bias_idx]['quantization' ]['scale'])})
                l.update({"b_zeropoint":np.array(tensors[bias_idx]['quantization' ]['zero_point'])})
            else:
                l.update({"b_scale":1})
                l.update({"b_zeropoint":0})
            #fuse pad
            if last_pad != None:
                l.update({"padding":2})
                l.update({"pad":last_pad})
        elif layer_name == "MEAN":
            data_idx = input_tensor_idx[1]
            log_func("    data: %s"%(tensors[data_idx]["name"]))
            reduce_idx = interpreter.get_tensor(data_idx)
            l.update({"reduce_idx":reduce_idx-1})
        elif layer_name == "FULLY_CONNECTED":
            weight_idx = input_tensor_idx[1]
            log_func("    weight: %s"%(tensors[weight_idx]["name"]))
            weight = interpreter.get_tensor(weight_idx)
            l.update({"weight":weight})
            if tensors[weight_idx]["quantization"]['scale'] is not None:
                l.update({"w_scale":np.array(tensors[weight_idx]['quantization' ]['scale'])})
                l.update({"w_zeropoint":np.array(tensors[weight_idx]['quantization' ]['zero_point'])})
            else:
                l.update({"w_scale":1})
                l.update({"w_zeropoint":0})
            bias_idx = input_tensor_idx[2]
            if bias_idx >= 0:
                log_func("    bias: %s"%(tensors[bias_idx]["name"]))
                bias = interpreter.get_tensor(bias_idx)
                l.update({"bias":bias})
                if tensors[bias_idx]["quantization"]['scale'] is not None:
                    l.update({"b_scale":np.array(tensors[bias_idx]['quantization' ]['scale'])})
                    l.update({"b_zeropoint":np.array(tensors[bias_idx]['quantization' ]['zero_point'])})
                else:
                    l.update({"b_scale":1})
                    l.update({"b_zeropoint":0})
        elif layer_name == "SOFTMAX":
            log_func("    softmax no param")
        elif layer_name == "RESHAPE":
            log_func("    reshape no param")
        elif layer_name == "PAD":
            log_func("    Dirty deal with PAD")
            layer_next = subg['operators'][idx+1]
            op_idx = layer_next['opcode_index']
            op_code = op_codes[op_idx]['builtin_code']
            layer_next_name = BuiltinCodeToName(op_code) 
            if layer_next_name == "CONV_2D" or layer_next_name == "DEPTHWISE_CONV_2D": 
                if layer_next['builtin_options']["padding"] == 1:  #valid
                    #print(layer_next)
                    #print(len(input_tensor_idx))
                    pad_idx = input_tensor_idx[1]
                    log_func("    pad: %s"%(tensors[pad_idx]["name"]))
                    pad = interpreter.get_tensor(pad_idx)
                    #print(pad)
                    assert pad[0,0]==0 and pad[0,1]==0 and pad[3,0]==0 and pad[3,1]==0
                    #l.update({"pad":[pad[1][0], pad[1][1], pad[2][0], pad[2][1]]})
                    last_pad = [pad[1][0], pad[1][1], pad[2][0], pad[2][1]]
                    continue
                else:
                    raise Exception("only deal with pad+conv_valid")
            else:
                raise Exception("only deal with pad+conv/dwconv")
        elif layer_name in ["SHAPE", "STRIDED_SLICE", "PACK"]:
            log_func("    ignore %s" % layer_name)
            continue
        elif layer_name == "QUANTIZE":
            raise Exception("QUANTIZE not supported, maybe tflite is not quantized model, check your tflite model")
        else:
            raise Exception("Not support layer %s"%layer_name)
        layers.append(l)
        last_pad = None
    return layers


