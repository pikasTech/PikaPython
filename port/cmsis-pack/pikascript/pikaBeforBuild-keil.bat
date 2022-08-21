@cd RTE/PikaScript

@if not exist pikascript-core (
    @pikaPackage.exe
)
@rust-msc-latest-win10.exe

@copy /B "pikascript-api\*.*"
@copy /B "pikascript-core\*.*"
@copy /B "pikascript-lib\PikaStdLib\*.*"

@echo clean up...
@clean.bat