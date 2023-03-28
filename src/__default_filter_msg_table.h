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

#include "__default_filter_msg_def.h"
#if defined(__clang__)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wunknown-warning-option"
#   pragma clang diagnostic ignored "-Winitializer-overrides"
#endif

/* add your own message filter here with syntax:
 *
 *      add_filter_msg(
 *          <name>,
 *          <string>,
 *          [.is_visible = PIKA_TRUE,]
 *          [.is_case_insensitive = PIKA_TRUE,] 
 *          [.ignore_mask = mask value,]
 *          [.target = your own object address/value,]
 *      )
 */

add_filter_msg(hi_pika,     "###Hi Pika")
add_filter_msg(bye_pika,    "###bye pika", .is_case_insensitive = PIKA_TRUE)

/* add your own message item here with syntax:
 *
 *      add_filter_item(
 *          <name>,
 *          .message = (const uint8_t []){< num0, num1, ... >},
 *          .size = <array size>,
 *          [.is_visible = PIKA_TRUE,]
 *          [.is_case_insensitive = PIKA_TRUE,] 
 *          [.ignore_mask = mask value,]
 *          [.target = your own object address/value,]
 *      )
 * 
 * for example:
 * 
 *      add_filter_item(
 *              example_array,
 *              .message = (const uint8_t []){'a','b','c'},
 *              .size = 3,
 *              __NO_FILTER_HANLDER__,
 *          )
 */

#if defined(__clang__)
#   pragma clang diagnostic pop
#endif

