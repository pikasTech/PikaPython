##
## Auto Generated makefile by CDK
## Do not modify this file, and any manual changes will be erased!!!   
##
## BuildSet
ProjectName            :=W806_SDK
ConfigurationName      :=BuildSet
WorkspacePath          :=../
ProjectPath            :=./
IntermediateDirectory  :=Obj
OutDir                 :=$(IntermediateDirectory)
User                   :=liuyan
Date                   :=2022/1/23
CDKPath                :=../../../../../../../../C-Sky/CDK
LinkerName             :=csky-elfabiv2-gcc
LinkerNameoption       :=
SIZE                   :=csky-elfabiv2-size
READELF                :=csky-elfabiv2-readelf
CHECKSUM               :=crc32
SharedObjectLinkerName :=
ObjectSuffix           :=.o
DependSuffix           :=.d
PreprocessSuffix       :=.i
DisassemSuffix         :=.asm
IHexSuffix             :=.ihex
BinSuffix              :=.bin
ExeSuffix              :=.elf
LibSuffix              :=.a
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
ElfInfoSwitch          :=-hlS
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
UnPreprocessorSwitch   :=-U
SourceSwitch           :=-c 
ObjdumpSwitch          :=-S
ObjcopySwitch          :=-O ihex
ObjcopyBinSwitch       :=-O binary
OutputFile             :=W806
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :=W806_SDK.txt
MakeDirCommand         :=mkdir
LinkOptions            := -mcpu=ck804ef  -mhard-float  -nostartfiles -Wl,--gc-sections -T"$(ProjectPath)/gcc_csky.ld"
LinkOtherFlagsOption   :=-Wl,-zmax-page-size=1024 -mhard-float
IncludePackagePath     :=
IncludeCPath           := $(IncludeSwitch)../../../../../../../../C-Sky/CDK/CSKY/MinGW/csky-abiv2-elf-toolchain/csky-elfabiv2/include $(IncludeSwitch). $(IncludeSwitch)../../../../../../PH7/PH7 $(IncludeSwitch)../Include $(IncludeSwitch)../Include/arch/xt804 $(IncludeSwitch)../Include/arch/xt804/csi_core $(IncludeSwitch)../Include/arch/xt804/csi_dsp $(IncludeSwitch)../Include/driver $(IncludeSwitch)../Libraries/component/FreeRTOS/include $(IncludeSwitch)../Libraries/component/FreeRTOS/portable/xt804 $(IncludeSwitch)inc $(IncludeSwitch)bsp/inc $(IncludeSwitch)pikascript/pikascript-api $(IncludeSwitch)pikascript/pikascript-core $(IncludeSwitch)pikascript/pikascript-lib/W801Device  
IncludeAPath           := $(IncludeSwitch)../../../../../../../../C-Sky/CDK/CSKY/csi/csi_core/csi_cdk/ $(IncludeSwitch)../../../../../../../../C-Sky/CDK/CSKY/csi/csi_core/include/ $(IncludeSwitch)../../../../../../../../C-Sky/CDK/CSKY/csi/csi_driver/include/ $(IncludeSwitch). $(IncludeSwitch)../../../../../../PH7/PH7 $(IncludeSwitch)../Include $(IncludeSwitch)../Include/arch/xt804 $(IncludeSwitch)../Include/arch/xt804/csi_core $(IncludeSwitch)../Include/arch/xt804/csi_dsp $(IncludeSwitch)../Libraries/component/FreeRTOS/include $(IncludeSwitch)bsp/inc $(IncludeSwitch)pikascript/pikascript-api $(IncludeSwitch)pikascript/pikascript-core $(IncludeSwitch)pikascript/pikascript-lib/W801Device  
Libs                   := -Wl,--start-group  -Wl,--end-group $(LibrarySwitch)dsp $(LibrarySwitch)m  
ArLibs                 := "dsp m" 
PackagesLibPath        :=
LibPath                :=$(LibraryPathSwitch).  $(PackagesLibPath) 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       :=csky-elfabiv2-ar rcu
CXX      :=csky-elfabiv2-g++
CC       :=csky-elfabiv2-gcc
AS       :=csky-elfabiv2-gcc
OBJDUMP  :=csky-elfabiv2-objdump
OBJCOPY  :=csky-elfabiv2-objcopy
CXXFLAGS := -mcpu=ck804ef  -mhard-float   $(PreprocessorSwitch)GCC_COMPILE=1  $(PreprocessorSwitch)TLS_CONFIG_CPU_XT804=1   -O1  -g3  -Wall  -ffunction-sections -fdata-sections -c 
CFLAGS   := -mcpu=ck804ef  -mhard-float   $(PreprocessorSwitch)GCC_COMPILE=1  $(PreprocessorSwitch)TLS_CONFIG_CPU_XT804=1   -O1  -g3  -Wall  -ffunction-sections -fdata-sections -c 
ASFLAGS  := -mcpu=ck804ef  -mhard-float    -Wa,--gdwarf2    


Objects0=$(IntermediateDirectory)/src_main$(ObjectSuffix) $(IntermediateDirectory)/src_wm_hal_msp$(ObjectSuffix) $(IntermediateDirectory)/src_wm_it$(ObjectSuffix) $(IntermediateDirectory)/src_mytask$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_adc$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_cpu$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_gpio$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_hal$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_i2c$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_internal_flash$(ObjectSuffix) \
	$(IntermediateDirectory)/drivers_wm_pmu$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_pwm$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_spi$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_spi_flash$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_tim$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_touch$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_uart$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_wdg$(ObjectSuffix) $(IntermediateDirectory)/drivers_wm_lcd$(ObjectSuffix) $(IntermediateDirectory)/bsp_board_init$(ObjectSuffix) \
	$(IntermediateDirectory)/bsp_startup$(ObjectSuffix) $(IntermediateDirectory)/bsp_system$(ObjectSuffix) $(IntermediateDirectory)/bsp_trap_c$(ObjectSuffix) $(IntermediateDirectory)/bsp_vectors$(ObjectSuffix) $(IntermediateDirectory)/libc_libc_port$(ObjectSuffix) $(IntermediateDirectory)/FreeRTOS_croutine$(ObjectSuffix) $(IntermediateDirectory)/FreeRTOS_event_groups$(ObjectSuffix) $(IntermediateDirectory)/FreeRTOS_list$(ObjectSuffix) $(IntermediateDirectory)/FreeRTOS_queue$(ObjectSuffix) $(IntermediateDirectory)/FreeRTOS_stream_buffer$(ObjectSuffix) \
	$(IntermediateDirectory)/FreeRTOS_tasks$(ObjectSuffix) $(IntermediateDirectory)/FreeRTOS_timers$(ObjectSuffix) $(IntermediateDirectory)/xt804_cpu_task_sw$(ObjectSuffix) $(IntermediateDirectory)/xt804_port$(ObjectSuffix) $(IntermediateDirectory)/MemMang_heap_5$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_PikaMain-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_pikaScript$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_PikaStdDevice_ADC-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_PikaStdDevice_GPIO-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_PikaStdDevice_IIC-api$(ObjectSuffix) \
	$(IntermediateDirectory)/pikascript-api_PikaStdDevice_PWM-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_PikaStdDevice_Time-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_PikaStdDevice_UART-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_PikaStdLib-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_PikaStdLib_MemChecker-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_PikaStdLib_RangeObj-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_PikaStdLib_StringObj-api$(ObjectSuffix) 

