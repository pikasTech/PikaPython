/* this demo shows the usage of method */

#include "SysObj.h"
#include <stdio.h>

void obj_runWithInfo(PikaObj *self, char *cmd)
{
	printf(">>> %s\r\n", cmd);
	obj_run(self, cmd);
}

extern DMEM_STATE DMEMS;

int main()
{
	/* new root object */
	PikaObj *root = newRootObj("root", New_SysObj);

	obj_runWithInfo(root, "set('a',1)");
	obj_runWithInfo(root, "print(a)");
	obj_runWithInfo(root, "type('a')");
	obj_runWithInfo(root, "del('a')");

	obj_runWithInfo(root, "set('a','test')");
	obj_runWithInfo(root, "print(a)");
	obj_runWithInfo(root, "type('a')");
	obj_runWithInfo(root, "del('a')");

	obj_runWithInfo(root, "set('a',1)");
	obj_runWithInfo(root, "set('b',a)");
	obj_runWithInfo(root, "print(b)");
	obj_runWithInfo(root, "del('a')");
	obj_runWithInfo(root, "del('b')");

	printf("memory used max = %0.2f kB\r\n", DMEMS.heapUsedMax / 1024.0);
	printf("memory used now = %0.2f kB\r\n", DMEMS.heapUsed / 1024.0);

	/* user input buff */
	char inputBuff[256] = {0};
	/* run the script with check*/
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
