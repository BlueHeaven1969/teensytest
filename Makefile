################################################################################
# Teensy Makefile
################################################################################

# COMMANDS
RM := rm -rf
CMP := arm-none-eabi-gcc
LNK := arm-none-eabi-g++
CPY := arm-none-eabi-objcopy
SIZ := arm-none-eabi-size

# OUTPUTS
ELF_OUT         := teensy_test.elf
HEX_OUT         := teensy_test.hex
SIZ_OUT         := teensy_test.siz
OUTPUT_DBG_DIR  := .build/debug/
OUTPUT_REL_DIR  := .build/release/

# FLAGS
X_FLAGS := \
-mcpu=cortex-m4 \
-mthumb \
-mfloat-abi=hard \
-mfpu=fpv4-sp-d16 \
-fmessage-length=0 \
-fsigned-char \
-ffunction-sections \
-fdata-sections \
-Wall

S_FLAGS := -x assembler-with-cpp

C_FLAGS := -std=gnu99

C_DEBUG_FLAGS := -O0 -g3

C_RELEASE_FLAGS := -Os

LD_FLAGS := \
-T "MK66FX1M0xxx18_flash.ld" \
-Xlinker --gc-sections \
-specs=nano.specs \
-specs=nosys.specs \
-Xlinker -z \
-Xlinker muldefs 

#DEFINES
DEFINES := \
-DCPU_MK66FX1M0VMD18_cm4 \
-DFSL_RTOS_FREE_RTOS \
-DSDK_OS_FREE_RTOS \
-DCPU_MK66FX1M0VMD18

#INCLUDE DIRECTORIES
INC_DIRS := \
-ICMSIS \
-Ifreertos/include \
-Ifreertos/portable \
-ISDK/drivers \
-ISDK/utilities \
-ISDK/sdmmc \
-ISDK/fatfs \
-Isource/inc

ifeq ($(MAKECMDGOALS),allrelease)
OUTPUT_DIR := $(OUTPUT_REL_DIR)
S_FFLAGS   := $(X_FLAGS) $(C_RELEASE_FLAGS) $(S_FLAGS)
C_FFLAGS   := $(X_FLAGS) $(C_RELEASE_FLAGS) $(C_FLAGS)
else
OUTPUT_DIR := $(OUTPUT_DBG_DIR)
S_FFLAGS   := $(X_FLAGS) $(C_DEBUG_FLAGS) $(S_FLAGS)
C_FFLAGS   := $(X_FLAGS) $(C_DEBUG_FLAGS) $(C_FLAGS)
endif

ifeq ($(MAKECMDGOALS),cleanrelease)
OUTPUT_DIR := $(OUTPUT_REL_DIR)
endif

# SOURCE FILES
S_SRCS := \
source/startup_MK66F18.S 

C_SRCS += \
source/clock_config.c \
source/fsl_sdmmc_event.c \
source/fsl_sdmmc_host.c \
source/main.c \
source/player.c \
source/strconv.c \
source/drivers/drv_irda.c \
source/drivers/drv_sdcard.c \
source/drivers/drv_uart.c \
source/drivers/drv_clcd.c \
freertos/source/croutine.c \
freertos/source/event_groups.c \
freertos/source/list.c \
freertos/source/queue.c \
freertos/source/tasks.c \
freertos/source/timers.c \
freertos/portable/fsl_tickless_systick.c \
freertos/portable/heap_4.c \
freertos/portable/port.c \
SDK/sdmmc/fsl_sd.c \
SDK/sdmmc/fsl_sdmmc_common.c \
SDK/fatfs/diskio.c \
SDK/fatfs/ff.c \
SDK/fatfs/freertos_syscall.c \
SDK/fatfs/fsl_sd_disk.c \
SDK/fatfs/unicode.c \
SDK/drivers/fsl_clock.c \
SDK/drivers/fsl_common.c \
SDK/drivers/fsl_dmamux.c \
SDK/drivers/fsl_dspi_edma.c \
SDK/drivers/fsl_dspi.c \
SDK/drivers/fsl_edma.c \
SDK/drivers/fsl_flash.c \
SDK/drivers/fsl_ftm.c \
SDK/drivers/fsl_gpio.c \
SDK/drivers/fsl_lptmr.c \
SDK/drivers/fsl_lpuart.c \
SDK/drivers/fsl_sdhc.c \
SDK/drivers/fsl_smc.c \
SDK/drivers/fsl_sysmpu.c \
SDK/drivers/fsl_uart.c \
CMSIS/system_MK66F18.c 

# OBJECT FILES
OBJS += $(patsubst %.c,$(OUTPUT_DIR)%.o,$(C_SRCS))
OBJS += $(patsubst %.S,$(OUTPUT_DIR)%.o,$(S_SRCS))

# DEPENDENCY FILES
C_DEPS := $(patsubst %.o,%.d,$(OBJS))

S_DEPS := $(patsubst %.S,$(OUTPUT_DIR)%.d,$(S_SRCS))

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

PRIMARY_FLASH   := $(OUTPUT_DIR)$(ELF_OUT)
SECONDARY_FLASH := $(OUTPUT_DIR)$(HEX_OUT)
SECONDARY_SIZE  := $(OUTPUT_DIR)$(SIZ_OUT)


# All Target
all: $(PRIMARY_FLASH) secondary-outputs

# All Target Release
allrelease: $(PRIMARY_FLASH) secondary-outputs

# C Compiling
$(OUTPUT_DIR)%.o: %.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	$(CMP) $(C_FFLAGS) $(DEFINES) $(INC_DIRS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

# S Compiling
$(OUTPUT_DIR)%.o: %.S
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	$(CMP) $(S_FFLAGS) $(DEFINES) $(INC_DIRS) -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


# Tool invocations
$(PRIMARY_FLASH): $(OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: Cross ARM GNU C++ Linker'
	$(LNK) $(C_FFLAGS) $(LD_FLAGS) -Wl,-Map,$(@:%.elf=%.map) -o "$@" $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

$(SECONDARY_FLASH): $(PRIMARY_FLASH)
	@echo 'Invoking: Cross ARM GNU Create Flash Image'
	$(CPY) -O ihex "$<" "$@"
	@echo 'Finished building: $@'
	@echo ' '

$(SECONDARY_SIZE): $(PRIMARY_FLASH)
	@echo 'Invoking: Cross ARM GNU Print Size'
	$(SIZ) --format=berkeley "$<"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(SECONDARY_SIZE)$(C_DEPS)$(OBJS)$(SECONDARY_FLASH)$(S_DEPS) teensy_test.elf
	-@echo ' '
	
cleanrelease:
	-$(RM) $(SECONDARY_SIZE)$(C_DEPS)$(OBJS)$(SECONDARY_FLASH)$(S_DEPS) teensy_test.elf
	-@echo ' '

secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_SIZE)

.PHONY: all clean dependents
