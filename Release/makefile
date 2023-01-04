################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include keypad/subdir.mk
-include driver/src/subdir.mk
-include display/subdir.mk
-include app/src/subdir.mk
-include app/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.elf \

MAP_FILES += \
WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.list \


# All Target
all: main-build

# Main-build Target
main-build: WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.elf secondary-outputs

# Tool invocations
WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.elf WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.map: $(OBJS) $(USER_OBJS) C:\Users\Lenovo\Desktop\Uni\Workspace\WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr\STM32F303RETx_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"C:\Users\Lenovo\Desktop\Uni\Workspace\WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr\STM32F303RETx_FLASH.ld" --specs=nosys.specs -Wl,-Map="WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.elf WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.list WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
