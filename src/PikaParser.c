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

#include "PikaParser.h"
#include "BaseObj.h"
#include "PikaObj.h"
#include "dataQueue.h"
#include "dataQueueObj.h"
#include "dataStack.h"
#include "dataStrs.h"

/* local head */
typedef QueueObj AST;
char* AST_toPikaASM(AST* ast, Args* outBuffs);
char* Lexer_getTokens(Args* outBuffs, char* stmt);
int32_t AST_deinit(AST* ast);
char* Parser_multiLineToAsm(Args* outBuffs, char* multiLine);
uint8_t Parser_isContainToken(char* tokens,
                              enum TokenType token_type,
                              char* pyload);
void ParserState_init(struct ParserState* ps);
void ParserState_parse(struct ParserState* ps, char* stmt);
void ParserState_deinit(struct ParserState* ps);
void ParserState_beforeIter(struct ParserState* ps);
void ParserState_iterStart(struct ParserState* ps);
void ParserState_iterEnd(struct ParserState* ps);
char* Parser_popToken(Args* buffs_p, char* tokens);

uint16_t Tokens_getSize(char* tokens) {
    if (strEqu("", tokens)) {
        return 0;
    }
    return strCountSign(tokens, 0x1F) + 1;
}

char* strsPopTokenWithSkip_byStr(Args* outBuffs,
                                 char* stmts,
                                 char* str,
                                 char skipStart,
                                 char skipEnd) {
    uint8_t divider_index = 0;
    Arg* keeped_arg = arg_setStr(NULL, "", "");
    Arg* poped_arg = arg_setStr(NULL, "", "");
    {
        ParserState_forEachToken(ps, stmts) {
            ParserState_iterStart(&ps);
            if (ps.branket_deepth == 0) {
                if (strEqu(str, ps.token1.pyload)) {
                    divider_index = ps.iter_index;
                }
            }
            ParserState_iterEnd(&ps);
        }
        ParserState_deinit(&ps);
    }
    {
        ParserState_forEachToken(ps, stmts) {
            ParserState_iterStart(&ps);
            if (ps.iter_index < divider_index) {
                poped_arg = arg_strAppend(poped_arg, ps.token1.pyload);
            }
            if (ps.iter_index > divider_index) {
                keeped_arg = arg_strAppend(keeped_arg, ps.token1.pyload);
            }
            ParserState_iterEnd(&ps);
        }
        ParserState_deinit(&ps);
    }
    char* keeped = arg_getStr(keeped_arg);
    char* poped = strsCopy(outBuffs, arg_getStr(poped_arg));
    __platform_memcpy(stmts, keeped, strGetSize(keeped) + 1);
    arg_deinit(poped_arg);
    arg_deinit(keeped_arg);
    return poped;
}

char* strsGetCleanCmd(Args* outBuffs, char* cmd) {
    int32_t size = strGetSize(cmd);
    char* strOut = args_getBuff(outBuffs, size);
    int32_t iOut = 0;
    ParserState_forEachToken(ps, cmd) {
        ParserState_iterStart(&ps);
        for (uint16_t k = 0; k < strGetSize(ps.token1.pyload); k++) {
            strOut[iOut] = ps.token1.pyload[k];
            iOut++;
        }
        ParserState_iterEnd(&ps);
    }
    ParserState_deinit(&ps);
    /* add \0 */
    strOut[iOut] = 0;
    return strOut;
}

char* strsDeleteBetween(Args* buffs_p, char* strIn, char begin, char end) {
    int32_t size = strGetSize(strIn);
    char* strOut = args_getBuff(buffs_p, size);
    uint8_t deepth = 0;
    uint32_t iOut = 0;
    for (int i = 0; i < size; i++) {
        if (end == strIn[i]) {
            deepth--;
        }
        if (0 == deepth) {
            strOut[iOut] = strIn[i];
            iOut++;
        }
        if (begin == strIn[i]) {
            deepth++;
        }
    }
    strOut[iOut] = 0;
    return strOut;
}

static uint8_t Lexer_isError(char* line) {
    Args buffs = {0};
    uint8_t res = 0; /* not error */
    char* tokens = Lexer_getTokens(&buffs, line);
    if (NULL == tokens) {
        res = 1; /* lex error */
        goto exit;
    }
    goto exit;
exit:
    strsDeinit(&buffs);
    return res;
}

static enum StmtType Lexer_matchStmtType(char* right) {
    Args buffs = {0};
    enum StmtType stmtType = STMT_none;
    char* rightWithoutSubStmt = strsDeleteBetween(&buffs, right, '(', ')');

    uint8_t is_get_operator = 0;
    uint8_t is_get_method = 0;
    uint8_t is_get_string = 0;
    uint8_t is_get_number = 0;
    uint8_t is_get_symbol = 0;
    ParserState_forEachToken(ps, rightWithoutSubStmt) {
        ParserState_iterStart(&ps);
        /* collect type */
        if (ps.token1.type == TOKEN_operator) {
            is_get_operator = 1;
            goto iter_continue;
        }
        if (ps.token2.type == TOKEN_devider) {
            is_get_method = 1;
            goto iter_continue;
        }
        if (ps.token1.type == TOKEN_literal) {
            if (ps.token1.pyload[0] == '\'' || ps.token1.pyload[0] == '"') {
                is_get_string = 1;
                goto iter_continue;
            }
            is_get_number = 1;
            goto iter_continue;
        }
        if (ps.token1.type == TOKEN_symbol) {
            is_get_symbol = 1;
            goto iter_continue;
        }
    iter_continue:
        ParserState_iterEnd(&ps);
    }
    if (is_get_operator) {
        stmtType = STMT_operator;
        goto exit;
    }
    if (is_get_method) {
        stmtType = STMT_method;
        goto exit;
    }
    if (is_get_string) {
        stmtType = STMT_string;
        goto exit;
    }
    if (is_get_number) {
        stmtType = STMT_number;
        goto exit;
    }
    if (is_get_symbol) {
        stmtType = STMT_reference;
        goto exit;
    }
exit:
    ParserState_deinit(&ps);
    strsDeinit(&buffs);
    return stmtType;
}

char* Lexer_printTokens(Args* outBuffs, char* tokens) {
    /* init */
    Args buffs = {0};
    char* printOut = strsCopy(&buffs, "");

    /* process */
    uint16_t token_size = Tokens_getSize(tokens);
    for (uint16_t i = 0; i < token_size; i++) {
        char* token = Parser_popToken(&buffs, tokens);
        if (token[0] == TOKEN_operator) {
            printOut = strsAppend(&buffs, printOut, "{opt}");
            printOut = strsAppend(&buffs, printOut, token + 1);
        }
        if (token[0] == TOKEN_devider) {
            printOut = strsAppend(&buffs, printOut, "{dvd}");
            printOut = strsAppend(&buffs, printOut, token + 1);
        }
        if (token[0] == TOKEN_symbol) {
            printOut = strsAppend(&buffs, printOut, "{sym}");
            printOut = strsAppend(&buffs, printOut, token + 1);
        }
        if (token[0] == TOKEN_literal) {
            printOut = strsAppend(&buffs, printOut, "{lit}");
            printOut = strsAppend(&buffs, printOut, token + 1);
        }
    }
    /* out put */
    printOut = strsCopy(outBuffs, printOut);
    strsDeinit(&buffs);
    return printOut;
}

uint8_t Parser_checkIsDirect(char* str) {
    Args buffs = {0};
    char* tokens = Lexer_getTokens(&buffs, str);
    uint8_t res = 0;
    if (Parser_isContainToken(tokens, TOKEN_operator, "=")) {
        res = 1;
        goto exit;
    }
exit:
    strsDeinit(&buffs);
    return res;
}

