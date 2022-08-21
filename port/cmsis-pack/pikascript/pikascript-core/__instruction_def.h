/*
 * This file is part of the PikaScript project.
 * http://github.com/pikastech/pikascript
 *
 * MIT License
 *
 * Copyright (c) 2021 GorgonMeducer ?? embedded_zhuoran@hotmail.com
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

#undef def_ins

#if defined(__INS_ENUM)
#define def_ins(__INS_NAME) __INS_NAME,
#endif

#if defined(__INS_TABLE)
#define def_ins(__INS_NAME) [__INS_NAME] = &VM_instruction_handler_##__INS_NAME,
#endif

#if defined(__INS_COMPIRE)
#define def_ins(__INS_NAME)                             \
    if (0 == strncmp(ins_str, "" #__INS_NAME "", 3)) { \
        return __INS_NAME;                              \
    }
#endif

#if defined(__INS_GET_INS_STR)
#define def_ins(__INS_NAME)                              \
    if (__INS_NAME ==  instructUnit_getInstruct(self)){ \
        return ""#__INS_NAME""; \
    }
#endif

#undef __INS_ENUM
#undef __INS_TABLE
#undef __INS_COMPIRE
