################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../display/display.c 

OBJS += \
./display/display.o 

C_DEPS += \
./display/display.d 


# Each subdirectory must supply rules for building sources it contributes
display/%.o display/%.su: ../display/%.c display/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -c -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr/app/inc" -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr/display" -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr/keypad" -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr/driver/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-display

clean-display:
	-$(RM) ./display/display.d ./display/display.o ./display/display.su

.PHONY: clean-display

