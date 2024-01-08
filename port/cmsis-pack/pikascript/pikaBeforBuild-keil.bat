@cd RTE/PikaScript

@if not exist pikascript-core (
    @pikaPackage.exe
)
@rust-msc-latest-win10.exe

@copy /B "pikascript-api\*.c"
@copy /B "pikascript-api\*.h"
@copy /B "pikascript-core\*.*"
@copy /B "pikascript-lib\PikaStdLib\*.*"
@copy /B "pikascript-lib\pika_lvgl\*.*"

@echo clean up...
@clean.bat