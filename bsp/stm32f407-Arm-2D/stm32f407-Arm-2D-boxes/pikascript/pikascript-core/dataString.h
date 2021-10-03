#ifndef __MY_TEST_TOOLS_H 
#define __MY_TEST_TOOLS_H 
#include <stdint.h>

/* size */
uint32_t strGetSize(char *pData);
/* append */
char *strAppend(char *strOut, char *pData);
char *strAppend_unlimited(char *strOut, char *pData);
char *strAppendWithSize(char *strOut, char *pData,int32_t Size);
/* cut */
char *strCut(char *strOut, char *strIn, char startSign, char endSign);
/* assert */
int32_t strIsStartWith(char *str, char *strStart);
int32_t strEqu(char *str1, char *str2);
/* delete */
char *strDeleteEnter(char *str);
char *strDeleteChar(char *strOut, char *strIn, char ch);
/* prefix */
char *strRemovePrefix(char *inputStr, char *prefix, char *outputStr);
/* token */
int32_t strGetToken(char *string, char **argv, char sign);
char *strPopToken(char *strOut, char *strIn, char sign);
int32_t strCountSign(char *strIn, char sign);
int32_t strGetTokenNum(char *strIn, char sign);
char *strGetFirstToken(char *strOut, char *strIn, char sign);
char *strGetLastToken(char *strOut, char *strIn, char sign);
char *strClear(char *str);
int32_t strIsContain(char *str, char ch);
char *strCopy(char *strBuff, char *strIn);

#endif
