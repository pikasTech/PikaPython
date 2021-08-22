/* this demo shows the usage of method */

#include "BaseObj.h"
#include <stdio.h>

void onMethod(PikaObj *self, Args *args)
{
	/* turn on the led */
	printf("the led is on! \r\n");
}

void offMethod(PikaObj *self, Args *args)
{
	/* turn off the led */
	printf("the led is off! \r\n");
}

PikaObj *New_LED(Args *args)
{
	/*	Derive from the tiny object class.
		Tiny object can not import sub object.	
		Tiny object is the smallest object. */
	PikaObj *self = New_TinyObj(args);

	/* bind the method */
	class_defineMethod(self, "on()", onMethod);
	class_defineMethod(self, "off()", offMethod);

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
	obj_import(self, "LED", New_LED);

	/* new led object bellow root object */
	obj_newObj(self, "led", "LED");

	/* return the object */
	return self;
}

int32_t main()
{
	/* new root object */
	PikaObj *root = newRootObj("root", New_MYROOT);
	/* user input buff */
	char inputBuff[256] = {0};
	/* run the script with check*/
	obj_run(root, "led.on()");
	printf("memory used max = %0.2f kB\r\n", pikaMemMax() / 1024.0);
	printf("memory used now = %0.2f kB\r\n", pikaMemNow() / 1024.0);
	while (1)
	{
		/* get user input */
		fgets(inputBuff, sizeof(inputBuff), stdin);

		/* run PikaScript and get res */
		Args *resArgs = obj_runDirect(root, inputBuff);

		/* get system output of PikaScript*/
		char *sysOut = args_getSysOut(resArgs);;

		if (NULL != sysOut)
		{
			/* print32_t out the system output */
			printf("%s\r\n", sysOut);
		}

		/* deinit the res */
		args_deinit(resArgs);
	}
}
