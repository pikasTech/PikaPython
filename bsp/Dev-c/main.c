#include <stdio.h>
#include <stdlib.h>
#include "pikascript.h"
 
/* run this program using the console pauser or add your own getch, system("pause") or input loop */

int main(int argc, char *argv[]) {
	PikaObj* pikaMain = pikaScriptInit();
	pikaScriptShell(pikaMain);
	return 0;
}

char __platfrom_getChar(void){
	return (char)getchar();
}

