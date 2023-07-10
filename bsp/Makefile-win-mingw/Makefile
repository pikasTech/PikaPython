#======================================================================================================================#
CC_SRC  := $(wildcard *.c)
CC_SRC  += $(wildcard PikaPython/pikascript-api/*.c)
CC_SRC  += $(wildcard PikaPython/pikascript-core/*.c)
CC_SRC  += $(wildcard PikaPython/pikascript-lib/PikaStdLib/*.c)

CC_INC  := .
CC_INC  += PikaPython/pikascript-api
CC_INC  += PikaPython/pikascript-core

#======================================================================================================================#
PY_SRC  := $(wildcard PikaPython/*.py)

#======================================================================================================================#
OUT     :=  pika.exe

#----------------------------------------------------------------------------------------------------------------------#
CC      :=  gcc

#----------------------------------------------------------------------------------------------------------------------#
CCFLAG  +=  -std=gnu11 -O1 -MMD -g
CCFLAG  +=  -ffunction-sections -fdata-sections
CCFLAG  +=  -flto

LDFLAG  +=  -Wl,--warn-common
LDFLAG  +=  -Wl,--gc-sections
LDFLAG  +=  -flto

#======================================================================================================================#
.DEFAULT_GOAL = all
_Comma := ,

ifneq ($(MAKECMDGOALS),clean)
  ifeq (${wildcard obj},)
    _NUL := ${shell mkdir obj}
  endif

  CC_OBJ := $(addprefix obj/,$(addsuffix .co,$(notdir $(CC_SRC))))

  $(foreach obj,$(CC_SRC),$(eval obj/$(notdir $(obj)).co : $(obj)))

  -include $(CC_OBJ:.co=.d)

  CCSuffix := $(CCFLAG) $(addprefix -I,$(CC_INC)) $(addprefix -D,$(CC_DEF))
  LDObject := $(CC_OBJ) $(addprefix -l,$(LD_LIB))
  LDSuffix := $(LDFLAG) $(addprefix -Wl$(_Comma)-L,$(LD_INC))

endif

#======================================================================================================================#
.PHONY: all
all: $(OUT)
	@echo Build Completed.

#----------------------------------------------------------------------------------------------------------------------#
PikaPython/pikascript-api/__asset_pikaModules_py_a.c : $(PY_SRC)
	@cd PikaPython && ./rust-msc-latest-win10.exe

#----------------------------------------------------------------------------------------------------------------------#
$(OUT): $(CC_OBJ)
	@$(CC) $(LDObject) $(LDSuffix) -o $(OUT)
	@echo

#----------------------------------------------------------------------------------------------------------------------#
obj/%.co:
	@echo Compile $(@F:.co=) ...
	@$(CC) -c $< -o $@ $(CCSuffix)

#----------------------------------------------------------------------------------------------------------------------#
.PHONY: clean
clean:
	@-rm -f pika.exe
	@-rm -rf ./obj
	@echo Clean Completed.
