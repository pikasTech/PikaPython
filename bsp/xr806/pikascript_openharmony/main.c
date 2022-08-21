#include "ohos_init.h"
#include "kernel/os/os.h"
#include "pikascript/pikascript-api/pikaScript.h"

static OS_Thread_t g_main_thread;

/* support pikaScript Shell */

static void MainThread(void *arg)
{
  
  PikaObj* pikamain = pikaScriptInit();
	while (1) {
		
	}

}

void HelloTestMain(void)
{
	printf("pikascript Test Start\n");

	if (OS_ThreadCreate(&g_main_thread, "MainThread", MainThread, NULL,
			    OS_THREAD_PRIO_APP, 4 * 1024) != OS_OK) {
		printf("[ERR] Create MainThread Failed\n");
	}
}

SYS_RUN(HelloTestMain);
