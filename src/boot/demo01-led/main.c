/* this demo shows the usage of method */

#include "BaseObj.h"
#include <stdio.h>

void onMethod(MimiObj *self, Args *args)
{
	/* turn on the led */
	printf("the led is on! \r\n");
}

void offMethod(MimiObj *self, Args *args)
{
	/* turn off the led */
	printf("the led is off! \r\n");
}

MimiObj *New_LED(Args *args)
{
	/*	Derive from the tiny object class.
		Tiny object can not import sub object.	
		Tiny object is the smallest object. */
	MimiObj *self = New_TinyObj(args);

	/* bind the method */
	class_defineMethod(self, "on()", onMethod);
	class_defineMethod(self, "off()", offMethod);

	/* return the object */
	return self;
}

MimiObj *New_MYROOT(Args *args)
{
	/*	Derive from the base object class .
		BaseObj is the smallest object that can 
		import sub object.		*/
	MimiObj *self = New_BaseObj(args);

	/* import LED class */
	obj_import(self, "LED", New_LED);

	/* new led object bellow root object */
	obj_newObj(self, "led", "LED");

	/* return the object */
	return self;
}

extern DMEM_STATE DMEMS;
int32_t main()
{
	/* new root object */
	MimiObj *root = newRootObj("root", New_MYROOT);
	/* user input buff */
	char inputBuff[256] = {0};
	/* run the script with check*/
	obj_run(root, "led.on()");
	printf("memory used max = %0.2f kB\r\n", DMEMS.maxNum*DMEM_BLOCK_SIZE/1024.0);
	printf("memory used now = %0.2f kB\r\n", DMEMS.blk_num*DMEM_BLOCK_SIZE/1024.0);
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