Arg* Lexer_setToken(Arg* tokens_arg,
                    enum TokenType token_type,
                    char*
                    operator) {
    Args buffs = {0};
    char token_type_buff[3] = {0};
    token_type_buff[0] = 0x1F;
    token_type_buff[1] = token_type;
    char* tokens = arg_getStr(tokens_arg);
    tokens = strsAppend(&buffs, tokens, token_type_buff);
    tokens = strsAppend(&buffs, tokens, operator);
    Arg* new_tokens_arg = arg_setStr(tokens_arg, "", tokens);
    arg_deinit(tokens_arg);
    strsDeinit(&buffs);
    return new_tokens_arg;
}

Arg* Lexer_setSymbel(Arg* tokens_arg,
                     char* stmt,
                     int32_t i,
                     int32_t* symbol_start_index) {
    Args buffs = {0};
    char* symbol_buff = NULL;
    /* nothing to add symbel */
    if (i == *symbol_start_index) {
        *symbol_start_index = -1;
        goto exit;
    }
    symbol_buff = args_getBuff(&buffs, i - *symbol_start_index);
    __platform_memcpy(symbol_buff, stmt + *symbol_start_index,
                      i - *symbol_start_index);
    /* literal */
    if ((symbol_buff[0] == '\'') || (symbol_buff[0] == '"') ||
        ((symbol_buff[0] >= '0') && (symbol_buff[0] <= '9'))) {
        tokens_arg = Lexer_setToken(tokens_arg, TOKEN_literal, symbol_buff);
    } else {
        /* symbol */
        tokens_arg = Lexer_setToken(tokens_arg, TOKEN_symbol, symbol_buff);
    }
    *symbol_start_index = -1;
exit:
    strsDeinit(&buffs);
    return tokens_arg;
}

/* tokens is devided by space */
/* a token is [TOKENTYPE|(CONTENT)] */
char* Lexer_getTokens(Args* outBuffs, char* stmt) {
    /* init */
    Arg* tokens_arg = New_arg(NULL);
    tokens_arg = arg_setStr(tokens_arg, "", "");
    int32_t size = strGetSize(stmt);
    uint8_t bracket_deepth = 0;
    uint8_t c0 = 0;
    uint8_t c1 = 0;
    uint8_t c2 = 0;
    uint8_t c3 = 0;
    int32_t symbol_start_index = -1;
    int is_in_string = 0;
    char* tokens;

    /* process */
    for (int32_t i = 0; i < size; i++) {
        /* update char */
        c0 = stmt[i];
        c1 = 0;
        c2 = 0;
        c3 = 0;
        if (i + 1 < size) {
            c1 = stmt[i + 1];
        }
        if (i + 2 < size) {
            c2 = stmt[i + 2];
        }
        if (i + 3 < size) {
            c3 = stmt[i + 3];
        }
        if (-1 == symbol_start_index) {
            symbol_start_index = i;
        }

        /* solve string */
        if (0 == is_in_string) {
            if ('\'' == c0) {
                /* in ' */
                is_in_string = 1;
                continue;
            }
            if ('"' == c0) {
                /* in "" */
                is_in_string = 2;
                continue;
            }
        }

        if (1 == is_in_string) {
            if ('\'' == c0) {
                is_in_string = 0;
                tokens_arg = Lexer_setSymbel(tokens_arg, stmt, i + 1,
                                             &symbol_start_index);
            }
            continue;
        }
        if (2 == is_in_string) {
            if ('"' == c0) {
                is_in_string = 0;
                tokens_arg = Lexer_setSymbel(tokens_arg, stmt, i + 1,
                                             &symbol_start_index);
            }
            continue;
        }

        /* match devider*/
        if (('(' == c0) || (')' == c0) || (',' == c0) || ('[' == c0) ||
            (']' == c0) || (':' == c0)) {
            tokens_arg =
                Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
            char content[2] = {0};
            content[0] = c0;
            tokens_arg = Lexer_setToken(tokens_arg, TOKEN_devider, content);
            if (c0 == '(') {
                bracket_deepth++;
            }
            if (c0 == ')') {
                bracket_deepth--;
            }
            continue;
        }
        /* match operator */
        if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0) ||
            ('+' == c0) || ('-' == c0) || ('!' == c0) || ('=' == c0) ||
            ('%' == c0) || ('&' == c0) || ('|' == c0) || ('^' == c0) ||
            ('~' == c0)) {
            if (('*' == c0) || ('/' == c0)) {
                /*
                    =, **=, //
                */
                if ((c0 == c1) && ('=' == c2)) {
                    char content[4] = {0};
                    content[0] = c0;
                    content[1] = c1;
                    content[2] = '=';
                    tokens_arg = Lexer_setSymbel(tokens_arg, stmt, i,
                                                 &symbol_start_index);
                    tokens_arg =
                        Lexer_setToken(tokens_arg, TOKEN_operator, content);
                    i = i + 2;
                    continue;
                }
            }
            /*
                >>, <<, **, //
            */
            if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0)) {
                if (c0 == c1) {
                    char content[3] = {0};
                    content[0] = c0;
                    content[1] = c1;
                    tokens_arg = Lexer_setSymbel(tokens_arg, stmt, i,
                                                 &symbol_start_index);
                    tokens_arg =
                        Lexer_setToken(tokens_arg, TOKEN_operator, content);
                    i = i + 1;
                    continue;
                }
            }
            /*
                >=, <=, *=, /=, +=, -=, !=, ==, %=
            */
            if (('>' == c0) || ('<' == c0) || ('*' == c0) || ('/' == c0) ||
                ('+' == c0) || ('-' == c0) || ('!' == c0) || ('=' == c0) ||
                ('%' == c0)) {
                if ('=' == c1) {
                    char content[3] = {0};
                    content[0] = c0;
                    content[1] = c1;
                    tokens_arg = Lexer_setSymbel(tokens_arg, stmt, i,
                                                 &symbol_start_index);
                    tokens_arg =
                        Lexer_setToken(tokens_arg, TOKEN_operator, content);
                    i = i + 1;
                    continue;
                }
            }

            /* single operator */
            /*
                +, -, *, ... /
            */
            char content[2] = {0};
            content[0] = c0;
            tokens_arg =
                Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
            tokens_arg = Lexer_setToken(tokens_arg, TOKEN_operator, content);
            continue;
        }
        /* not */
        if ('n' == c0) {
            if (('o' == c1) && ('t' == c2) && (' ' == c3)) {
                tokens_arg =
                    Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
                tokens_arg =
                    Lexer_setToken(tokens_arg, TOKEN_operator, " not ");
                i = i + 3;
                continue;
            }
        }
        /* and */
        if ('a' == c0) {
            if (('n' == c1) && ('d' == c2) && (' ' == c3)) {
                tokens_arg =
                    Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
                tokens_arg =
                    Lexer_setToken(tokens_arg, TOKEN_operator, " and ");
                i = i + 3;
                continue;
            }
        }
        /* or */
        if ('o' == c0) {
            if (('r' == c1) && (' ' == c2)) {
                tokens_arg =
                    Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
                tokens_arg = Lexer_setToken(tokens_arg, TOKEN_operator, " or ");
                i = i + 2;
                continue;
            }
        }
        /* skip spaces */
        if (' ' == c0) {
            /* not get symbal */
            if (i == symbol_start_index) {
                symbol_start_index++;
            } else {
                /* already get symbal */
                tokens_arg =
                    Lexer_setSymbel(tokens_arg, stmt, i, &symbol_start_index);
            }
        }
        if (i == size - 1) {
            /* last check symbel */
            tokens_arg =
                Lexer_setSymbel(tokens_arg, stmt, size, &symbol_start_index);
        }
    }
    if (0 != bracket_deepth) {
        /* bracket match error */
        tokens = NULL;
        goto exit;
    }
    /* output */
    tokens = arg_getStr(tokens_arg);
    tokens = strsCopy(outBuffs, tokens);
exit:
    arg_deinit(tokens_arg);
    return tokens;
}

char* Parser_popToken(Args* buffs_p, char* tokens) {
    return strsPopToken(buffs_p, tokens, 0x1F);
}

