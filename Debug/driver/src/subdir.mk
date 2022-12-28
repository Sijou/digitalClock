################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../driver/src/gpio.c \
../driver/src/i2c.c 

OBJS += \
./driver/src/gpio.o \
./driver/src/i2c.o 

C_DEPS += \
./driver/src/gpio.d \
./driver/src/i2c.d 


# Each subdirectory must supply rules for building sources it contributes
driver/src/%.o driver/src/%.su: ../driver/src/%.c driver/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -c -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitalihr/app/inc" -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitalihr/keypad" -I"C:/Users/Lenovo/Desktop/Uni/Workspace/WiSe2223_GruppeK3_s_Projektaufgabe_Digitalihr/driver/inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-driver-2f-src

clean-driver-2f-src:
	-$(RM) ./driver/src/gpio.d ./driver/src/gpio.o ./driver/src/gpio.su ./driver/src/i2c.d ./driver/src/i2c.o ./driver/src/i2c.su

.PHONY: clean-driver-2f-src

