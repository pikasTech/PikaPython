/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikascript
 *
 * MIT License
 *
 * Copyright (c) 2021 lyon 李昂 liang6516@outlook.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "BaseObj.h"
#include "PikaObj.h"
#include "TinyObj.h"
#include "dataMemory.h"
#include "dataString.h"
#include "dataStrs.h"

static void print_no_end(PikaObj* self, Args* args) {
    obj_setErrorCode(self, 0);
    char* res = args_print(args, "val");
    if (NULL == res) {
        obj_setSysOut(self, "[error] print: can not print val");
        obj_setErrorCode(self, 1);
        return;
    }
    /* not empty */
    if (strIsContain(res, '\\')) {
        res = strsReplace(args, res, "\\n", "\n");
        res = strsReplace(args, res, "\\r", "\r");
        res = strsReplace(args, res, "\\t", "\t");
    }
    __platform_printf("%s", res);
}

void Baseobj_print(PikaObj* self, Args* args) {
    obj_setErrorCode(self, 0);
    Arg* arg = args_getArg(args, "val");
    ArgType arg_type = arg_getType(arg);
    if (NULL != arg) {
        if (arg_getType(arg) == ARG_TYPE_BYTES) {
            arg_printBytes(arg);
            return;
        }
    }
    if (ARG_TYPE_OBJECT == arg_type) {
        char* to_str = obj_toStr(arg_getPtr(arg));
        if (NULL != to_str) {
            __platform_printf("%s\r\n", to_str);
            return;
        }
    }
    char* res = args_print(args, "val");
    if (NULL == res) {
        obj_setSysOut(self, "[error] print: can not print val");
        obj_setErrorCode(self, 1);
        return;
    }
    /* not empty */
    if (strIsContain(res, '\\')) {
        res = strsReplace(args, res, "\\n", "\n");
        res = strsReplace(args, res, "\\r", "\r");
        res = strsReplace(args, res, "\\t", "\t");
    }
    __platform_printf("%s\r\n", res);
}

PikaObj* New_BaseObj(Args* args) {
    PikaObj* self = New_TinyObj(args);
    class_defineMethod(self, "print(val:any)", Baseobj_print);
    class_defineMethod(self, "printNoEnd(val:any)", print_no_end);
    return self;
}