enum TokenType Token_getType(char* token) {
    return (enum TokenType)token[0];
}

char* Token_getPyload(char* token) {
    return (char*)((uintptr_t)token + 1);
}

uint8_t Parser_isContainToken(char* tokens,
                              enum TokenType token_type,
                              char* pyload) {
    Args buffs = {0};
    char* tokens_buff = strsCopy(&buffs, tokens);
    uint8_t res = 0;
    uint16_t token_size = Tokens_getSize(tokens);
    for (int i = 0; i < token_size; i++) {
        char* token = Parser_popToken(&buffs, tokens_buff);
        if (token_type == Token_getType(token)) {
            if (strEqu(Token_getPyload(token), pyload)) {
                res = 1;
                goto exit;
            }
        }
    }
exit:
    strsDeinit(&buffs);
    return res;
}

char* Lexer_getOperator(Args* outBuffs, char* stmt) {
    Args buffs = {0};
    char* tokens = Lexer_getTokens(&buffs, stmt);
    char* operator= NULL;
    const char operators[][6] = {
        "**", "~",   "*",  "/",  "%",  "//",  "+",     "-",     ">>",  "<<",
        "&",  "^",   "|",  "<",  "<=", ">",   ">=",    "!=",    "==",  "%=",
        "/=", "//=", "-=", "+=", "*=", "**=", " not ", " and ", " or "};
    for (uint32_t i = 0; i < sizeof(operators) / 6; i++) {
        if (Parser_isContainToken(tokens, TOKEN_operator,
                                  (char*)operators[i])) {
            operator= strsCopy(&buffs, (char*)operators[i]);
        }
    }
    /* match the last operator in equal level */
    if ((strEqu(operator, "+")) || (strEqu(operator, "-"))) {
        ParserState_forEachToken(ps, stmt) {
            ParserState_iterStart(&ps);
            if (strEqu(ps.token1.pyload, "+")) {
                operator= strsCopy(&buffs, "+");
            }
            if (strEqu(ps.token1.pyload, "-")) {
                operator= strsCopy(&buffs, "-");
            }
            ParserState_iterEnd(&ps);
        }
        ParserState_deinit(&ps);
    }
    /* out put */
    operator= strsCopy(outBuffs, operator);
    strsDeinit(&buffs);
    return operator;
}

const char void_str[] = "";

void LexToken_update(struct LexToken* lex_token) {
    lex_token->type = Token_getType(lex_token->token);
    if (lex_token->type == TOKEN_strEnd) {
        lex_token->pyload = (char*)void_str;
    } else {
        lex_token->pyload = Token_getPyload(lex_token->token);
    }
}

void ParserState_iterStart(struct ParserState* ps) {
    ps->iter_index++;
    ps->iter_buffs = New_strBuff();
    /* token1 is the last token */
    ps->token1.token = strsCopy(ps->iter_buffs, arg_getStr(ps->last_token));
    /* token2 is the next token */
    ps->token2.token = Parser_popToken(ps->iter_buffs, ps->tokens);
    /* store last token */
    arg_deinit(ps->last_token);
    ps->last_token = arg_setStr(NULL, "", ps->token2.token);

    LexToken_update(&ps->token1);
    LexToken_update(&ps->token2);
    if (strEqu(ps->token1.pyload, "(")) {
        ps->branket_deepth++;
    }
    if (strEqu(ps->token1.pyload, ")")) {
        ps->branket_deepth--;
    }
}

void LexToken_init(struct LexToken* lt) {
    lt->pyload = NULL;
    lt->token = NULL;
    lt->type = TOKEN_strEnd;
}

void ParserState_init(struct ParserState* ps) {
    ps->tokens = NULL;
    ps->length = 0;
    ps->iter_index = 0;
    ps->branket_deepth = 0;
    ps->last_token = NULL;
    ps->iter_buffs = NULL;
    ps->buffs_p = New_strBuff();
    LexToken_init(&ps->token1);
    LexToken_init(&ps->token2);
}

void ParserState_iterEnd(struct ParserState* ps) {
    args_deinit(ps->iter_buffs);
}

void ParserState_deinit(struct ParserState* ps) {
    if (NULL != ps->last_token) {
        arg_deinit(ps->last_token);
    }
    args_deinit(ps->buffs_p);
}

void ParserState_parse(struct ParserState* ps, char* stmt) {
    ps->tokens = Lexer_getTokens(ps->buffs_p, stmt);
    ps->length = Tokens_getSize(ps->tokens);
}

void ParserState_beforeIter(struct ParserState* ps) {
    /* clear first token */
    Parser_popToken(ps->buffs_p, ps->tokens);
    ps->last_token =
        arg_setStr(NULL, "", Parser_popToken(ps->buffs_p, ps->tokens));
}

char* Parser_solveBranckets(Args* outBuffs,
                            char* content,
                            char* stmt,
                            char* mode) {
    /* init objects */
    Args buffs = {0};
    Arg* right_arg = arg_setStr(NULL, "", "");
    uint8_t is_in_brancket = 0;
    args_setStr(&buffs, "index", "");
    char* tokens;
    /* exit when NULL */
    if (NULL == content) {
        arg_deinit(right_arg);
        right_arg = arg_setStr(right_arg, "", stmt);
        goto exit;
    }
    tokens = Lexer_getTokens(&buffs, content);
    /* exit when no '[' ']' */
    if (!Parser_isContainToken(tokens, TOKEN_devider, "[")) {
        /* not contain '[', return origin */
        arg_deinit(right_arg);
        if (strEqu(mode, "right")) {
            right_arg = arg_setStr(right_arg, "", content);
        } else if (strEqu(mode, "left")) {
            right_arg = arg_setStr(right_arg, "", stmt);
        }
        goto exit;
    }
    /* matched [] */
    ParserState_forEachToken(ps, content) {
        ParserState_iterStart(&ps);
        /* found '[' */
        if ((TOKEN_devider == ps.token2.type) &&
            (strEqu(ps.token2.pyload, "["))) {
            /* get 'obj' from obj[] */
            args_setStr(&buffs, "obj", ps.token1.pyload);
            is_in_brancket = 1;
            /* fond ']' */
        } else if ((TOKEN_devider == ps.token2.type) &&
                   (strEqu(ps.token2.pyload, "]"))) {
            is_in_brancket = 0;
            char* index = args_getStr(&buffs, "index");
            Arg* index_arg = arg_setStr(NULL, "", index);
            index_arg = arg_strAppend(index_arg, ps.token1.pyload);
            args_setStr(&buffs, "index", arg_getStr(index_arg));
            arg_deinit(index_arg);

            if (strEqu(mode, "right")) {
                right_arg = arg_strAppend(right_arg, "__get__(");
            } else if (strEqu(mode, "left")) {
                right_arg = arg_strAppend(right_arg, "__set__(");
            }
            right_arg = arg_strAppend(right_arg, args_getStr(&buffs, "obj"));
            right_arg = arg_strAppend(right_arg, ",");
            right_arg = arg_strAppend(right_arg, args_getStr(&buffs, "index"));
            if (strEqu(mode, "left")) {
                right_arg = arg_strAppend(right_arg, ",");
                right_arg = arg_strAppend(right_arg, stmt);
                right_arg = arg_strAppend(right_arg,
                                          ","
                                          "'");
                right_arg =
                    arg_strAppend(right_arg, args_getStr(&buffs, "obj"));
                right_arg = arg_strAppend(right_arg, "'");
            }
            right_arg = arg_strAppend(right_arg, ")");
            args_setStr(&buffs, "index", "");
            /* in brancket and found '[' */
        } else if (is_in_brancket && (!strEqu(ps.token1.pyload, "["))) {
            char* index = args_getStr(&buffs, "index");
            Arg* index_arg = arg_setStr(NULL, "", index);
            index_arg = arg_strAppend(index_arg, ps.token1.pyload);
            args_setStr(&buffs, "index", arg_getStr(index_arg));
            arg_deinit(index_arg);
            /* out of brancket and not found ']' */
        } else if (!is_in_brancket && (!strEqu(ps.token1.pyload, "]"))) {
            if (TOKEN_strEnd != ps.token1.type) {
                right_arg = arg_strAppend(right_arg, ps.token1.pyload);
            }
        }
        ParserState_iterEnd(&ps);
    }
    ParserState_deinit(&ps);
exit:
    /* clean and return */
    content = strsCopy(outBuffs, arg_getStr(right_arg));
    arg_deinit(right_arg);
    strsDeinit(&buffs);
    return content;
}

