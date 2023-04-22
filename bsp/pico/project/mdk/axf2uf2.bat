@echo off
set MDK_ROOT=%1

@echo on
"..\..\tool\elf2uf2.exe" .\Objects\template.axf .\template.uf2

@if not exist "%MDK_ROOT%Arm\Flash\Raspberry_Pi_Pico.FLM" (

    @echo ==================================================
    @echo Try to install RP2040 flash programming algorithm...
    @echo If you didn't see "1 file(s) copied", please manually copy Raspberry_Pi_Pico.FLM to "<KEIL Folder>\Arm\Flash", e.g. "C:\Keil_v5\Arm\Flash\"
    @echo ==================================================
    @copy /B ".\Raspberry_Pi_Pico.FLM" "%MDK_ROOT%Arm\Flash\Raspberry_Pi_Pico.FLM"

)