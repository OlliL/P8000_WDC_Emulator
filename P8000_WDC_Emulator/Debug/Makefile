################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../wdc_avr.c \
../wdc_drv_pata.c \
../wdc_main.c \
../wdc_drv_mmc.c \
../wdc_if_disk_perf.c \
../uart.c \
../wdc_if_p8000.c \
../wdc_if_disk.c \
../wdc_par.c \
../wdc_ram.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
wdc_avr.o \
wdc_drv_pata.o \
wdc_main.o \
wdc_drv_mmc.o \
wdc_if_disk_perf.o \
uart.o \
wdc_if_p8000.o \
wdc_if_disk.o \
wdc_par.o \
wdc_ram.o

OBJS_AS_ARGS +=  \
wdc_avr.o \
wdc_drv_pata.o \
wdc_main.o \
wdc_drv_mmc.o \
wdc_if_disk_perf.o \
uart.o \
wdc_if_p8000.o \
wdc_if_disk.o \
wdc_par.o \
wdc_ram.o

C_DEPS +=  \
wdc_avr.d \
wdc_drv_pata.d \
wdc_main.d \
wdc_drv_mmc.d \
wdc_if_disk_perf.d \
uart.d \
wdc_if_p8000.d \
wdc_if_disk.d \
wdc_par.d \
wdc_ram.d

C_DEPS_AS_ARGS +=  \
wdc_avr.d \
wdc_drv_pata.d \
wdc_main.d \
wdc_drv_mmc.d \
wdc_if_disk_perf.d \
uart.d \
wdc_if_p8000.d \
wdc_if_disk.d \
wdc_par.d \
wdc_ram.d

OUTPUT_FILE_PATH +=P8000_WDC_Emulator.elf

OUTPUT_FILE_PATH_AS_ARGS +=P8000_WDC_Emulator.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler





















./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.9.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.90\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -Wall -Wundef -Werror -pedantic -pedantic-errors -mmcu=atmega1284p -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.90\gcc\dev\atmega1284p" -c -save-temps -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 4.9.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="P8000_WDC_Emulator.map" -Wl,-u,vfprintf -Wl,--start-group -Wl,-lm -Wl,-lprintf_flt  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega1284p -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.90\gcc\dev\atmega1284p"  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "P8000_WDC_Emulator.elf" "P8000_WDC_Emulator.hex"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "P8000_WDC_Emulator.elf" "P8000_WDC_Emulator.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "P8000_WDC_Emulator.elf" > "P8000_WDC_Emulator.lss"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "P8000_WDC_Emulator.elf" "P8000_WDC_Emulator.srec"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "P8000_WDC_Emulator.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "P8000_WDC_Emulator.elf" "P8000_WDC_Emulator.a" "P8000_WDC_Emulator.hex" "P8000_WDC_Emulator.lss" "P8000_WDC_Emulator.eep" "P8000_WDC_Emulator.map" "P8000_WDC_Emulator.srec" "P8000_WDC_Emulator.usersignatures"
	