char* Parser_solveRightBranckets(Args* outBuffs, char* right) {
    return Parser_solveBranckets(outBuffs, right, NULL, "right");
}

char* Parser_solveLeftBranckets(Args* outBuffs, char* right, char* left) {
    return Parser_solveBranckets(outBuffs, left, right, "left");
}

uint8_t Parser_solveSelfOperator(Args* outbuffs,
                                 char* stmt,
                                 char** right_p,
                                 char** left_p) {
    char* left_new = NULL;
    char* right_new = NULL;
    Arg* left_arg = arg_setStr(NULL, "", "");
    Arg* right_arg = arg_setStr(NULL, "", "");
    Arg* right_arg_new = arg_setStr(NULL, "", "");
    uint8_t is_left_exist = 0;

    Args buffs = {0};
    char _operator[2] = {0};
    char* operator=(char*) _operator;
    char* tokens = Lexer_getTokens(&buffs, stmt);
    uint8_t is_right = 0;
    if (Parser_isContainToken(tokens, TOKEN_operator, "+=")) {
        operator[0] = '+';
    }
    if (Parser_isContainToken(tokens, TOKEN_operator, "-=")) {
        operator[0] = '-';
    }
    if (Parser_isContainToken(tokens, TOKEN_operator, "*=")) {
        operator[0] = '*';
    }
    if (Parser_isContainToken(tokens, TOKEN_operator, "/=")) {
        operator[0] = '/';
    }
    /* not found self operator */
    if (operator[0] == 0) {
        goto exit;
    }
    /* found self operator */
    is_left_exist = 1;
    ParserState_forEachToken(ps, stmt) {
        ParserState_iterStart(&ps);
        if ((strEqu(ps.token1.pyload, "*=")) ||
            (strEqu(ps.token1.pyload, "/=")) ||
            (strEqu(ps.token1.pyload, "+=")) ||
            (strEqu(ps.token1.pyload, "-="))) {
            is_right = 1;
            goto iter_continue;
        }
        if (!is_right) {
            left_arg = arg_strAppend(left_arg, ps.token1.pyload);
        } else {
            right_arg = arg_strAppend(right_arg, ps.token1.pyload);
        }
    iter_continue:
        ParserState_iterEnd(&ps);
    }
    ParserState_deinit(&ps);
    /* connect right */
    right_arg_new = arg_strAppend(right_arg_new, arg_getStr(left_arg));
    right_arg_new = arg_strAppend(right_arg_new, operator);
    right_arg_new = arg_strAppend(right_arg_new, "(");
    right_arg_new = arg_strAppend(right_arg_new, arg_getStr(right_arg));
    right_arg_new = arg_strAppend(right_arg_new, ")");

    /* collect left_new and right_new */
    left_new = arg_getStr(left_arg);
    right_new = arg_getStr(right_arg_new);

exit:
    strsDeinit(&buffs);
    if (NULL != right_new) {
        *(right_p) = strsCopy(outbuffs, right_new);
        ;
    }
    if (NULL != left_new) {
        *(left_p) = strsCopy(outbuffs, left_new);
    }
    arg_deinit(right_arg);
    arg_deinit(left_arg);
    arg_deinit(right_arg_new);
    return is_left_exist;
}

AST* AST_parseStmt(AST* ast, char* stmt) {
    Args buffs = {0};
    char* assignment = strsGetFirstToken(&buffs, stmt, '(');
    char* method = NULL;
    char* ref = NULL;
    char* str = NULL;
    char* num = NULL;
    char* left = NULL;
    char* right = NULL;

    right = stmt;
    /* solve check direct */
    uint8_t isLeftExist = 0;
    if (Parser_checkIsDirect(assignment)) {
        isLeftExist = 1;
        left = strsGetFirstToken(&buffs, assignment, '=');
        right = strPointToLastToken(stmt, '=');
    }
    /* solve the += -= /= *= stmt */
    if (!isLeftExist) {
        isLeftExist = Parser_solveSelfOperator(&buffs, stmt, &right, &left);
    }

    /* solve the [] stmt */
    right = Parser_solveRightBranckets(&buffs, right);
    char* right_new = Parser_solveLeftBranckets(&buffs, right, left);

    /* left is contain the '[]' */
    if (!strEqu(right_new, right)) {
        /* update new right */
        right = right_new;
        /* cancel left */
        isLeftExist = 0;
    }

    /* set left */
    if (isLeftExist) {
        obj_setStr(ast, (char*)"left", left);
    }
    /* match statment type */
    enum StmtType stmtType = Lexer_matchStmtType(right);
    /* solve operator stmt */
    if (STMT_operator == stmtType) {
        char* rightWithoutSubStmt = strsDeleteBetween(&buffs, right, '(', ')');
        char* operator= Lexer_getOperator(&buffs, rightWithoutSubStmt);
        obj_setStr(ast, (char*)"operator", operator);
        char* rightBuff = strsCopy(&buffs, right);
        char* subStmt1 =
            strsPopTokenWithSkip_byStr(&buffs, rightBuff, operator, '(', ')');
        char* subStmt2 = rightBuff;
        queueObj_pushObj(ast, (char*)"stmt");
        AST_parseStmt(queueObj_getCurrentObj(ast), subStmt1);
        queueObj_pushObj(ast, (char*)"stmt");
        AST_parseStmt(queueObj_getCurrentObj(ast), subStmt2);
        goto exit;
    }
    /* solve method stmt */
    if (STMT_method == stmtType) {
        method = strsGetFirstToken(&buffs, right, '(');
        obj_setStr(ast, (char*)"method", method);
        char* subStmts = strsCut(&buffs, right, '(', ')');
        /* add ',' at the end */
        subStmts = strsAppend(&buffs, subStmts, ",");
        /* init process values */
        Arg* subStmt = arg_setStr(NULL, "", "");
        /* start iteration */
        char* subStmt_str = NULL;
        ParserState_forEachToken(ps, subStmts) {
            ParserState_iterStart(&ps);
            /* parse process */
            if (ps.branket_deepth > 0) {
                /* in brankets */
                /* append token to subStmt */
                subStmt = arg_strAppend(subStmt, ps.token1.pyload);
                subStmt_str = arg_getStr(subStmt);
            } else {
                /* not in brankets */
                if (strEqu(ps.token1.pyload, ",")) {
                    /* found "," push subStmt */
                    queueObj_pushObj(ast, (char*)"stmt");
                    subStmt_str = arg_getStr(subStmt);
                    AST_parseStmt(queueObj_getCurrentObj(ast), subStmt_str);
                    /* clear subStmt */
                    arg_deinit(subStmt);
                    subStmt = arg_setStr(NULL, "", "");
                } else {
                    /* not "," append subStmt */
                    subStmt = arg_strAppend(subStmt, ps.token1.pyload);
                    subStmt_str = arg_getStr(subStmt);
                }
            }
            /* parse preocess end */
            ParserState_iterEnd(&ps);
            continue;
        }
        ParserState_deinit(&ps);
        arg_deinit(subStmt);
        goto exit;
    }
    /* solve reference stmt */
    if (STMT_reference == stmtType) {
        ref = right;
        obj_setStr(ast, (char*)"ref", ref);
        goto exit;
    }
    /* solve str stmt */
    if (STMT_string == stmtType) {
        str = right;
        str = strsDeleteChar(&buffs, str, '\'');
        str = strsDeleteChar(&buffs, str, '\"');
        obj_setStr(ast, (char*)"string", str);
        goto exit;
    }
    /* solve number stmt */
    if (STMT_number == stmtType) {
        num = right;
        obj_setStr(ast, (char*)"num", num);
        goto exit;
    }
exit:
    strsDeinit(&buffs);
    return ast;
}

