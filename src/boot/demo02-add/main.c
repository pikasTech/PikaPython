/* this demo shows the usage of input and return of method */

#include "BaseObj.h"
#include <stdio.h>

void addMethod(PikaObj *self, Args* args)
{
	int32_t val1 = args_getInt(args, "val1");
	int32_t val2 = args_getInt(args, "val2");
	int32_t res = val1 + val2;
	method_returnInt(args, res);
}

PikaObj *New_TEST(Args *args)
{
	/*	Derive from the tiny object class.
		Tiny object can not import sub object.	
		Tiny object is the smallest object. */
	PikaObj *self = New_TinyObj(args);

	/* bind the method */
	class_defineMethod(self, "add(val1:int, val2:int)->int", addMethod);

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
	obj_import(self, "TEST", New_TEST);

	/* new led object bellow root object */
	obj_newObj(self, "test", "TEST");

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
	obj_run(root, "res = test.add(1, 2)");
	int32_t res = obj_getInt(root, "res");
	printf("the res of 'test.add(1, 2)' is %d \r\n", res);
	printf("memory used max = %0.2f kB\r\n", DMEMS.heapUsedMax / 1024.0);
	printf("memory used now = %0.2f kB\r\n", DMEMS.heapUsed / 1024.0);
	while (1)
	{
		/* get user input */
		fgets(inputBuff, sizeof(inputBuff), stdin);

		/* run PikaScript and get res */
		Args *resArgs = obj_runDirect(root, inputBuff);

		/* get system output of PikaScript*/
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
