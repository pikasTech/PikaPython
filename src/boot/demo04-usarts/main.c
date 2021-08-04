/* this demo shows the usage of derive and override */

#include "baseObj.h"
#include <stdio.h>

void sendMethod(MimiObj *self, Args *args)
{
	char *data = args_getStr(args, "data");
	/* send to com1 */
	printf("[com1]: %s\r\n", data);
}

void setSpeedMethod(MimiObj *self , Args *args)
{
	int speed = args_getInt(args, "speed");
	obj_setInt(self, "speed", speed);
}

void printSpeedMethod(MimiObj *self , Args *args)
{
	int speed = obj_getInt(self, "speed");
	printf("%d\r\n", speed);
}

MimiObj *New_USART(Args *args)
{
	/*	Derive from the tiny object class.
		Tiny object can not import sub object.	
		Tiny object is the smallest object. */
	MimiObj *self = New_TinyObj(args);

	/* setArgs */
	obj_setInt(self, "speed", 9600);

	/* bind the method */
	class_defineMethod(self, "send(data:string)", sendMethod);
	class_defineMethod(self, "setSpeed(speed:int)", setSpeedMethod);
	class_defineMethod(self, "printSpeed()", printSpeedMethod);

	/* return the object */
	return self;
}

void sendFun2(MimiObj *self, Args *args)
{
	char *data = args_getStr(args, "data");
	/* send to com1 */
	printf("[com2]: %s\r\n", data);
}

MimiObj *New_USART2(Args *args)
{
	/*	Derive from the usart class.*/
	MimiObj *self = New_USART(args);

	/* override the method */
	class_defineMethod(self, "send(data:string)", sendFun2);

	/* return the object */
	return self;
}

MimiObj *New_MYROOT(Args *args)
{
	/*	Derive from the base object class .
		BaseObj is the smallest object that can 
		import sub object.		*/
	MimiObj *self = New_baseObj(args);

	/* import LED class */
	obj_import(self, "USART", New_USART);
	obj_import(self, "USART2", New_USART2);

	/* new object bellow root object */
	obj_newObj(self, "usart1", "USART");
	obj_newObj(self, "usart2", "USART2");

	/* return the object */
	return self;
}

extern DMEM_STATE DMEMS;
int main()
{
	/* new root object */
	MimiObj *root = newRootObj("root", New_MYROOT);
	/* user input buff */
	char inputBuff[256] = {0};

	/* usart and usart2 can also be use the method in USART */
	printf("the speed of usart1 before set:");
	obj_run(root, "usart1.printSpeed()");

	printf("the speed of usart2 before set:");
	obj_run(root, "usart2.printSpeed()");

	obj_run(root, "usart1.setSpeed(115200)");
	obj_run(root, "usart2.setSpeed(284000)");

	printf("the speed of usart1 after set:");
	obj_run(root, "usart1.printSpeed()");

	printf("the speed of usart2 after set:");
	obj_run(root, "usart2.printSpeed()");

	/* the send method is override in usart2 */
	obj_run(root, "usart1.send('hello world')");
	obj_run(root, "usart2.send('hello world')");

	printf("memory used max = %0.2f kB\r\n", DMEMS.maxNum * DMEM_BLOCK_SIZE / 1024.0);
	printf("memory used now = %0.2f kB\r\n", DMEMS.blk_num * DMEM_BLOCK_SIZE / 1024.0);
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
			/* print out the system output */
			printf("%s\r\n", sysOut);
		}

		/* deinit the res */
		args_deinit(resArgs);
	}
}
