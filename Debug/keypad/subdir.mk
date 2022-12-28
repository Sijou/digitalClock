################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../keypad/keypad.c 

OBJS += \
./keypad/keypad.o 

C_DEPS += \
./keypad/keypad.d 


# Each subdirectory must supply rules for building sources it contributes
keypad/%.o keypad/%.su: ../keypad/%.c keypad/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -c -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitalihr/app/inc" -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitalihr/keypad" -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitalihr/driver/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-keypad

clean-keypad:
	-$(RM) ./keypad/keypad.d ./keypad/keypad.o ./keypad/keypad.su

.PHONY: clean-keypad