static int32_t Parser_getPyLineBlockDeepth(char* line) {
    uint32_t size = strGetSize(line);
    for (uint32_t i = 0; i < size; i++) {
        if (line[i] != ' ') {
            uint32_t spaceNum = i;
            if (0 == spaceNum % 4) {
                return spaceNum / 4;
            }
            /* space Num is not 4N, error*/
            return -1;
        }
    }
    return 0;
}

char* Parser_removeAnnotation(char* line) {
    uint8_t is_annotation_exit = 0;
    uint8_t is_in_single_quotes = 0;
    uint8_t is_in_double_quotes_deepth = 0;
    for (uint32_t i = 0; i < strGetSize(line); i++) {
        if ('\'' == line[i]) {
            is_in_single_quotes = !is_in_single_quotes;
            continue;
        }
        if ('"' == line[i]) {
            is_in_double_quotes_deepth = !is_in_double_quotes_deepth;
            continue;
        }
        if (!(is_in_single_quotes == 0 && is_in_double_quotes_deepth == 0)) {
            continue;
        }
        if ('#' == line[i]) {
            /* end the line */
            line[i] = 0;
            is_annotation_exit = 1;
            break;
        }
    }
    /* no annotation, exit */
    if (!is_annotation_exit) {
        return line;
    }
    /* check empty line */
    for (uint32_t i = 0; i < strGetSize(line); i++) {
        if (' ' != line[i]) {
            return line;
        }
    }
    /* is an emply line */
    line = "@annontation";
    return line;
}

AST* AST_parseLine(char* line, Stack* block_stack) {
    /* match block start keywords */
    const char control_keywords[][9] = {"break", "continue"};

    /* normal keyward */
    const char normal_keywords[][7] = {"while", "if", "elif"};

    /* line is not exist */
    if (line == NULL) {
        return NULL;
    }
    /* init data */
    AST* ast = New_queueObj();
    Args buffs = {0};
    int8_t block_deepth, block_deepth_last;
    char *line_start, *stmt;
    /* get block deepth */
    block_deepth = Parser_getPyLineBlockDeepth(line);
    block_deepth_last = block_deepth;
    /* set block deepth */
    if (block_deepth == -1) {
        /* get block_deepth error */
        obj_deinit(ast);
        ast = NULL;
        goto exit;
    }
    obj_setInt(ast, "blockDeepth", block_deepth);

    /* check if exit block */
    if (NULL != block_stack) {
        block_deepth_last = stack_getTop(block_stack);
        /* exit each block */
        for (int i = 0; i < block_deepth_last - block_deepth; i++) {
            QueueObj* exit_block_queue = obj_getObj(ast, "exitBlock", 0);
            /* create an exit_block queue */
            if (NULL == exit_block_queue) {
                obj_newObj(ast, "exitBlock", "", New_TinyObj);
                exit_block_queue = obj_getObj(ast, "exitBlock", 0);
                queueObj_init(exit_block_queue);
            }
            char buff[10] = {0};
            char* block_type = stack_popStr(block_stack, buff);
            /* push exit block type to exit_block queue */
            queueObj_pushStr(exit_block_queue, block_type);
        }
    }

    line_start = line + block_deepth * 4;
    stmt = line_start;

    for (uint32_t i = 0; i < sizeof(normal_keywords) / 7; i++) {
        char* keyword = (char*)normal_keywords[i];
        uint8_t keyword_len = strGetSize(keyword);
        if (strIsStartWith(line_start, keyword) &&
            (line_start[keyword_len] == ' ')) {
            stmt = strsCut(&buffs, line_start, ' ', ':');
            obj_setStr(ast, "block", keyword);
            if (NULL != block_stack) {
                stack_pushStr(block_stack, keyword);
            }
            goto block_matched;
        }
    }

    /* contral keyward */
    for (uint32_t i = 0; i < sizeof(control_keywords) / 8; i++) {
        char* keyward = (char*)control_keywords[i];
        uint8_t keyward_size = strGetSize(keyward);
        if ((strIsStartWith(line_start, keyward)) &&
            ((line_start[keyward_size] == ' ') ||
             (line_start[keyward_size] == 0))) {
            obj_setStr(ast, keyward, "");
            stmt = "";
            goto block_matched;
        }
    }

    if (strIsStartWith(line_start, "for ")) {
        Args* list_buffs = New_strBuff();
        char* line_buff = strsCopy(list_buffs, line_start + 4);
        char* arg_in = strsPopToken(list_buffs, line_buff, ' ');
        obj_setStr(ast, "arg_in", arg_in);
        strsPopToken(list_buffs, line_buff, ' ');
        if (strIsStartWith(line_buff, "range(")) {
            obj_setInt(ast, "isRange", 1);
        }
        char* list_in = strsPopToken(list_buffs, line_buff, ':');
        list_in = strsAppend(list_buffs, "iter(", list_in);
        list_in = strsAppend(list_buffs, list_in, ")");
        list_in = strsCopy(&buffs, list_in);
        args_deinit(list_buffs);
        obj_setStr(ast, "block", "for");
        obj_setStr(ast, "list_in", list_in);
        if (NULL != block_stack) {
            stack_pushStr(block_stack, "for");
        }
        stmt = list_in;
        goto block_matched;
    }

    if (strIsStartWith(line_start, "else")) {
        if ((line_start[4] == ' ') || (line_start[4] == ':')) {
            stmt = "";
            obj_setStr(ast, "block", "else");
            if (NULL != block_stack) {
                stack_pushStr(block_stack, "else");
            }
        }
        goto block_matched;
    }
    if (strEqu(line_start, "return")) {
        obj_setStr(ast, "return", "");
        stmt = "";
        goto block_matched;
    }
    if (strIsStartWith(line_start, "return ")) {
        char* lineBuff = strsCopy(&buffs, line_start);
        strsPopToken(&buffs, lineBuff, ' ');
        stmt = lineBuff;
        obj_setStr(ast, "return", "");
        goto block_matched;
    }
    if (strIsStartWith(line_start, "import ")) {
        stmt = "";
        goto block_matched;
    }
    if (strIsStartWith(line_start, "from ")) {
        stmt = "";
        goto block_matched;
    }
    if (strIsStartWith(line_start, "global ")) {
        stmt = "";
        char* global_list = line_start + 7;
        global_list = strsGetCleanCmd(&buffs, global_list);
        obj_setStr(ast, "global", global_list);
        goto block_matched;
    }
    if (strIsStartWith(line_start, (char*)"def ")) {
        stmt = "";
        char* declear = strsCut(&buffs, line_start, ' ', ':');
        declear = strsGetCleanCmd(&buffs, declear);
        obj_setStr(ast, "block", "def");
        obj_setStr(ast, "declear", declear);
        if (NULL != block_stack) {
            stack_pushStr(block_stack, "def");
        }
        goto block_matched;
    }
    if (strIsStartWith(line_start, (char*)"class ")) {
        stmt = "";
        char* declear = strsCut(&buffs, line_start, ' ', ':');
        declear = strsGetCleanCmd(&buffs, declear);
        obj_setStr(ast, "block", "class");
        obj_setStr(ast, "declear", declear);
        if (NULL != block_stack) {
            stack_pushStr(block_stack, "class");
        }
        goto block_matched;
    }

block_matched:
    stmt = strsGetCleanCmd(&buffs, stmt);
    ast = AST_parseStmt(ast, stmt);
    goto exit;
exit:
    strsDeinit(&buffs);
    return ast;
}

