################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/src/main.c 

OBJS += \
./app/src/main.o 

C_DEPS += \
./app/src/main.d 


# Each subdirectory must supply rules for building sources it contributes
app/src/%.o app/src/%.su: ../app/src/%.c app/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -c -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr/app/inc" -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr/display" -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr/driver/inc" -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitaluhr/keypad" -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-app-2f-src

clean-app-2f-src:
	-$(RM) ./app/src/main.d ./app/src/main.o ./app/src/main.su

.PHONY: clean-app-2f-src

