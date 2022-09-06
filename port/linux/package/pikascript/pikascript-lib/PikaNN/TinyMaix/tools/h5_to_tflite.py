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
import time
from PIL import Image

from os import environ
# environ['CUDA_VISIBLE_DEVICES'] = '0'

# norm_type: "0to1", "n1to1"
def h5_to_tflite(h5_name, tflite_name, is_quant, quant_dir, norm_type = None, mean = 0.0, std = 0.0):
    def representative_data_gen():
        files = os.listdir(quant_dir)
        valid_files = []
        valid_format = [".jpg", ".jpeg", ".png", ".bmp", ".ppm", ".pgm"]
        for name in files:
            ext = os.path.splitext(name)[1].lower()
            if ext not in valid_format:
                continue
            valid_files.append(os.path.join(quant_dir, name))
        if len(valid_files) == 0:
            raise Exception("No valid files in quant_input dir {}, support format: ".format(quant_dir, valid_format))
        for path in valid_files:
            img = Image.open(path)
            img = np.array(img).astype(np.float32)
            shape = img.shape
            if len(shape) == 2:
                shape = (1, shape[0], shape[1], 1)
            else:
                shape = (1, shape[0], shape[1], shape[2])
            img = img.reshape(shape)
            if norm_type is not None:
                if norm_type == "0to1":
                    img = img/255.0
                elif norm_type == "n1to1":
                    img = (img-128)/128
                else:
                    raise Exception("Unsupported norm_type: {}".format(norm_type))
            else:
                img = (img - mean) / std
            yield [img]

    if is_quant==0:
        tf.compat.v1.disable_eager_execution()
        converter = tf.compat.v1.lite.TFLiteConverter.from_keras_model_file(h5_name)
        tflite_model = converter.convert()
        open(tflite_name, "wb").write(tflite_model)  
        print("Done")
    else:
        quant_type = tf.int8  #tf2 only support int8 quant
        converter = tf.compat.v1.lite.TFLiteConverter.from_keras_model_file(h5_name)
        converter._experimental_disable_per_channel = False #True
        converter.optimizations = [tf.lite.Optimize.DEFAULT]
        converter.representative_dataset = representative_data_gen
        # Ensure that if any ops can't be quantized, the converter throws an error
        converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS_INT8]
        # Set the input and output tensors to int8 (APIs added in r2.3)
        converter.inference_input_type = quant_type
        converter.inference_output_type = quant_type

        start_time = time.time()
        tflite_model_quant = converter.convert()
        used_time = time.time() - start_time

        open(tflite_name, "wb").write(tflite_model_quant)
        print('Done, quant used time:{}'.format(used_time))


def print_usage():
    print("Usage: python3 h5_to_tflite.py h5_name tflite_name is_quant quant_dir norm_type")
    print("       norm_type: 0to1, n1to1")


# python3 h5_to_tflite.py h5/mnist_dw.h5 tflite/mnist_dw_f.tflite 0
# python3 h5_to_tflite.py h5/mnist_dw.h5 tflite/mnist_dw_q.tflite 1 quant_img_mnist/ 0to1
# python3 h5_to_tflite.py h5/mbnet96_0.125.h5 tflite/mbnet96_0.125_q.tflite 1 quant_img96/ 0to1
if __name__ == '__main__':
    if len(sys.argv) != 4 and len(sys.argv) != 6:
        print_usage()
        exit()

    h5_name     = sys.argv[1]
    tflite_name = sys.argv[2]
    is_quant    = int(sys.argv[3])
    quant_dir   = None
    norm_type   = None
    if is_quant == 1:
        if len(sys.argv) != 6:
            print_usage()
            exit()
        quant_dir   = sys.argv[4]
        norm_type   = sys.argv[5]
    h5_to_tflite(h5_name, tflite_name, is_quant, quant_dir, norm_type = norm_type)