Objects1=$(IntermediateDirectory)/pikascript-api_PikaStdLib_SysObj-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_W801Device-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_W801Device_GPIO-api$(ObjectSuffix) \
	$(IntermediateDirectory)/pikascript-api_W801Device_PWM-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_W801Device_Time-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_W801Device_UART-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_W801Device_IIC-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-api_W801Device_ADC-api$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_BaseObj$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_dataArg$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_dataArgs$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_dataLink$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_dataLinkNode$(ObjectSuffix) \
	$(IntermediateDirectory)/pikascript-core_dataMemory$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_dataQueue$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_dataQueueObj$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_dataStack$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_dataString$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_dataStrs$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_PikaObj$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_PikaParser$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_PikaPlatform$(ObjectSuffix) $(IntermediateDirectory)/pikascript-core_PikaVM$(ObjectSuffix) \
	$(IntermediateDirectory)/pikascript-core_TinyObj$(ObjectSuffix) $(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_ADC$(ObjectSuffix) $(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_GPIO$(ObjectSuffix) $(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_IIC$(ObjectSuffix) $(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_PWM$(ObjectSuffix) $(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_Time$(ObjectSuffix) $(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_UART$(ObjectSuffix) $(IntermediateDirectory)/PikaStdLib_PikaStdLib_MemChecker$(ObjectSuffix) $(IntermediateDirectory)/PikaStdLib_PikaStdLib_RangeObj$(ObjectSuffix) $(IntermediateDirectory)/PikaStdLib_PikaStdLib_StringObj$(ObjectSuffix) \
	$(IntermediateDirectory)/PikaStdLib_PikaStdLib_SysObj$(ObjectSuffix) $(IntermediateDirectory)/W801Device_W801_common$(ObjectSuffix) $(IntermediateDirectory)/W801Device_W801_GPIO$(ObjectSuffix) $(IntermediateDirectory)/W801Device_W801_PWM$(ObjectSuffix) $(IntermediateDirectory)/W801Device_W801_Time$(ObjectSuffix) $(IntermediateDirectory)/W801Device_W801_Uart$(ObjectSuffix) $(IntermediateDirectory)/W801Device_fifo$(ObjectSuffix) $(IntermediateDirectory)/W801Device_W801_IIC$(ObjectSuffix) $(IntermediateDirectory)/W801Device_W801_ADC$(ObjectSuffix) $(IntermediateDirectory)/__rt_entry$(ObjectSuffix) 



Objects=$(Objects0) $(Objects1) 

##
## Main Build Targets 
##
.PHONY: all
all: $(IntermediateDirectory)/$(OutputFile)

$(IntermediateDirectory)/$(OutputFile):  $(Objects) Always_Link 
	$(LinkerName) $(OutputSwitch) $(IntermediateDirectory)/$(OutputFile)$(ExeSuffix) $(LinkerNameoption) $(LinkOtherFlagsOption)  -Wl,--ckmap=$(ProjectPath)/Lst/$(OutputFile).map  @$(ObjectsFileList)  $(LinkOptions) $(LibPath) $(Libs)
	@mv $(ProjectPath)/Lst/$(OutputFile).map $(ProjectPath)/Lst/$(OutputFile).temp && $(READELF) $(ElfInfoSwitch) $(ProjectPath)/Obj/$(OutputFile)$(ExeSuffix) > $(ProjectPath)/Lst/$(OutputFile).map && echo ====================================================================== >> $(ProjectPath)/Lst/$(OutputFile).map && cat $(ProjectPath)/Lst/$(OutputFile).temp >> $(ProjectPath)/Lst/$(OutputFile).map && rm -rf $(ProjectPath)/Lst/$(OutputFile).temp
	$(OBJDUMP) $(ObjdumpSwitch) $(ProjectPath)/$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix)  > $(ProjectPath)/Lst/$(OutputFile)$(DisassemSuffix) 
	@echo size of target:
	@$(SIZE) $(ProjectPath)$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix) 
	@echo -n checksum value of target:  
	@$(CHECKSUM) $(ProjectPath)/$(IntermediateDirectory)/$(OutputFile)$(ExeSuffix) 
	@$(ProjectName).modify.bat $(IntermediateDirectory) $(OutputFile)$(ExeSuffix) 

Always_Link:


##
## Objects
##
$(IntermediateDirectory)/src_main$(ObjectSuffix): src/main.c  
	$(CC) $(SourceSwitch) src/main.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/src_main$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/src_main$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/src_main$(PreprocessSuffix): src/main.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/src_main$(PreprocessSuffix) src/main.c

$(IntermediateDirectory)/src_wm_hal_msp$(ObjectSuffix): src/wm_hal_msp.c  
	$(CC) $(SourceSwitch) src/wm_hal_msp.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/src_wm_hal_msp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_wm_hal_msp$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/src_wm_hal_msp$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/src_wm_hal_msp$(PreprocessSuffix): src/wm_hal_msp.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/src_wm_hal_msp$(PreprocessSuffix) src/wm_hal_msp.c

$(IntermediateDirectory)/src_wm_it$(ObjectSuffix): src/wm_it.c  
	$(CC) $(SourceSwitch) src/wm_it.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/src_wm_it$(ObjectSuffix) -MF$(IntermediateDirectory)/src_wm_it$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/src_wm_it$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/src_wm_it$(PreprocessSuffix): src/wm_it.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/src_wm_it$(PreprocessSuffix) src/wm_it.c

$(IntermediateDirectory)/src_mytask$(ObjectSuffix): src/mytask.c  
	$(CC) $(SourceSwitch) src/mytask.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/src_mytask$(ObjectSuffix) -MF$(IntermediateDirectory)/src_mytask$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/src_mytask$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/src_mytask$(PreprocessSuffix): src/mytask.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/src_mytask$(PreprocessSuffix) src/mytask.c

$(IntermediateDirectory)/drivers_wm_adc$(ObjectSuffix): ../Libraries/drivers/wm_adc.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_adc.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_adc$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_adc$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_adc$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_adc$(PreprocessSuffix): ../Libraries/drivers/wm_adc.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_adc$(PreprocessSuffix) ../Libraries/drivers/wm_adc.c

$(IntermediateDirectory)/drivers_wm_cpu$(ObjectSuffix): ../Libraries/drivers/wm_cpu.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_cpu.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_cpu$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_cpu$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_cpu$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_cpu$(PreprocessSuffix): ../Libraries/drivers/wm_cpu.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_cpu$(PreprocessSuffix) ../Libraries/drivers/wm_cpu.c

$(IntermediateDirectory)/drivers_wm_gpio$(ObjectSuffix): ../Libraries/drivers/wm_gpio.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_gpio.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_gpio$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_gpio$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_gpio$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_gpio$(PreprocessSuffix): ../Libraries/drivers/wm_gpio.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_gpio$(PreprocessSuffix) ../Libraries/drivers/wm_gpio.c

$(IntermediateDirectory)/drivers_wm_hal$(ObjectSuffix): ../Libraries/drivers/wm_hal.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_hal.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_hal$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_hal$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_hal$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_hal$(PreprocessSuffix): ../Libraries/drivers/wm_hal.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_hal$(PreprocessSuffix) ../Libraries/drivers/wm_hal.c

$(IntermediateDirectory)/drivers_wm_i2c$(ObjectSuffix): ../Libraries/drivers/wm_i2c.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_i2c.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_i2c$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_i2c$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_i2c$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_i2c$(PreprocessSuffix): ../Libraries/drivers/wm_i2c.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_i2c$(PreprocessSuffix) ../Libraries/drivers/wm_i2c.c

$(IntermediateDirectory)/drivers_wm_internal_flash$(ObjectSuffix): ../Libraries/drivers/wm_internal_flash.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_internal_flash.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_internal_flash$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_internal_flash$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_internal_flash$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_internal_flash$(PreprocessSuffix): ../Libraries/drivers/wm_internal_flash.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_internal_flash$(PreprocessSuffix) ../Libraries/drivers/wm_internal_flash.c

$(IntermediateDirectory)/drivers_wm_pmu$(ObjectSuffix): ../Libraries/drivers/wm_pmu.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_pmu.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_pmu$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_pmu$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_pmu$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_pmu$(PreprocessSuffix): ../Libraries/drivers/wm_pmu.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_pmu$(PreprocessSuffix) ../Libraries/drivers/wm_pmu.c

$(IntermediateDirectory)/drivers_wm_pwm$(ObjectSuffix): ../Libraries/drivers/wm_pwm.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_pwm.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_pwm$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_pwm$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_pwm$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_pwm$(PreprocessSuffix): ../Libraries/drivers/wm_pwm.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_pwm$(PreprocessSuffix) ../Libraries/drivers/wm_pwm.c

$(IntermediateDirectory)/drivers_wm_spi$(ObjectSuffix): ../Libraries/drivers/wm_spi.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_spi.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_spi$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_spi$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_spi$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_spi$(PreprocessSuffix): ../Libraries/drivers/wm_spi.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_spi$(PreprocessSuffix) ../Libraries/drivers/wm_spi.c

$(IntermediateDirectory)/drivers_wm_spi_flash$(ObjectSuffix): ../Libraries/drivers/wm_spi_flash.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_spi_flash.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_spi_flash$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_spi_flash$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_spi_flash$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_spi_flash$(PreprocessSuffix): ../Libraries/drivers/wm_spi_flash.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_spi_flash$(PreprocessSuffix) ../Libraries/drivers/wm_spi_flash.c

$(IntermediateDirectory)/drivers_wm_tim$(ObjectSuffix): ../Libraries/drivers/wm_tim.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_tim.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_tim$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_tim$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_tim$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_tim$(PreprocessSuffix): ../Libraries/drivers/wm_tim.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_tim$(PreprocessSuffix) ../Libraries/drivers/wm_tim.c

$(IntermediateDirectory)/drivers_wm_touch$(ObjectSuffix): ../Libraries/drivers/wm_touch.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_touch.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_touch$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_touch$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_touch$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_touch$(PreprocessSuffix): ../Libraries/drivers/wm_touch.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_touch$(PreprocessSuffix) ../Libraries/drivers/wm_touch.c

$(IntermediateDirectory)/drivers_wm_uart$(ObjectSuffix): ../Libraries/drivers/wm_uart.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_uart.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_uart$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_uart$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_uart$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_uart$(PreprocessSuffix): ../Libraries/drivers/wm_uart.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_uart$(PreprocessSuffix) ../Libraries/drivers/wm_uart.c

$(IntermediateDirectory)/drivers_wm_wdg$(ObjectSuffix): ../Libraries/drivers/wm_wdg.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_wdg.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_wdg$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_wdg$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_wdg$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_wdg$(PreprocessSuffix): ../Libraries/drivers/wm_wdg.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_wdg$(PreprocessSuffix) ../Libraries/drivers/wm_wdg.c

$(IntermediateDirectory)/drivers_wm_lcd$(ObjectSuffix): ../Libraries/drivers/wm_lcd.c  
	$(CC) $(SourceSwitch) ../Libraries/drivers/wm_lcd.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/drivers_wm_lcd$(ObjectSuffix) -MF$(IntermediateDirectory)/drivers_wm_lcd$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/drivers_wm_lcd$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/drivers_wm_lcd$(PreprocessSuffix): ../Libraries/drivers/wm_lcd.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/drivers_wm_lcd$(PreprocessSuffix) ../Libraries/drivers/wm_lcd.c

$(IntermediateDirectory)/bsp_board_init$(ObjectSuffix): ../Libraries/arch/xt804/bsp/board_init.c  
	$(CC) $(SourceSwitch) ../Libraries/arch/xt804/bsp/board_init.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/bsp_board_init$(ObjectSuffix) -MF$(IntermediateDirectory)/bsp_board_init$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/bsp_board_init$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/bsp_board_init$(PreprocessSuffix): ../Libraries/arch/xt804/bsp/board_init.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/bsp_board_init$(PreprocessSuffix) ../Libraries/arch/xt804/bsp/board_init.c

$(IntermediateDirectory)/bsp_startup$(ObjectSuffix): ../Libraries/arch/xt804/bsp/startup.S  
	$(AS) $(SourceSwitch) ../Libraries/arch/xt804/bsp/startup.S $(ASFLAGS) -MMD -MP -MT$(IntermediateDirectory)/bsp_startup$(ObjectSuffix) -MF$(IntermediateDirectory)/bsp_startup$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/bsp_startup$(ObjectSuffix) $(IncludeAPath) $(IncludePackagePath)
Lst/bsp_startup$(PreprocessSuffix): ../Libraries/arch/xt804/bsp/startup.S
	$(CC) $(CFLAGS)$(IncludeAPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/bsp_startup$(PreprocessSuffix) ../Libraries/arch/xt804/bsp/startup.S

$(IntermediateDirectory)/bsp_system$(ObjectSuffix): ../Libraries/arch/xt804/bsp/system.c  
	$(CC) $(SourceSwitch) ../Libraries/arch/xt804/bsp/system.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/bsp_system$(ObjectSuffix) -MF$(IntermediateDirectory)/bsp_system$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/bsp_system$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/bsp_system$(PreprocessSuffix): ../Libraries/arch/xt804/bsp/system.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/bsp_system$(PreprocessSuffix) ../Libraries/arch/xt804/bsp/system.c

$(IntermediateDirectory)/bsp_trap_c$(ObjectSuffix): ../Libraries/arch/xt804/bsp/trap_c.c  
	$(CC) $(SourceSwitch) ../Libraries/arch/xt804/bsp/trap_c.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/bsp_trap_c$(ObjectSuffix) -MF$(IntermediateDirectory)/bsp_trap_c$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/bsp_trap_c$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/bsp_trap_c$(PreprocessSuffix): ../Libraries/arch/xt804/bsp/trap_c.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/bsp_trap_c$(PreprocessSuffix) ../Libraries/arch/xt804/bsp/trap_c.c

$(IntermediateDirectory)/bsp_vectors$(ObjectSuffix): ../Libraries/arch/xt804/bsp/vectors.S  
	$(AS) $(SourceSwitch) ../Libraries/arch/xt804/bsp/vectors.S $(ASFLAGS) -MMD -MP -MT$(IntermediateDirectory)/bsp_vectors$(ObjectSuffix) -MF$(IntermediateDirectory)/bsp_vectors$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/bsp_vectors$(ObjectSuffix) $(IncludeAPath) $(IncludePackagePath)
Lst/bsp_vectors$(PreprocessSuffix): ../Libraries/arch/xt804/bsp/vectors.S
	$(CC) $(CFLAGS)$(IncludeAPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/bsp_vectors$(PreprocessSuffix) ../Libraries/arch/xt804/bsp/vectors.S

$(IntermediateDirectory)/libc_libc_port$(ObjectSuffix): ../Libraries/arch/xt804/libc/libc_port.c  
	$(CC) $(SourceSwitch) ../Libraries/arch/xt804/libc/libc_port.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/libc_libc_port$(ObjectSuffix) -MF$(IntermediateDirectory)/libc_libc_port$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/libc_libc_port$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/libc_libc_port$(PreprocessSuffix): ../Libraries/arch/xt804/libc/libc_port.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/libc_libc_port$(PreprocessSuffix) ../Libraries/arch/xt804/libc/libc_port.c

$(IntermediateDirectory)/FreeRTOS_croutine$(ObjectSuffix): ../Libraries/component/FreeRTOS/croutine.c  
	$(CC) $(SourceSwitch) ../Libraries/component/FreeRTOS/croutine.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/FreeRTOS_croutine$(ObjectSuffix) -MF$(IntermediateDirectory)/FreeRTOS_croutine$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/FreeRTOS_croutine$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/FreeRTOS_croutine$(PreprocessSuffix): ../Libraries/component/FreeRTOS/croutine.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FreeRTOS_croutine$(PreprocessSuffix) ../Libraries/component/FreeRTOS/croutine.c

$(IntermediateDirectory)/FreeRTOS_event_groups$(ObjectSuffix): ../Libraries/component/FreeRTOS/event_groups.c  
	$(CC) $(SourceSwitch) ../Libraries/component/FreeRTOS/event_groups.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/FreeRTOS_event_groups$(ObjectSuffix) -MF$(IntermediateDirectory)/FreeRTOS_event_groups$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/FreeRTOS_event_groups$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/FreeRTOS_event_groups$(PreprocessSuffix): ../Libraries/component/FreeRTOS/event_groups.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FreeRTOS_event_groups$(PreprocessSuffix) ../Libraries/component/FreeRTOS/event_groups.c

$(IntermediateDirectory)/FreeRTOS_list$(ObjectSuffix): ../Libraries/component/FreeRTOS/list.c  
	$(CC) $(SourceSwitch) ../Libraries/component/FreeRTOS/list.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/FreeRTOS_list$(ObjectSuffix) -MF$(IntermediateDirectory)/FreeRTOS_list$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/FreeRTOS_list$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/FreeRTOS_list$(PreprocessSuffix): ../Libraries/component/FreeRTOS/list.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FreeRTOS_list$(PreprocessSuffix) ../Libraries/component/FreeRTOS/list.c

$(IntermediateDirectory)/FreeRTOS_queue$(ObjectSuffix): ../Libraries/component/FreeRTOS/queue.c  
	$(CC) $(SourceSwitch) ../Libraries/component/FreeRTOS/queue.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/FreeRTOS_queue$(ObjectSuffix) -MF$(IntermediateDirectory)/FreeRTOS_queue$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/FreeRTOS_queue$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/FreeRTOS_queue$(PreprocessSuffix): ../Libraries/component/FreeRTOS/queue.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FreeRTOS_queue$(PreprocessSuffix) ../Libraries/component/FreeRTOS/queue.c

$(IntermediateDirectory)/FreeRTOS_stream_buffer$(ObjectSuffix): ../Libraries/component/FreeRTOS/stream_buffer.c  
	$(CC) $(SourceSwitch) ../Libraries/component/FreeRTOS/stream_buffer.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/FreeRTOS_stream_buffer$(ObjectSuffix) -MF$(IntermediateDirectory)/FreeRTOS_stream_buffer$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/FreeRTOS_stream_buffer$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/FreeRTOS_stream_buffer$(PreprocessSuffix): ../Libraries/component/FreeRTOS/stream_buffer.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FreeRTOS_stream_buffer$(PreprocessSuffix) ../Libraries/component/FreeRTOS/stream_buffer.c

$(IntermediateDirectory)/FreeRTOS_tasks$(ObjectSuffix): ../Libraries/component/FreeRTOS/tasks.c  
	$(CC) $(SourceSwitch) ../Libraries/component/FreeRTOS/tasks.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/FreeRTOS_tasks$(ObjectSuffix) -MF$(IntermediateDirectory)/FreeRTOS_tasks$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/FreeRTOS_tasks$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/FreeRTOS_tasks$(PreprocessSuffix): ../Libraries/component/FreeRTOS/tasks.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FreeRTOS_tasks$(PreprocessSuffix) ../Libraries/component/FreeRTOS/tasks.c

$(IntermediateDirectory)/FreeRTOS_timers$(ObjectSuffix): ../Libraries/component/FreeRTOS/timers.c  
	$(CC) $(SourceSwitch) ../Libraries/component/FreeRTOS/timers.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/FreeRTOS_timers$(ObjectSuffix) -MF$(IntermediateDirectory)/FreeRTOS_timers$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/FreeRTOS_timers$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/FreeRTOS_timers$(PreprocessSuffix): ../Libraries/component/FreeRTOS/timers.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/FreeRTOS_timers$(PreprocessSuffix) ../Libraries/component/FreeRTOS/timers.c

$(IntermediateDirectory)/xt804_cpu_task_sw$(ObjectSuffix): ../Libraries/component/FreeRTOS/portable/xt804/cpu_task_sw.S  
	$(AS) $(SourceSwitch) ../Libraries/component/FreeRTOS/portable/xt804/cpu_task_sw.S $(ASFLAGS) -MMD -MP -MT$(IntermediateDirectory)/xt804_cpu_task_sw$(ObjectSuffix) -MF$(IntermediateDirectory)/xt804_cpu_task_sw$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/xt804_cpu_task_sw$(ObjectSuffix) $(IncludeAPath) $(IncludePackagePath)
Lst/xt804_cpu_task_sw$(PreprocessSuffix): ../Libraries/component/FreeRTOS/portable/xt804/cpu_task_sw.S
	$(CC) $(CFLAGS)$(IncludeAPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/xt804_cpu_task_sw$(PreprocessSuffix) ../Libraries/component/FreeRTOS/portable/xt804/cpu_task_sw.S

$(IntermediateDirectory)/xt804_port$(ObjectSuffix): ../Libraries/component/FreeRTOS/portable/xt804/port.c  
	$(CC) $(SourceSwitch) ../Libraries/component/FreeRTOS/portable/xt804/port.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/xt804_port$(ObjectSuffix) -MF$(IntermediateDirectory)/xt804_port$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/xt804_port$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/xt804_port$(PreprocessSuffix): ../Libraries/component/FreeRTOS/portable/xt804/port.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/xt804_port$(PreprocessSuffix) ../Libraries/component/FreeRTOS/portable/xt804/port.c

$(IntermediateDirectory)/MemMang_heap_5$(ObjectSuffix): ../Libraries/component/FreeRTOS/portable/MemMang/heap_5.c  
	$(CC) $(SourceSwitch) ../Libraries/component/FreeRTOS/portable/MemMang/heap_5.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/MemMang_heap_5$(ObjectSuffix) -MF$(IntermediateDirectory)/MemMang_heap_5$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/MemMang_heap_5$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/MemMang_heap_5$(PreprocessSuffix): ../Libraries/component/FreeRTOS/portable/MemMang/heap_5.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/MemMang_heap_5$(PreprocessSuffix) ../Libraries/component/FreeRTOS/portable/MemMang/heap_5.c

$(IntermediateDirectory)/pikascript-api_PikaMain-api$(ObjectSuffix): pikascript/pikascript-api/PikaMain-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaMain-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaMain-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaMain-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaMain-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaMain-api$(PreprocessSuffix): pikascript/pikascript-api/PikaMain-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaMain-api$(PreprocessSuffix) pikascript/pikascript-api/PikaMain-api.c

$(IntermediateDirectory)/pikascript-api_pikaScript$(ObjectSuffix): pikascript/pikascript-api/pikaScript.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/pikaScript.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_pikaScript$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_pikaScript$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_pikaScript$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_pikaScript$(PreprocessSuffix): pikascript/pikascript-api/pikaScript.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_pikaScript$(PreprocessSuffix) pikascript/pikascript-api/pikaScript.c

$(IntermediateDirectory)/pikascript-api_PikaStdDevice_ADC-api$(ObjectSuffix): pikascript/pikascript-api/PikaStdDevice_ADC-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaStdDevice_ADC-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaStdDevice_ADC-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaStdDevice_ADC-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaStdDevice_ADC-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaStdDevice_ADC-api$(PreprocessSuffix): pikascript/pikascript-api/PikaStdDevice_ADC-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaStdDevice_ADC-api$(PreprocessSuffix) pikascript/pikascript-api/PikaStdDevice_ADC-api.c

$(IntermediateDirectory)/pikascript-api_PikaStdDevice_GPIO-api$(ObjectSuffix): pikascript/pikascript-api/PikaStdDevice_GPIO-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaStdDevice_GPIO-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaStdDevice_GPIO-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaStdDevice_GPIO-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaStdDevice_GPIO-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaStdDevice_GPIO-api$(PreprocessSuffix): pikascript/pikascript-api/PikaStdDevice_GPIO-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaStdDevice_GPIO-api$(PreprocessSuffix) pikascript/pikascript-api/PikaStdDevice_GPIO-api.c

$(IntermediateDirectory)/pikascript-api_PikaStdDevice_IIC-api$(ObjectSuffix): pikascript/pikascript-api/PikaStdDevice_IIC-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaStdDevice_IIC-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaStdDevice_IIC-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaStdDevice_IIC-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaStdDevice_IIC-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaStdDevice_IIC-api$(PreprocessSuffix): pikascript/pikascript-api/PikaStdDevice_IIC-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaStdDevice_IIC-api$(PreprocessSuffix) pikascript/pikascript-api/PikaStdDevice_IIC-api.c

$(IntermediateDirectory)/pikascript-api_PikaStdDevice_PWM-api$(ObjectSuffix): pikascript/pikascript-api/PikaStdDevice_PWM-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaStdDevice_PWM-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaStdDevice_PWM-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaStdDevice_PWM-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaStdDevice_PWM-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaStdDevice_PWM-api$(PreprocessSuffix): pikascript/pikascript-api/PikaStdDevice_PWM-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaStdDevice_PWM-api$(PreprocessSuffix) pikascript/pikascript-api/PikaStdDevice_PWM-api.c

$(IntermediateDirectory)/pikascript-api_PikaStdDevice_Time-api$(ObjectSuffix): pikascript/pikascript-api/PikaStdDevice_Time-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaStdDevice_Time-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaStdDevice_Time-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaStdDevice_Time-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaStdDevice_Time-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaStdDevice_Time-api$(PreprocessSuffix): pikascript/pikascript-api/PikaStdDevice_Time-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaStdDevice_Time-api$(PreprocessSuffix) pikascript/pikascript-api/PikaStdDevice_Time-api.c

$(IntermediateDirectory)/pikascript-api_PikaStdDevice_UART-api$(ObjectSuffix): pikascript/pikascript-api/PikaStdDevice_UART-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaStdDevice_UART-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaStdDevice_UART-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaStdDevice_UART-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaStdDevice_UART-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaStdDevice_UART-api$(PreprocessSuffix): pikascript/pikascript-api/PikaStdDevice_UART-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaStdDevice_UART-api$(PreprocessSuffix) pikascript/pikascript-api/PikaStdDevice_UART-api.c

$(IntermediateDirectory)/pikascript-api_PikaStdLib-api$(ObjectSuffix): pikascript/pikascript-api/PikaStdLib-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaStdLib-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaStdLib-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaStdLib-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaStdLib-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaStdLib-api$(PreprocessSuffix): pikascript/pikascript-api/PikaStdLib-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaStdLib-api$(PreprocessSuffix) pikascript/pikascript-api/PikaStdLib-api.c

$(IntermediateDirectory)/pikascript-api_PikaStdLib_MemChecker-api$(ObjectSuffix): pikascript/pikascript-api/PikaStdLib_MemChecker-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaStdLib_MemChecker-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaStdLib_MemChecker-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaStdLib_MemChecker-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaStdLib_MemChecker-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaStdLib_MemChecker-api$(PreprocessSuffix): pikascript/pikascript-api/PikaStdLib_MemChecker-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaStdLib_MemChecker-api$(PreprocessSuffix) pikascript/pikascript-api/PikaStdLib_MemChecker-api.c

$(IntermediateDirectory)/pikascript-api_PikaStdLib_RangeObj-api$(ObjectSuffix): pikascript/pikascript-api/PikaStdLib_RangeObj-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaStdLib_RangeObj-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaStdLib_RangeObj-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaStdLib_RangeObj-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaStdLib_RangeObj-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaStdLib_RangeObj-api$(PreprocessSuffix): pikascript/pikascript-api/PikaStdLib_RangeObj-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaStdLib_RangeObj-api$(PreprocessSuffix) pikascript/pikascript-api/PikaStdLib_RangeObj-api.c

$(IntermediateDirectory)/pikascript-api_PikaStdLib_StringObj-api$(ObjectSuffix): pikascript/pikascript-api/PikaStdLib_StringObj-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaStdLib_StringObj-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaStdLib_StringObj-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaStdLib_StringObj-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaStdLib_StringObj-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaStdLib_StringObj-api$(PreprocessSuffix): pikascript/pikascript-api/PikaStdLib_StringObj-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaStdLib_StringObj-api$(PreprocessSuffix) pikascript/pikascript-api/PikaStdLib_StringObj-api.c

$(IntermediateDirectory)/pikascript-api_PikaStdLib_SysObj-api$(ObjectSuffix): pikascript/pikascript-api/PikaStdLib_SysObj-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/PikaStdLib_SysObj-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_PikaStdLib_SysObj-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_PikaStdLib_SysObj-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_PikaStdLib_SysObj-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_PikaStdLib_SysObj-api$(PreprocessSuffix): pikascript/pikascript-api/PikaStdLib_SysObj-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_PikaStdLib_SysObj-api$(PreprocessSuffix) pikascript/pikascript-api/PikaStdLib_SysObj-api.c

$(IntermediateDirectory)/pikascript-api_W801Device-api$(ObjectSuffix): pikascript/pikascript-api/W801Device-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/W801Device-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_W801Device-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_W801Device-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_W801Device-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_W801Device-api$(PreprocessSuffix): pikascript/pikascript-api/W801Device-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_W801Device-api$(PreprocessSuffix) pikascript/pikascript-api/W801Device-api.c

$(IntermediateDirectory)/pikascript-api_W801Device_GPIO-api$(ObjectSuffix): pikascript/pikascript-api/W801Device_GPIO-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/W801Device_GPIO-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_W801Device_GPIO-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_W801Device_GPIO-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_W801Device_GPIO-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_W801Device_GPIO-api$(PreprocessSuffix): pikascript/pikascript-api/W801Device_GPIO-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_W801Device_GPIO-api$(PreprocessSuffix) pikascript/pikascript-api/W801Device_GPIO-api.c

$(IntermediateDirectory)/pikascript-api_W801Device_PWM-api$(ObjectSuffix): pikascript/pikascript-api/W801Device_PWM-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/W801Device_PWM-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_W801Device_PWM-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_W801Device_PWM-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_W801Device_PWM-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_W801Device_PWM-api$(PreprocessSuffix): pikascript/pikascript-api/W801Device_PWM-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_W801Device_PWM-api$(PreprocessSuffix) pikascript/pikascript-api/W801Device_PWM-api.c

$(IntermediateDirectory)/pikascript-api_W801Device_Time-api$(ObjectSuffix): pikascript/pikascript-api/W801Device_Time-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/W801Device_Time-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_W801Device_Time-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_W801Device_Time-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_W801Device_Time-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_W801Device_Time-api$(PreprocessSuffix): pikascript/pikascript-api/W801Device_Time-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_W801Device_Time-api$(PreprocessSuffix) pikascript/pikascript-api/W801Device_Time-api.c

$(IntermediateDirectory)/pikascript-api_W801Device_UART-api$(ObjectSuffix): pikascript/pikascript-api/W801Device_UART-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/W801Device_UART-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_W801Device_UART-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_W801Device_UART-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_W801Device_UART-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_W801Device_UART-api$(PreprocessSuffix): pikascript/pikascript-api/W801Device_UART-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_W801Device_UART-api$(PreprocessSuffix) pikascript/pikascript-api/W801Device_UART-api.c

$(IntermediateDirectory)/pikascript-api_W801Device_IIC-api$(ObjectSuffix): pikascript/pikascript-api/W801Device_IIC-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/W801Device_IIC-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_W801Device_IIC-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_W801Device_IIC-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_W801Device_IIC-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_W801Device_IIC-api$(PreprocessSuffix): pikascript/pikascript-api/W801Device_IIC-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_W801Device_IIC-api$(PreprocessSuffix) pikascript/pikascript-api/W801Device_IIC-api.c

$(IntermediateDirectory)/pikascript-api_W801Device_ADC-api$(ObjectSuffix): pikascript/pikascript-api/W801Device_ADC-api.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-api/W801Device_ADC-api.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-api_W801Device_ADC-api$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-api_W801Device_ADC-api$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-api_W801Device_ADC-api$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-api_W801Device_ADC-api$(PreprocessSuffix): pikascript/pikascript-api/W801Device_ADC-api.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-api_W801Device_ADC-api$(PreprocessSuffix) pikascript/pikascript-api/W801Device_ADC-api.c

$(IntermediateDirectory)/pikascript-core_BaseObj$(ObjectSuffix): pikascript/pikascript-core/BaseObj.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/BaseObj.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_BaseObj$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_BaseObj$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_BaseObj$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_BaseObj$(PreprocessSuffix): pikascript/pikascript-core/BaseObj.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_BaseObj$(PreprocessSuffix) pikascript/pikascript-core/BaseObj.c

$(IntermediateDirectory)/pikascript-core_dataArg$(ObjectSuffix): pikascript/pikascript-core/dataArg.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/dataArg.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_dataArg$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_dataArg$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_dataArg$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_dataArg$(PreprocessSuffix): pikascript/pikascript-core/dataArg.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_dataArg$(PreprocessSuffix) pikascript/pikascript-core/dataArg.c

$(IntermediateDirectory)/pikascript-core_dataArgs$(ObjectSuffix): pikascript/pikascript-core/dataArgs.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/dataArgs.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_dataArgs$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_dataArgs$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_dataArgs$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_dataArgs$(PreprocessSuffix): pikascript/pikascript-core/dataArgs.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_dataArgs$(PreprocessSuffix) pikascript/pikascript-core/dataArgs.c

$(IntermediateDirectory)/pikascript-core_dataLink$(ObjectSuffix): pikascript/pikascript-core/dataLink.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/dataLink.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_dataLink$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_dataLink$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_dataLink$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_dataLink$(PreprocessSuffix): pikascript/pikascript-core/dataLink.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_dataLink$(PreprocessSuffix) pikascript/pikascript-core/dataLink.c

$(IntermediateDirectory)/pikascript-core_dataLinkNode$(ObjectSuffix): pikascript/pikascript-core/dataLinkNode.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/dataLinkNode.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_dataLinkNode$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_dataLinkNode$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_dataLinkNode$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_dataLinkNode$(PreprocessSuffix): pikascript/pikascript-core/dataLinkNode.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_dataLinkNode$(PreprocessSuffix) pikascript/pikascript-core/dataLinkNode.c

$(IntermediateDirectory)/pikascript-core_dataMemory$(ObjectSuffix): pikascript/pikascript-core/dataMemory.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/dataMemory.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_dataMemory$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_dataMemory$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_dataMemory$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_dataMemory$(PreprocessSuffix): pikascript/pikascript-core/dataMemory.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_dataMemory$(PreprocessSuffix) pikascript/pikascript-core/dataMemory.c

$(IntermediateDirectory)/pikascript-core_dataQueue$(ObjectSuffix): pikascript/pikascript-core/dataQueue.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/dataQueue.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_dataQueue$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_dataQueue$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_dataQueue$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_dataQueue$(PreprocessSuffix): pikascript/pikascript-core/dataQueue.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_dataQueue$(PreprocessSuffix) pikascript/pikascript-core/dataQueue.c

$(IntermediateDirectory)/pikascript-core_dataQueueObj$(ObjectSuffix): pikascript/pikascript-core/dataQueueObj.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/dataQueueObj.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_dataQueueObj$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_dataQueueObj$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_dataQueueObj$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_dataQueueObj$(PreprocessSuffix): pikascript/pikascript-core/dataQueueObj.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_dataQueueObj$(PreprocessSuffix) pikascript/pikascript-core/dataQueueObj.c

$(IntermediateDirectory)/pikascript-core_dataStack$(ObjectSuffix): pikascript/pikascript-core/dataStack.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/dataStack.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_dataStack$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_dataStack$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_dataStack$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_dataStack$(PreprocessSuffix): pikascript/pikascript-core/dataStack.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_dataStack$(PreprocessSuffix) pikascript/pikascript-core/dataStack.c

$(IntermediateDirectory)/pikascript-core_dataString$(ObjectSuffix): pikascript/pikascript-core/dataString.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/dataString.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_dataString$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_dataString$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_dataString$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_dataString$(PreprocessSuffix): pikascript/pikascript-core/dataString.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_dataString$(PreprocessSuffix) pikascript/pikascript-core/dataString.c

$(IntermediateDirectory)/pikascript-core_dataStrs$(ObjectSuffix): pikascript/pikascript-core/dataStrs.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/dataStrs.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_dataStrs$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_dataStrs$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_dataStrs$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_dataStrs$(PreprocessSuffix): pikascript/pikascript-core/dataStrs.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_dataStrs$(PreprocessSuffix) pikascript/pikascript-core/dataStrs.c

$(IntermediateDirectory)/pikascript-core_PikaObj$(ObjectSuffix): pikascript/pikascript-core/PikaObj.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/PikaObj.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_PikaObj$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_PikaObj$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_PikaObj$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_PikaObj$(PreprocessSuffix): pikascript/pikascript-core/PikaObj.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_PikaObj$(PreprocessSuffix) pikascript/pikascript-core/PikaObj.c

$(IntermediateDirectory)/pikascript-core_PikaParser$(ObjectSuffix): pikascript/pikascript-core/PikaParser.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/PikaParser.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_PikaParser$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_PikaParser$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_PikaParser$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_PikaParser$(PreprocessSuffix): pikascript/pikascript-core/PikaParser.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_PikaParser$(PreprocessSuffix) pikascript/pikascript-core/PikaParser.c

$(IntermediateDirectory)/pikascript-core_PikaPlatform$(ObjectSuffix): pikascript/pikascript-core/PikaPlatform.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/PikaPlatform.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_PikaPlatform$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_PikaPlatform$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_PikaPlatform$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_PikaPlatform$(PreprocessSuffix): pikascript/pikascript-core/PikaPlatform.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_PikaPlatform$(PreprocessSuffix) pikascript/pikascript-core/PikaPlatform.c

$(IntermediateDirectory)/pikascript-core_PikaVM$(ObjectSuffix): pikascript/pikascript-core/PikaVM.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/PikaVM.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_PikaVM$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_PikaVM$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_PikaVM$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_PikaVM$(PreprocessSuffix): pikascript/pikascript-core/PikaVM.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_PikaVM$(PreprocessSuffix) pikascript/pikascript-core/PikaVM.c

$(IntermediateDirectory)/pikascript-core_TinyObj$(ObjectSuffix): pikascript/pikascript-core/TinyObj.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-core/TinyObj.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/pikascript-core_TinyObj$(ObjectSuffix) -MF$(IntermediateDirectory)/pikascript-core_TinyObj$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/pikascript-core_TinyObj$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/pikascript-core_TinyObj$(PreprocessSuffix): pikascript/pikascript-core/TinyObj.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/pikascript-core_TinyObj$(PreprocessSuffix) pikascript/pikascript-core/TinyObj.c

$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_ADC$(ObjectSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_ADC.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_ADC.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_ADC$(ObjectSuffix) -MF$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_ADC$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_ADC$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/PikaStdDevice_PikaStdDevice_ADC$(PreprocessSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_ADC.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/PikaStdDevice_PikaStdDevice_ADC$(PreprocessSuffix) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_ADC.c

$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_GPIO$(ObjectSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_GPIO.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_GPIO.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_GPIO$(ObjectSuffix) -MF$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_GPIO$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_GPIO$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/PikaStdDevice_PikaStdDevice_GPIO$(PreprocessSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_GPIO.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/PikaStdDevice_PikaStdDevice_GPIO$(PreprocessSuffix) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_GPIO.c

$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_IIC$(ObjectSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_IIC.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_IIC.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_IIC$(ObjectSuffix) -MF$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_IIC$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_IIC$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/PikaStdDevice_PikaStdDevice_IIC$(PreprocessSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_IIC.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/PikaStdDevice_PikaStdDevice_IIC$(PreprocessSuffix) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_IIC.c

$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_PWM$(ObjectSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_PWM.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_PWM.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_PWM$(ObjectSuffix) -MF$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_PWM$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_PWM$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/PikaStdDevice_PikaStdDevice_PWM$(PreprocessSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_PWM.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/PikaStdDevice_PikaStdDevice_PWM$(PreprocessSuffix) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_PWM.c

$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_Time$(ObjectSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_Time.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_Time.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_Time$(ObjectSuffix) -MF$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_Time$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_Time$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/PikaStdDevice_PikaStdDevice_Time$(PreprocessSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_Time.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/PikaStdDevice_PikaStdDevice_Time$(PreprocessSuffix) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_Time.c

$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_UART$(ObjectSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_UART.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_UART.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_UART$(ObjectSuffix) -MF$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_UART$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/PikaStdDevice_PikaStdDevice_UART$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/PikaStdDevice_PikaStdDevice_UART$(PreprocessSuffix): pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_UART.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/PikaStdDevice_PikaStdDevice_UART$(PreprocessSuffix) pikascript/pikascript-lib/PikaStdDevice/PikaStdDevice_UART.c

$(IntermediateDirectory)/PikaStdLib_PikaStdLib_MemChecker$(ObjectSuffix): pikascript/pikascript-lib/PikaStdLib/PikaStdLib_MemChecker.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/PikaStdLib/PikaStdLib_MemChecker.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/PikaStdLib_PikaStdLib_MemChecker$(ObjectSuffix) -MF$(IntermediateDirectory)/PikaStdLib_PikaStdLib_MemChecker$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/PikaStdLib_PikaStdLib_MemChecker$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/PikaStdLib_PikaStdLib_MemChecker$(PreprocessSuffix): pikascript/pikascript-lib/PikaStdLib/PikaStdLib_MemChecker.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/PikaStdLib_PikaStdLib_MemChecker$(PreprocessSuffix) pikascript/pikascript-lib/PikaStdLib/PikaStdLib_MemChecker.c

$(IntermediateDirectory)/PikaStdLib_PikaStdLib_RangeObj$(ObjectSuffix): pikascript/pikascript-lib/PikaStdLib/PikaStdLib_RangeObj.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/PikaStdLib/PikaStdLib_RangeObj.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/PikaStdLib_PikaStdLib_RangeObj$(ObjectSuffix) -MF$(IntermediateDirectory)/PikaStdLib_PikaStdLib_RangeObj$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/PikaStdLib_PikaStdLib_RangeObj$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/PikaStdLib_PikaStdLib_RangeObj$(PreprocessSuffix): pikascript/pikascript-lib/PikaStdLib/PikaStdLib_RangeObj.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/PikaStdLib_PikaStdLib_RangeObj$(PreprocessSuffix) pikascript/pikascript-lib/PikaStdLib/PikaStdLib_RangeObj.c

$(IntermediateDirectory)/PikaStdLib_PikaStdLib_StringObj$(ObjectSuffix): pikascript/pikascript-lib/PikaStdLib/PikaStdLib_StringObj.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/PikaStdLib/PikaStdLib_StringObj.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/PikaStdLib_PikaStdLib_StringObj$(ObjectSuffix) -MF$(IntermediateDirectory)/PikaStdLib_PikaStdLib_StringObj$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/PikaStdLib_PikaStdLib_StringObj$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/PikaStdLib_PikaStdLib_StringObj$(PreprocessSuffix): pikascript/pikascript-lib/PikaStdLib/PikaStdLib_StringObj.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/PikaStdLib_PikaStdLib_StringObj$(PreprocessSuffix) pikascript/pikascript-lib/PikaStdLib/PikaStdLib_StringObj.c

$(IntermediateDirectory)/PikaStdLib_PikaStdLib_SysObj$(ObjectSuffix): pikascript/pikascript-lib/PikaStdLib/PikaStdLib_SysObj.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/PikaStdLib/PikaStdLib_SysObj.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/PikaStdLib_PikaStdLib_SysObj$(ObjectSuffix) -MF$(IntermediateDirectory)/PikaStdLib_PikaStdLib_SysObj$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/PikaStdLib_PikaStdLib_SysObj$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/PikaStdLib_PikaStdLib_SysObj$(PreprocessSuffix): pikascript/pikascript-lib/PikaStdLib/PikaStdLib_SysObj.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/PikaStdLib_PikaStdLib_SysObj$(PreprocessSuffix) pikascript/pikascript-lib/PikaStdLib/PikaStdLib_SysObj.c

$(IntermediateDirectory)/W801Device_W801_common$(ObjectSuffix): pikascript/pikascript-lib/W801Device/W801_common.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/W801Device/W801_common.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/W801Device_W801_common$(ObjectSuffix) -MF$(IntermediateDirectory)/W801Device_W801_common$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/W801Device_W801_common$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/W801Device_W801_common$(PreprocessSuffix): pikascript/pikascript-lib/W801Device/W801_common.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/W801Device_W801_common$(PreprocessSuffix) pikascript/pikascript-lib/W801Device/W801_common.c

$(IntermediateDirectory)/W801Device_W801_GPIO$(ObjectSuffix): pikascript/pikascript-lib/W801Device/W801_GPIO.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/W801Device/W801_GPIO.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/W801Device_W801_GPIO$(ObjectSuffix) -MF$(IntermediateDirectory)/W801Device_W801_GPIO$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/W801Device_W801_GPIO$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/W801Device_W801_GPIO$(PreprocessSuffix): pikascript/pikascript-lib/W801Device/W801_GPIO.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/W801Device_W801_GPIO$(PreprocessSuffix) pikascript/pikascript-lib/W801Device/W801_GPIO.c

$(IntermediateDirectory)/W801Device_W801_PWM$(ObjectSuffix): pikascript/pikascript-lib/W801Device/W801_PWM.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/W801Device/W801_PWM.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/W801Device_W801_PWM$(ObjectSuffix) -MF$(IntermediateDirectory)/W801Device_W801_PWM$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/W801Device_W801_PWM$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/W801Device_W801_PWM$(PreprocessSuffix): pikascript/pikascript-lib/W801Device/W801_PWM.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/W801Device_W801_PWM$(PreprocessSuffix) pikascript/pikascript-lib/W801Device/W801_PWM.c

$(IntermediateDirectory)/W801Device_W801_Time$(ObjectSuffix): pikascript/pikascript-lib/W801Device/W801_Time.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/W801Device/W801_Time.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/W801Device_W801_Time$(ObjectSuffix) -MF$(IntermediateDirectory)/W801Device_W801_Time$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/W801Device_W801_Time$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/W801Device_W801_Time$(PreprocessSuffix): pikascript/pikascript-lib/W801Device/W801_Time.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/W801Device_W801_Time$(PreprocessSuffix) pikascript/pikascript-lib/W801Device/W801_Time.c

$(IntermediateDirectory)/W801Device_W801_Uart$(ObjectSuffix): pikascript/pikascript-lib/W801Device/W801_Uart.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/W801Device/W801_Uart.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/W801Device_W801_Uart$(ObjectSuffix) -MF$(IntermediateDirectory)/W801Device_W801_Uart$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/W801Device_W801_Uart$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/W801Device_W801_Uart$(PreprocessSuffix): pikascript/pikascript-lib/W801Device/W801_Uart.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/W801Device_W801_Uart$(PreprocessSuffix) pikascript/pikascript-lib/W801Device/W801_Uart.c

$(IntermediateDirectory)/W801Device_fifo$(ObjectSuffix): pikascript/pikascript-lib/W801Device/fifo.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/W801Device/fifo.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/W801Device_fifo$(ObjectSuffix) -MF$(IntermediateDirectory)/W801Device_fifo$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/W801Device_fifo$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/W801Device_fifo$(PreprocessSuffix): pikascript/pikascript-lib/W801Device/fifo.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/W801Device_fifo$(PreprocessSuffix) pikascript/pikascript-lib/W801Device/fifo.c

$(IntermediateDirectory)/W801Device_W801_IIC$(ObjectSuffix): pikascript/pikascript-lib/W801Device/W801_IIC.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/W801Device/W801_IIC.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/W801Device_W801_IIC$(ObjectSuffix) -MF$(IntermediateDirectory)/W801Device_W801_IIC$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/W801Device_W801_IIC$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/W801Device_W801_IIC$(PreprocessSuffix): pikascript/pikascript-lib/W801Device/W801_IIC.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/W801Device_W801_IIC$(PreprocessSuffix) pikascript/pikascript-lib/W801Device/W801_IIC.c

$(IntermediateDirectory)/W801Device_W801_ADC$(ObjectSuffix): pikascript/pikascript-lib/W801Device/W801_ADC.c  
	$(CC) $(SourceSwitch) pikascript/pikascript-lib/W801Device/W801_ADC.c $(CFLAGS) -MMD -MP -MT$(IntermediateDirectory)/W801Device_W801_ADC$(ObjectSuffix) -MF$(IntermediateDirectory)/W801Device_W801_ADC$(DependSuffix) $(ObjectSwitch)$(IntermediateDirectory)/W801Device_W801_ADC$(ObjectSuffix) $(IncludeCPath) $(IncludePackagePath)
Lst/W801Device_W801_ADC$(PreprocessSuffix): pikascript/pikascript-lib/W801Device/W801_ADC.c
	$(CC) $(CFLAGS)$(IncludeCPath) $(PreprocessOnlySwitch) $(OutputSwitch) Lst/W801Device_W801_ADC$(PreprocessSuffix) pikascript/pikascript-lib/W801Device/W801_ADC.c


$(IntermediateDirectory)/__rt_entry$(ObjectSuffix): $(IntermediateDirectory)/__rt_entry$(DependSuffix)
	@$(AS) $(SourceSwitch) $(ProjectPath)/$(IntermediateDirectory)/__rt_entry.S $(ASFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/__rt_entry$(ObjectSuffix) $(IncludeAPath)
$(IntermediateDirectory)/__rt_entry$(DependSuffix):
	@$(CC) $(CFLAGS) $(IncludeAPath) -MG -MP -MT$(IntermediateDirectory)/__rt_entry$(ObjectSuffix) -MF$(IntermediateDirectory)/__rt_entry$(DependSuffix) -MM $(ProjectPath)/$(IntermediateDirectory)/__rt_entry.S

-include $(IntermediateDirectory)/*$(DependSuffix)
