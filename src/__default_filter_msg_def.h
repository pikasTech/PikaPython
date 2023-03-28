/*
 * This file is part of the PikaPython project.
 * http://github.com/pikastech/pikapython
 *
 * MIT License
 *
 * Copyright (c) 2023 GorgonMeducer ?? embedded_zhuoran@hotmail.com
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
#undef add_filter_msg
#undef add_filter_item
#undef __add_filter_msg
#undef __add_filter_item
#undef __NO_FILTER_HANLDER__


#if defined(__MSG_TABLE)
    #define __add_filter_msg(__name, __msg, ...)                                \
        {                                                                       \
            .message = (const uint8_t []){__msg},                               \
            .size = sizeof((const uint8_t []){__msg}) - 1,                      \
            .handler = _filter_msg_##__name##_handler,                          \
            __VA_ARGS__                                                         \
        },
    #define __add_filter_item(__name, ...)                                      \
        {                                                                       \
            .handler = _filter_msg_##__name##_handler,                          \
            __VA_ARGS__                                                         \
        },
#endif

#if defined(__MSG_DECLARE)
    #define __add_filter_msg(__name, __msg, ...)                                \
            PIKA_BOOL _filter_msg_##__name##_handler(   FilterItem *msg,        \
                                                        PikaObj* self,          \
                                                        ShellConfig* shell);
    #define __add_filter_item(__name, ...)                                      \
            PIKA_BOOL _filter_msg_##__name##_handler(   FilterItem *msg,        \
                                                        PikaObj* self,          \
                                                        ShellConfig* shell);
#endif

#undef __MSG_TABLE
#undef __MSG_DECLARE


#define add_filter_msg(__name, __msg, ...)                                      \
            __add_filter_msg(__name, __msg, ##__VA_ARGS__)
#define add_filter_item(__name, ...)                                            \
            __add_filter_item(__name, ##__VA_ARGS__)
#define __NO_FILTER_HANLDER__   .handler = NULL
