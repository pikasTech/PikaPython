#include "sysObj.h"
#include <stdio.h>

void onFun(MimiObj *self, Args *args)
{
	/* turn on the led */
	printf("the led is on! \r\n");
}

void offFun(MimiObj *self, Args *args)
{
	/* turn off the led */
	printf("the led is off! \r\n");
}

MimiObj *New_LED(Args *args)
{
	/* derive from the base mimiObj class */
	MimiObj *self = New_MimiObj_sys(args);

	/* bind the method */
	class_defineMethod(self, "on()", onFun);
	class_defineMethod(self, "off()", offFun);

	/* return the object */
	return self;
}

MimiObj *New_MYROOT(Args *args)
{
	/* derive from the base mimiObj class */
	MimiObj *self = New_MimiObj_sys(args);

	/* import LED class */
	obj_import(self, "LED", New_LED);

	/* new led object bellow root object */
	obj_newObj(self, "led", "LED");

	/* return the object */
	return self;
}

int main()
{
	/* new root object */
	MimiObj *root = newRootObj("root", New_MYROOT);
	/* user input buff */
	char inputBuff[256] = {0};
	/* run the script with check*/
	obj_run(root, "led.on()");
	while (1)
	{
		/* get user input */
		fgets(inputBuff, sizeof(inputBuff), stdin);

		/* run mimiScript and get res */
		Args *res = obj_runDirect(root, inputBuff);

		/* get system output of mimiScript*/
		char *sysOut = args_getStr(res, "sysOut");

		if (NULL != sysOut)
		{
			/* print out the system output */
			printf("%s\r\n", sysOut);
		}

		/* deinit the res */
		args_deinit(res);
	}
}