static char* Parser_linePreProcess(Args* buffs_p, char* line) {
    /* check syntex error */
    if (Lexer_isError(line)) {
        line = NULL;
        goto exit;
    }
    /* process EOL */
    line = strsDeleteChar(buffs_p, line, '\r');
    line = Parser_removeAnnotation(line);

exit:
    return line;
}

char* Parser_LineToAsm(Args* buffs_p, char* line, Stack* blockStack) {
    char* ASM = NULL;
    AST* ast = NULL;
    /* pre process */
    line = Parser_linePreProcess(buffs_p, line);
    if (NULL == line) {
        /* preprocess error */
        goto exit;
    }
    if (strEqu("@annontation", line)) {
        ASM = "";
        goto exit;
    }

    /* parse line to tokens */
    /* parse tokens to AST */
    ast = AST_parseLine(line, blockStack);
    /* gen ASM from AST */
    ASM = AST_toPikaASM(ast, buffs_p);
exit:
    if (NULL != ast) {
        AST_deinit(ast);
    }
    return ASM;
}

static int Parser_isVoidLine(char* line) {
    for (uint32_t i = 0; i < strGetSize(line); i++) {
        if (line[i] != ' ') {
            return 0;
        }
    }
    return 1;
}

static uint8_t Parser_checkIsMultiComment(char* line) {
    for (uint32_t i = 0; i < strGetSize(line); i++) {
        /* not match ' or " */
        if ((line[i] != '\'') && (line[i] != '"')) {
            continue;
        }
        /* not match ''' or """ */
        if (!((line[i + 1] == line[i]) && (line[i + 2] == line[i]))) {
            continue;
        }
        /* check char befor the ''' or """ */
        if (!((0 == i) || (line[i - 1] == ' '))) {
            continue;
        }
        /* check char after the ''' or """ */
        if (!((line[i + 3] == ' ') || (line[i + 3] == 0))) {
            continue;
        }
        /* mached */
        return 1;
    }
    /* not mached */
    return 0;
}

static char* Parser_parsePyLines(Args* outBuffs,
                                 ByteCodeFrame* bytecode_frame,
                                 char* py_lines) {
    Stack block_stack;
    stack_init(&block_stack);
    Arg* asm_buff = arg_setStr(NULL, "", "");
    uint32_t lines_offset = 0;
    uint32_t lines_size = strGetSize(py_lines);
    uint16_t lines_num = strCountSign(py_lines, '\n');
    uint16_t lines_index = 0;
    uint8_t is_in_multi_comment = 0;
    char* out_ASM = NULL;
    char* single_ASM;
    uint32_t line_size = 0;
    /* parse each line */
    while (1) {
        lines_index++;
        Args buffs = {0};
        /* get single line by pop multiline */
        char* line = strsGetFirstToken(&buffs, py_lines + lines_offset, '\n');
        /* filter for not end \n */
        if (lines_index != lines_num) {
            if (Parser_isVoidLine(line)) {
                goto next_line;
            }
        }

        /* filter for multiline comment ''' or """ */
        if (Parser_checkIsMultiComment(line)) {
            is_in_multi_comment = ~is_in_multi_comment;
            goto next_line;
        }

        /* skipe multiline comment */
        if (is_in_multi_comment) {
            goto next_line;
        }

        /* parse single Line to Asm */
        single_ASM = Parser_LineToAsm(&buffs, line, &block_stack);
        if (NULL == single_ASM) {
            out_ASM = NULL;
            strsDeinit(&buffs);
            goto exit;
        }
        if (NULL == bytecode_frame) {
            /* store ASM */
            asm_buff = arg_strAppend(asm_buff, single_ASM);
        } else if (NULL == outBuffs) {
            /* store ByteCode */
            byteCodeFrame_appendFromAsm(bytecode_frame, single_ASM);
        }
    next_line:
        line_size = strGetSize(line);
        lines_offset = lines_offset + line_size + 1;
        strsDeinit(&buffs);
        /* exit when finished */
        if (lines_offset >= lines_size) {
            break;
        }
    }
    if (NULL != outBuffs) {
        /* load stored ASM */
        out_ASM = strsCopy(outBuffs, arg_getStr(asm_buff));
    } else {
        out_ASM = (char*)1;
    }
    goto exit;
exit:
    if (NULL != asm_buff) {
        arg_deinit(asm_buff);
    }
    stack_deinit(&block_stack);
    return out_ASM;
};

int bytecodeFrame_fromMultiLine(ByteCodeFrame* bytecode_frame,
                                char* multi_line) {
    if (NULL == Parser_parsePyLines(NULL, bytecode_frame, multi_line)) {
        /* error */
        return 1;
    }
    /* succeed */
    return 0;
};

char* Parser_multiLineToAsm(Args* outBuffs, char* multi_line) {
    return Parser_parsePyLines(outBuffs, NULL, multi_line);
}

char* AST_appandPikaASM(AST* ast, AST* subAst, Args* outBuffs, char* pikaAsm) {
    int deepth = obj_getInt(ast, "deepth");
    Args buffs = {0};
    while (1) {
        QueueObj* subStmt = queueObj_popObj(subAst);
        if (NULL == subStmt) {
            break;
        }
        obj_setInt(ast, "deepth", deepth + 1);
        pikaAsm = AST_appandPikaASM(ast, subStmt, &buffs, pikaAsm);
    }
    char* method = obj_getStr(subAst, "method");
    char* operator= obj_getStr(subAst, "operator");
    char* ref = obj_getStr(subAst, "ref");
    char* left = obj_getStr(subAst, "left");
    char* str = obj_getStr(subAst, "string");
    char* num = obj_getStr(subAst, "num");
    char* buff = args_getBuff(&buffs, PIKA_SPRINTF_BUFF_SIZE);
    if (NULL != ref) {
        __platform_sprintf(buff, "%d REF %s\n", deepth, ref);
        pikaAsm = strsAppend(&buffs, pikaAsm, buff);
    }
    if (NULL != operator) {
        __platform_sprintf(buff, "%d OPT %s\n", deepth, operator);
        pikaAsm = strsAppend(&buffs, pikaAsm, buff);
    }
    if (NULL != method) {
        __platform_sprintf(buff, "%d RUN %s\n", deepth, method);
        pikaAsm = strsAppend(&buffs, pikaAsm, buff);
    }
    if (NULL != str) {
        __platform_sprintf(buff, "%d STR %s\n", deepth, str);
        pikaAsm = strsAppend(&buffs, pikaAsm, buff);
    }
    if (NULL != num) {
        __platform_sprintf(buff, "%d NUM %s\n", deepth, num);
        pikaAsm = strsAppend(&buffs, pikaAsm, buff);
    }
    if (NULL != left) {
        __platform_sprintf(buff, "%d OUT %s\n", deepth, left);
        pikaAsm = strsAppend(&buffs, pikaAsm, buff);
    }
    obj_setInt(ast, "deepth", deepth - 1);
    goto exit;
exit:
    pikaAsm = strsCopy(outBuffs, pikaAsm);
    strsDeinit(&buffs);
    return pikaAsm;
}

char* ASM_addBlockDeepth(AST* ast,
                         Args* buffs_p,
                         char* pikaAsm,
                         uint8_t deepthOffset) {
    pikaAsm = strsAppend(buffs_p, pikaAsm, (char*)"B");
    char buff[11];
    pikaAsm = strsAppend(
        buffs_p, pikaAsm,
        fast_itoa(buff, obj_getInt(ast, "blockDeepth") + deepthOffset));
    pikaAsm = strsAppend(buffs_p, pikaAsm, (char*)"\n");
    return pikaAsm;
}

