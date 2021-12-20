#include "PikaParser.h"
#include "dataStrs.h"

char* __pikaExtern_beforeParseLine(Args* outBuffs, char* line) {
    Args* buffs = New_strBuff();
    char* outLine = line;
    if (0 == strncmp(line, (char*)"for ", 4)) {
        char* tokens = Lexer_getTokens(buffs, line + 4);
        Lexer_popToken(buffs, tokens);
        char* arg_name = Lexer_popToken(buffs, tokens) + 1;
        Lexer_popToken(buffs, tokens);
        char* list_name = Lexer_popToken(buffs, tokens) + 1;
        outLine = strsCopy(buffs, "while ");
        outLine = strsAppend(buffs, outLine, list_name);
        outLine = strsAppend(buffs, outLine, ".next() != -9999:");
        goto output;
    }
output:
    if (outLine != line) {
        line = strsCopy(outBuffs, outLine);
    }
    args_deinit(buffs);
    return line;
}