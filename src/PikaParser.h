#ifndef __PIKA__PARSER__H
#define __PIKA__PARSER__H
#include "dataQueueObj.h"
#include "dataStack.h"

typedef QueueObj AST;
AST* pikaParseLine(char* line, Stack* blockStack);
char* pikaParseLineToAsm(Args* buffs, char* line, Stack* blockStack);
int32_t AST_deinit(AST* ast);
char* AST_toPikaAsm(AST* ast, Args* buffs);
char* pikaParseMultiLineToAsm(Args* outBuffs, char* multiLine);

#endif