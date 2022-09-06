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

def print_usage():
    print("Usage: python3 tmdl2header.py tmdl_name")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print_usage()
        exit()

    mdl_name  = sys.argv[1]
    hmdl = ".".join(mdl_name.split(".")[:-1])+".h"
    fr=open(mdl_name, "rb")
    fw=open(hmdl, "w")
    data = fr.read()
    fw.writelines("#include <stdint.h>\r\n")
    fw.writelines("#define MDL_BUF_LEN (%d)\r\n"%buf_size)
    fw.writelines("#define LBUF_LEN (%d)\r\n"%(MDLBINHEAD_SIZE+max(layer_sizes)))
    fw.writelines("const uint8_t mdl_data[%d]={\\\r\n\t"%(len(data)))
    for i in range(len(data)):
        fw.writelines("0x%02x, "%(data[i]))
        if i%16 == 15:
            fw.writelines("\r\n\t")
    fw.writelines("\r};\r\n")
    fr.close()
    fw.close()
    print("Done!")