char* AST_toPikaASM(AST* ast, Args* outBuffs) {
    Args buffs = {0};
    char* pikaAsm = strsCopy(&buffs, "");
    QueueObj* exitBlock;
    uint8_t is_block_matched;
    if (NULL == ast) {
        pikaAsm = NULL;
        goto exit;
    }
    exitBlock = obj_getObj(ast, "exitBlock", 0);
    /* exiting from block */
    if (exitBlock != NULL) {
        while (1) {
            uint8_t blockTypeNum = obj_getInt(exitBlock, "top") -
                                   obj_getInt(exitBlock, "bottom") - 1;
            char* blockType = queueObj_popStr(exitBlock);
            if (NULL == blockType) {
                break;
            }
            /* goto the while start when exit while block */
            if (strEqu(blockType, "while")) {
                pikaAsm =
                    ASM_addBlockDeepth(ast, outBuffs, pikaAsm, blockTypeNum);
                pikaAsm = strsAppend(outBuffs, pikaAsm, (char*)"0 JMP -1\n");
            }
            /* goto the while start when exit while block */
            if (strEqu(blockType, "for")) {
                pikaAsm =
                    ASM_addBlockDeepth(ast, outBuffs, pikaAsm, blockTypeNum);
                pikaAsm = strsAppend(outBuffs, pikaAsm, (char*)"0 JMP -1\n");
                /* garbage collect for the list */
                pikaAsm =
                    ASM_addBlockDeepth(ast, outBuffs, pikaAsm, blockTypeNum);
                char _l_x[] = "_lx";
                char block_deepth_char =
                    obj_getInt(ast, "blockDeepth") + blockTypeNum + '0';
                _l_x[sizeof(_l_x) - 2] = block_deepth_char;
                pikaAsm = strsAppend(outBuffs, pikaAsm, (char*)"0 DEL ");
                pikaAsm = strsAppend(outBuffs, pikaAsm, (char*)_l_x);
                pikaAsm = strsAppend(outBuffs, pikaAsm, (char*)"\n");
            }
            /* return when exit method */
            if (strEqu(blockType, "def")) {
                pikaAsm = ASM_addBlockDeepth(ast, outBuffs, pikaAsm,
                                             blockTypeNum + 1);
                pikaAsm = strsAppend(outBuffs, pikaAsm, (char*)"0 RET \n");
            }
            /* return when exit class */
            if (strEqu(blockType, "class")) {
                pikaAsm = ASM_addBlockDeepth(ast, outBuffs, pikaAsm,
                                             blockTypeNum + 1);
                pikaAsm =
                    strsAppend(outBuffs, pikaAsm, (char*)"0 RAS $origin\n");
                pikaAsm = ASM_addBlockDeepth(ast, outBuffs, pikaAsm, 1);
                pikaAsm = strsAppend(outBuffs, pikaAsm, (char*)"0 NEW self\n");
                pikaAsm = strsAppend(outBuffs, pikaAsm, (char*)"0 RET \n");
            }
        }
    }
    /* add block deepth */
    /* example: B0 */
    pikaAsm = ASM_addBlockDeepth(ast, outBuffs, pikaAsm, 0);

    /* "deepth" is invoke deepth, not the blockDeepth */
    obj_setInt(ast, "deepth", 0);

    /* match block */
    is_block_matched = 0;
    if (strEqu(obj_getStr(ast, "block"), "for")) {
        /* for "for" iter */
        char* arg_in = obj_getStr(ast, "arg_in");
        Arg* newAsm_arg = arg_setStr(NULL, "", "");
        char _l_x[] = "_lx";
        char block_deepth_char = '0';
        block_deepth_char += obj_getInt(ast, "blockDeepth");
        _l_x[sizeof(_l_x) - 2] = block_deepth_char;
        /* init iter */
        /*     get the iter(_l<x>) */
        pikaAsm = AST_appandPikaASM(ast, ast, &buffs, pikaAsm);
        newAsm_arg = arg_strAppend(newAsm_arg, "0 OUT ");
        newAsm_arg = arg_strAppend(newAsm_arg, _l_x);
        newAsm_arg = arg_strAppend(newAsm_arg, "\n");
        if (1 == obj_getInt(ast, "isRange")) {
            newAsm_arg = arg_strAppend(newAsm_arg,
                                       "0 REF _r1\n"
                                       "0 REF _r2\n"
                                       "0 OUT ");
            newAsm_arg = arg_strAppend(newAsm_arg, _l_x);
            newAsm_arg = arg_strAppend(newAsm_arg,
                                       ".a2\n"
                                       "0 OUT ");
            newAsm_arg = arg_strAppend(newAsm_arg, _l_x);
            newAsm_arg = arg_strAppend(newAsm_arg, ".a1\n");
        }
        pikaAsm = strsAppend(&buffs, pikaAsm, arg_getStr(newAsm_arg));
        arg_deinit(newAsm_arg);
        newAsm_arg = arg_setStr(NULL, "", "");
        /* get next */
        /*     run next(_l<x>) */
        /*     check item is exist */
        pikaAsm = ASM_addBlockDeepth(ast, outBuffs, pikaAsm, 0);
        newAsm_arg = arg_strAppend(newAsm_arg, "0 RUN ");
        newAsm_arg = arg_strAppend(newAsm_arg, _l_x);
        newAsm_arg = arg_strAppend(newAsm_arg,
                                   ".__next__\n"
                                   "0 OUT ");
        newAsm_arg = arg_strAppend(newAsm_arg, arg_in);
        newAsm_arg = arg_strAppend(newAsm_arg,
                                   "\n"
                                   "0 EST ");
        newAsm_arg = arg_strAppend(newAsm_arg, arg_in);
        newAsm_arg = arg_strAppend(newAsm_arg, "\n0 JEZ 2\n");
        pikaAsm = strsAppend(&buffs, pikaAsm, arg_getStr(newAsm_arg));
        arg_deinit(newAsm_arg);
        is_block_matched = 1;
        goto exit;
    }
    if (strEqu(obj_getStr(ast, "block"), "while")) {
        /* parse stmt ast */
        pikaAsm = AST_appandPikaASM(ast, ast, &buffs, pikaAsm);
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 JEZ 2\n");
        is_block_matched = 1;
        goto exit;
    }
    if (strEqu(obj_getStr(ast, "block"), "if")) {
        /* parse stmt ast */
        pikaAsm = AST_appandPikaASM(ast, ast, &buffs, pikaAsm);
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 JEZ 1\n");
        is_block_matched = 1;
        goto exit;
    }
    if (strEqu(obj_getStr(ast, "block"), "else")) {
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 NEL 1\n");
        goto exit;
    }
    if (strEqu(obj_getStr(ast, "block"), "elif")) {
        /* skip if __else is 0 */
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 NEL 1\n");
        /* parse stmt ast */
        pikaAsm = AST_appandPikaASM(ast, ast, &buffs, pikaAsm);
        /* skip if stmt is 0 */
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 JEZ 1\n");
        is_block_matched = 1;
        goto exit;
    }
    if (strEqu(obj_getStr(ast, "block"), "def")) {
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 DEF ");
        pikaAsm = strsAppend(&buffs, pikaAsm, obj_getStr(ast, "declear"));
        pikaAsm = strsAppend(&buffs, pikaAsm,
                             "\n"
                             "0 JMP 1\n");
        is_block_matched = 1;
        goto exit;
    }

    if (strEqu(obj_getStr(ast, "block"), "class")) {
        char* declear = obj_getStr(ast, "declear");
        char* thisClass = NULL;
        char* superClass = NULL;
        if (strIsContain(declear, '(')) {
            thisClass = strsGetFirstToken(&buffs, declear, '(');
            superClass = strsCut(&buffs, declear, '(', ')');
        } else {
            thisClass = declear;
            superClass = "";
        }
        if (strEqu("", superClass)) {
            /* default superClass */
            superClass = "PikaStdLib.PikaObj";
        }
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 DEF ");
        pikaAsm = strsAppend(&buffs, pikaAsm,
                             strsAppend(&buffs, thisClass,
                                        "()\n"
                                        "0 JMP 1\n"));
        char block_deepth_str[] = "B0\n";
        /* goto deeper block */
        block_deepth_str[1] += obj_getInt(ast, "blockDeepth") + 1;
        pikaAsm = strsAppend(&buffs, pikaAsm, block_deepth_str);
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 RUN ");
        pikaAsm = strsAppend(&buffs, pikaAsm, superClass);
        pikaAsm = strsAppend(&buffs, pikaAsm, "\n");
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 OUT self\n");
        pikaAsm = strsAppend(&buffs, pikaAsm, block_deepth_str);
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 RAS self\n");
        is_block_matched = 1;
        goto exit;
    }

    if (obj_isArgExist(ast, "return")) {
        /* parse stmt ast */
        pikaAsm = AST_appandPikaASM(ast, ast, &buffs, pikaAsm);
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 RET \n");
        is_block_matched = 1;
        goto exit;
    }
    if (obj_isArgExist(ast, "global")) {
        /* parse stmt ast */
        pikaAsm = AST_appandPikaASM(ast, ast, &buffs, pikaAsm);
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 GLB ");
        pikaAsm = strsAppend(&buffs, pikaAsm, obj_getStr(ast, "global"));
        pikaAsm = strsAppend(&buffs, pikaAsm, "\n");
        is_block_matched = 1;
        goto exit;
    }
    if (obj_isArgExist(ast, "break")) {
        /* parse stmt ast */
        pikaAsm = AST_appandPikaASM(ast, ast, &buffs, pikaAsm);
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 BRK\n");
        is_block_matched = 1;
        goto exit;
    }
    if (obj_isArgExist(ast, "continue")) {
        /* parse stmt ast */
        pikaAsm = AST_appandPikaASM(ast, ast, &buffs, pikaAsm);
        pikaAsm = strsAppend(&buffs, pikaAsm, "0 CTN\n");
        is_block_matched = 1;
        goto exit;
    }
exit:
    if (NULL == pikaAsm) {
        strsDeinit(&buffs);
        return NULL;
    }
    if (!is_block_matched) {
        /* parse stmt ast */
        pikaAsm = AST_appandPikaASM(ast, ast, &buffs, pikaAsm);
    }

    /* output pikaAsm */
    pikaAsm = strsCopy(outBuffs, pikaAsm);
    strsDeinit(&buffs);
    return pikaAsm;
}

