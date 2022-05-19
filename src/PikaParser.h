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

#ifndef __PIKA_PARSER__H
#define __PIKA_PARSER__H
#include "PikaVM.h"
#include "dataQueueObj.h"
#include "dataStack.h"

/*! \NOTE: Make sure #include "plooc_class.h" is close to the class definition
 */
//#define __PLOOC_CLASS_USE_STRICT_TEMPLATE__

#if defined(__PIKA_PARSER_CLASS_IMPLEMENT__)
#define __PLOOC_CLASS_IMPLEMENT__
#elif defined(__PIKA_PARSER_CLASS_INHERIT__)
#define __PLOOC_CLASS_INHERIT__
#endif

#include "__pika_ooc.h"

enum TokenType {
    TOKEN_strEnd = 0,
    TOKEN_symbol,
    TOKEN_keyword,
    TOKEN_operator,
    TOKEN_devider,
    TOKEN_literal,
};

enum StmtType {
    STMT_reference,
    STMT_string,
    STMT_bytes,
    STMT_number,
    STMT_method,
    STMT_operator,
    STMT_import,
    STMT_list,
    STMT_none,
};

dcl_class(Asmer);

def_class(Asmer, 
    private_member(
        char* asm_code;
        uint8_t block_deepth_now;
        uint8_t is_new_line;
        char* line_pointer;
    )
);

dcl_class(LexToken);

def_class(LexToken, 
    private_member(
        char* token;
        enum TokenType type;
        char* pyload;
    )
);

dcl_class(ParserState);

def_class(ParserState, 
    private_member(
        char* tokens;
        uint16_t length;
        uint16_t iter_index;
        uint8_t branket_deepth;
        struct LexToken token1;
        struct LexToken token2;
        Arg* last_token;
        Args* iter_buffs;
        Args* buffs_p;
    )
);

char* Parser_multiLineToAsm(Args* outBuffs, char* multiLine);
char* instructUnit_fromAsmLine(Args* outBuffs, char* pikaAsm);
char* Parser_byteCodeToAsm(Args* outBuffs, char* pikaByteCode);
ByteCodeFrame* byteCodeFrame_appendFromAsm(ByteCodeFrame* bf, char* pikaAsm);
int bytecodeFrame_fromMultiLine(ByteCodeFrame* bytecode_frame,
                                char* python_lines);
void Parser_compilePyToBytecodeArray(char* lines);
char* Parser_parsePyLines(Args* outBuffs,
                          ByteCodeFrame* bytecode_frame,
                          char* py_lines);
#define ParserState_forEach(parseState)  \
    ParserState_beforeIter(&parseState); \
    for (int i = 0; i < parseState.length; i++)

#define ParserState_forEachTokenExistPs(parseState, tokens) \
    /* init parserStage */                                 \
    ParserState_init(&parseState);                         \
    ParserState_parse(&parseState, tokens);                \
    ParserState_forEach(parseState)

#define ParserState_forEachToken(parseState, tokens) \
    struct ParserState ps;                           \
    ParserState_forEachTokenExistPs(parseState, tokens)

#undef __PIKA_PARSER_CLASS_IMPLEMENT__
#undef __PIKA_PARSER_CLASS_INHERIT__

#endif
