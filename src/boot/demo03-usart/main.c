/* this demo shows the usage of string arg in method */

#include "BaseObj.h"
#include <stdio.h>

void sendMethod(PikaObj *self, Args *args)
{
	char *data = args_getStr(args, "data");
	/* send to com1 */
	printf("[com1]: %s\r\n", data);
}

PikaObj *New_USART(Args *args)
{
	/*	Derive from the tiny object class.
		Tiny object can not import sub object.	
		Tiny object is the smallest object. */
	PikaObj *self = New_TinyObj(args);

	/* bind the method */
	class_defineMethod(self, "send(data:str)", sendMethod);

	/* return the object */
	return self;
}

PikaObj *New_MYROOT(Args *args)
{
	/*	Derive from the base object class .
		BaseObj is the smallest object that can 
		import sub object.		*/
	PikaObj *self = New_BaseObj(args);

	/* import LED class */
	obj_import(self, "USART", New_USART);

	/* new led object bellow root object */
	obj_newObj(self, "usart", "USART");

	/* return the object */
	return self;
}

extern DMEM_STATE DMEMS;
int32_t main()
{
	/* new root object */
	PikaObj *root = newRootObj("root", New_MYROOT);
	/* user input buff */
	char inputBuff[256] = {0};
	/* run the script with check*/
	obj_run(root, "res = usart.send('hello world')");

	printf("memory used max = %0.2f kB\r\n", DMEMS.heapUsedMax / 1024.0);
	printf("memory used now = %0.2f kB\r\n", DMEMS.heapUsed / 1024.0);
	while (1)
	{
		/* get user input */
		fgets(inputBuff, sizeof(inputBuff), stdin);

		/* run mimiScript and get res */
		Args *resArgs = obj_runDirect(root, inputBuff);

		/* get system output of mimiScript*/
		char *sysOut = args_getStr(resArgs, "sysOut");

		if (NULL != sysOut)
		{
			/* print32_t out the system output */
			printf("%s\r\n", sysOut);
		}

		/* deinit the res */
		args_deinit(resArgs);
	}
}