int32_t AST_deinit(AST* ast) {
    return obj_deinit(ast);
}

ByteCodeFrame* byteCodeFrame_appendFromAsm(ByteCodeFrame* self, char* pikaAsm) {
    Asmer asmer = {
        .asm_code = pikaAsm,
        .block_deepth_now = 0,
        .is_new_line = 0,
        .line_pointer = pikaAsm,
    };
    uint16_t const_pool_offset;
    char* data;
    uint16_t exist_offset;

    char line_buff[PIKA_PATH_BUFF_SIZE] = {0};
    for (int i = 0; i < strCountSign(pikaAsm, '\n'); i++) {
        char* line = strGetLine(line_buff, asmer.line_pointer);
        InstructUnit ins_unit = {0};
        /* remove '\r' */
        if (line[strGetSize(line) - 1] == '\r') {
            line[strGetSize(line) - 1] = 0;
        }
        /* process block deepth flag*/
        if ('B' == line[0]) {
            asmer.block_deepth_now = line[1] - '0';
            asmer.is_new_line = 1;
            goto next_line;
        }

        /* process each ins */

        /* get constPool offset */
        const_pool_offset = 0;

        data = line + 6;
        exist_offset = constPool_getOffsetByData(&(self->const_pool), data);

        /* get const offset */
        if (strEqu(data, "")) {
            /* not need const value */
            const_pool_offset = 0;
        } else if (65535 == exist_offset) {
            /* push new const value */
            const_pool_offset = constPool_getLastOffset(&(self->const_pool));
            /* load const to const pool buff */
            constPool_append(&(self->const_pool), data);
        } else {
            /* use exist const value */
            const_pool_offset = exist_offset;
        }

        /* load Asm to byte code unit */
        instructUnit_setBlockDeepth(&ins_unit, asmer.block_deepth_now);
        instructUnit_setInvokeDeepth(&ins_unit, line[0] - '0');
        instructUnit_setConstPoolIndex(&ins_unit, const_pool_offset);
        instructUnit_setInstruct(&ins_unit, pikaVM_getInstructFromAsm(line));
        if (asmer.is_new_line) {
            instructUnit_setIsNewLine(&ins_unit, 1);
            asmer.is_new_line = 0;
        }

        /* append instructUnit to instructArray */
        instructArray_append(&(self->instruct_array), &ins_unit);

    next_line:
        /* point to next line */
        asmer.line_pointer += strGetLineSize(asmer.line_pointer) + 1;
    }
    return self;
}

char* Parser_byteCodeToAsm(Args* outBuffs, char* pikaByteCode) {
    return NULL;
}

void Parser_compilePyToBytecodeArray(char* lines) {
    ByteCodeFrame bytecode_frame;
    byteCodeFrame_init(&bytecode_frame);
    bytecodeFrame_fromMultiLine(&bytecode_frame, lines);
    /* do something */
    byteCodeFrame_print(&bytecode_frame);
    byteCodeFrame_printAsArray(&bytecode_frame);
    /* deinit */
    byteCodeFrame_deinit(&bytecode_frame);
}

/* const Pool output redirect */
static void __handler_constPool_output_file(ConstPool* self, char* content) {
    /* to ram */
    uint16_t size = strGetSize(content) + 1;
    self->arg_buff = arg_append(self->arg_buff, content, size);
    /* to flash */
    __platform_fwrite(content, 1, size, self->output_f);
}

/* instruct array output redirect */
static void __handler_instructArray_output_none(InstructArray* self,
                                                InstructUnit* ins_unit) {
    /* none */
}

static void __handler_instructArray_output_file(InstructArray* self,
                                                InstructUnit* ins_unit) {
    /* to flash */
    __platform_fwrite(ins_unit, 1, instructUnit_getSize(), self->output_f);
}

/*
    need implament :
        __platform_fopen()
        __platform_fwrite()
        __platform_fclose()
*/
int pikaCompile(char* output_file_name, char* py_lines) {
    ByteCodeFrame bytecode_frame = {0};

    FILE* bytecode_f = __platform_fopen(output_file_name, "w+");
    /* main process */

    /* step 1, get size of const pool and instruct array */
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_redirect_fun =
        __handler_instructArray_output_none;
    Parser_parsePyLines(NULL, &bytecode_frame, py_lines);
    uint16_t const_pool_size = bytecode_frame.const_pool.size;
    uint16_t instruct_array_size = bytecode_frame.instruct_array.size;
    byteCodeFrame_deinit(&bytecode_frame);

    /* step 2, write instruct array to file */
    __platform_fwrite(&instruct_array_size, 1, 2, bytecode_f);
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_f = bytecode_f;
    /* instruct array to file */
    bytecode_frame.instruct_array.output_redirect_fun =
        __handler_instructArray_output_file;
    Parser_parsePyLines(NULL, &bytecode_frame, py_lines);
    byteCodeFrame_deinit(&bytecode_frame);

    /* step 3, write const pool to file */
    __platform_fwrite(&const_pool_size, 1, 2, bytecode_f);
    char void_ = 0;
    /* add \0 at the start */
    __platform_fwrite(&void_, 1, 1, bytecode_f);
    byteCodeFrame_init(&bytecode_frame);
    bytecode_frame.const_pool.output_f = bytecode_f;
    bytecode_frame.instruct_array.output_f = bytecode_f;
    /* const pool to file */
    bytecode_frame.const_pool.output_redirect_fun =
        __handler_constPool_output_file;
    /* instruct array to none */
    bytecode_frame.instruct_array.output_redirect_fun =
        __handler_instructArray_output_none;
    Parser_parsePyLines(NULL, &bytecode_frame, py_lines);
    byteCodeFrame_deinit(&bytecode_frame);

    /* deinit */
    __platform_fclose(bytecode_f);
    /* succeed */
    return 0;
};
