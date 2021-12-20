#include "PikaParser.h"
#include "dataStrs.h"

char* __pikaExtern_beforeParseLine(Args* outBuffs, char* line) {
    Args * buffs = New_strBuff();
    // char* tokens = Lexer_getTokens(buffs, line);
    // char* print_tokens = Lexer_printTokens(buffs, tokens);
output:
    args_deinit(buffs);
    return line;
}