/***************************************************************************** 
* 
* File Name : main.c
* 
* Description: main 
* 
* Copyright (c) 2014 Winner Micro Electronic Design Co., Ltd. 
* All rights reserved. 
* 
* Author : dave
* 
* Date : 2014-6-14
*****************************************************************************/ 
#include "wm_include.h"
#include "pikaScript.h"
void UserMain(void)
{
	printf("[info]: w801 system init ok.\r\n");
	PikaObj *pikaMain = pikaScriptInit();
	while(1){
	}

#if DEMO_CONSOLE
	CreateDemoTask();
#endif
//用户自己的task
}

