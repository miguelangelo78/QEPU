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

# Every subdirectory with source files must be described here
SUBDIRS := 


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../complex.cpp \
../eeprom.cpp \
../flag.cpp \
../gates.cpp \
../interrupts.cpp \
../jumpstack.cpp \
../qepu.cpp \
../QEPU_Project_Atmega32.cpp \
../qmem.cpp \
../sram.cpp \
../uart.cpp \
../utils.cpp


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
complex.o \
eeprom.o \
flag.o \
gates.o \
interrupts.o \
jumpstack.o \
qepu.o \
QEPU_Project_Atmega32.o \
qmem.o \
sram.o \
uart.o \
utils.o

OBJS_AS_ARGS +=  \
complex.o \
eeprom.o \
flag.o \
gates.o \
interrupts.o \
jumpstack.o \
qepu.o \
QEPU_Project_Atmega32.o \
qmem.o \
sram.o \
uart.o \
utils.o

C_DEPS +=  \
complex.d \
eeprom.d \
flag.d \
gates.d \
interrupts.d \
jumpstack.d \
qepu.d \
QEPU_Project_Atmega32.d \
qmem.d \
sram.d \
uart.d \
utils.d

C_DEPS_AS_ARGS +=  \
complex.d \
eeprom.d \
flag.d \
gates.d \
interrupts.d \
jumpstack.d \
qepu.d \
QEPU_Project_Atmega32.d \
qmem.d \
sram.d \
uart.d \
utils.d

OUTPUT_FILE_PATH +=QEPU_Project_Atmega32.elf

OUTPUT_FILE_PATH_AS_ARGS +=QEPU_Project_Atmega32.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

# AVR32/GNU C Compiler

























./%.o: .././%.cpp
	@echo Building file: $<
	@echo Invoking: AVR8/GNU C Compiler : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -funsigned-char -funsigned-bitfields -DDEBUG  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -g2 -Wall -mmcu=atmega32 -c -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
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

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP)
	@echo Building target: $@
	@echo Invoking: AVR8/GNU Linker : 4.8.1
	$(QUOTE)C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-g++.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="QEPU_Project_Atmega32.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mmcu=atmega32  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "QEPU_Project_Atmega32.elf" "QEPU_Project_Atmega32.hex"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "QEPU_Project_Atmega32.elf" "QEPU_Project_Atmega32.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "QEPU_Project_Atmega32.elf" > "QEPU_Project_Atmega32.lss"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "QEPU_Project_Atmega32.elf" "QEPU_Project_Atmega32.srec"
	"C:\Program Files (x86)\Atmel\Atmel Toolchain\AVR8 GCC\Native\3.4.1056\avr8-gnu-toolchain\bin\avr-size.exe" "QEPU_Project_Atmega32.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "QEPU_Project_Atmega32.elf" "QEPU_Project_Atmega32.a" "QEPU_Project_Atmega32.hex" "QEPU_Project_Atmega32.lss" "QEPU_Project_Atmega32.eep" "QEPU_Project_Atmega32.map" "QEPU_Project_Atmega32.srec" "QEPU_Project_Atmega32.usersignatures"
	