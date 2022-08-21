#define __OOC_CPP__
#define __OOC_DEBUG__

#include "gtest/gtest.h"
extern "C" {
#include "BaseObj.h"
#include "PikaCompiler.h"
#include "PikaMain.h"
#include "PikaParser.h"
#include "PikaStdLib_MemChecker.h"
#include "PikaMath_Operator.h"
#include "PikaStdLib_SysObj.h"
#include "PikaVM.h"
#include "TinyObj.h"
#include "dataArgs.h"
#include "dataMemory.h"
#include "dataStrs.h"
#include "pikaScript.h"
#include "pika_config_gtest.h"
extern PikaMemInfo pikaMemInfo;
/* the log_buff of printf */
extern char log_buff[LOG_BUFF_MAX][LOG_SIZE];
}
