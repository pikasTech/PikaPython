#define __OOC_CPP__
#define __OOC_DEBUG__

extern "C" {
#include <stdio.h>
#include "PikaCompiler.h"
#include "PikaMain.h"
#include "PikaMath_Operator.h"
#include "BaseObj.h"
#include "pika_hal.h"
#include "PikaParser.h"
#include "PikaStdLib_MemChecker.h"
#include "PikaStdLib_SysObj.h"
#include "PikaVM.h"
#include "TinyObj.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataStrs.h"
#include "pikaScript.h"
#include "pika_config_gtest.h"
extern PikaMemInfo g_PikaMemInfo;
/* the log_buff of printf */
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];
}

#if USE_GOOGLE_TEST
#include "gtest/gtest.h"
#define TEST_START
#define TEST_END
#else
#include "test_purec.h"
#endif
