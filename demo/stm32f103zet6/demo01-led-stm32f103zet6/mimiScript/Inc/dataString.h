#ifndef __MY_TEST_TOOLS_H 
#define __MY_TEST_TOOLS_H 

/* size */
unsigned short int strGetSize(char *pData);
/* append */
char *strAppend(char *strOut, char *pData);
char *strAppend_unlimited(char *strOut, char *pData);
char *strAppendWithSize(char *strOut, char *pData, int Size);
/* cut */
char *strCut(char *strOut, char *strIn, char startSign, char endSign);
/* assert */
int strIsStartWith(char *str, char *strStart);
int strEqu(char *str1, char *str2);
/* delete */
char *strDeleteEnter(char *str);
char *strDeleteChar(char *strOut, char *strIn, char ch);
/* prefix */
char *strRemovePrefix(char *inputStr, char *prefix, char *outputStr);
/* token */
int strGetToken(char *string, char **argv, char sign);
char *strPopToken(char *strOut, char *strIn, char sign);
int strCountSign(char *strIn, char sign);
int strGetTokenNum(char *strIn, char sign);
char *strGetFirstToken(char *strOut, char *strIn, char sign);
char *strGetLastToken(char *strOut, char *strIn, char sign);
char *strClear(char *str);
int strIsContain(char *str, char ch);
char *strCopy(char *strBuff, char *strIn);

#endif
