################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../app/statup_stm32f303re.s 

OBJS += \
./app/statup_stm32f303re.o 

S_DEPS += \
./app/statup_stm32f303re.d 


# Each subdirectory must supply rules for building sources it contributes
app/%.o: ../app/%.s app/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-app

clean-app:
	-$(RM) ./app/statup_stm32f303re.d ./app/statup_stm32f303re.o

.PHONY: clean-app

