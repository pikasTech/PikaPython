@cd RTE/PikaScript

@if not exist pikascript-core (
    @pikaPackage.exe
)
@rust-msc-latest-win10.exe

@copy /B "pikascript-api\*.*"
@copy /B "pikascript-core\*.*"
@copy /B "pikascript-lib\PikaStdLib\*.*"

@echo clean up...
@del "pikascript-api\PikaDebug.h"
@del "pikascript-api\PikaDebug_Debuger.h"
@del "pikascript-api\PikaMain.h"
@del "pikascript-api\pikaScript.h"
@del "pikascript-api\PikaStdData.h"
@del "pikascript-api\PikaStdData_ByteArray.h"
@del "pikascript-api\PikaStdData_Dict.h"
@del "pikascript-api\PikaStdData_List.h"
@del "pikascript-api\PikaStdData_String.h"
@del "pikascript-api\PikaStdData_Utils.h"
@del "pikascript-api\PikaStdLib.h"
@del "pikascript-api\PikaStdLib_MemChecker.h"
@del "pikascript-api\PikaStdLib_PikaObj.h"
@del "pikascript-api\PikaStdLib_RangeObj.h"
@del "pikascript-api\PikaStdLib_StringObj.h"
@del "pikascript-api\PikaStdLib_SysObj.h"
@del "pikascript-api\PikaStdTask.h"
@del "pikascript-api\PikaStdTask_Task.h"

@del "pikascript-api\__asset_pikaModules_py_a.c"
@del "pikascript-api\PikaDebug_Debuger-api.c"
@del "pikascript-api\PikaDebug-api.c"
@del "pikascript-api\PikaMain-api.c"
@del "pikascript-api\pikaScript.c"
@del "pikascript-api\PikaStdData_ByteArray-api.c"
@del "pikascript-api\PikaStdData_Dict-api.c"
@del "pikascript-api\PikaStdData_List-api.c"
@del "pikascript-api\PikaStdData_String-api.c"
@del "pikascript-api\PikaStdData_Utils-api.c"
@del "pikascript-api\PikaStdData-api.c"
@del "pikascript-api\PikaStdLib_MemChecker-api.c"
@del "pikascript-api\PikaStdLib_PikaObj-api.c"
@del "pikascript-api\PikaStdLib_RangeObj-api.c"
@del "pikascript-api\PikaStdLib_StringObj-api.c"
@del "pikascript-api\PikaStdLib_SysObj-api.c"
@del "pikascript-api\PikaStdLib-api.c"
@del "pikascript-api\PikaStdTask_Task-api.c"
@del "pikascript-api\PikaStdTask-api.c"
@del "pikascript-api\compiler-info.txt"
@del "pikascript-api\main.py.o"
@del "pikascript-api\pikaModules.py.a"