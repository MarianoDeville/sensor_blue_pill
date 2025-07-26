################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../App/Scr/app_debounce.c \
../App/Scr/app_delay_unlock.c \
../App/Scr/app_eeprom.c \
../App/Scr/app_leds.c \
../App/Scr/app_principal.c 

OBJS += \
./App/Scr/app_debounce.o \
./App/Scr/app_delay_unlock.o \
./App/Scr/app_eeprom.o \
./App/Scr/app_leds.o \
./App/Scr/app_principal.o 

C_DEPS += \
./App/Scr/app_debounce.d \
./App/Scr/app_delay_unlock.d \
./App/Scr/app_eeprom.d \
./App/Scr/app_leds.d \
./App/Scr/app_principal.d 


# Each subdirectory must supply rules for building sources it contributes
App/Scr/%.o App/Scr/%.su App/Scr/%.cyclo: ../App/Scr/%.c App/Scr/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"E:/Embebidos/STM32CubeIDE/sensor_blue_pill/App/Inc" -I"E:/Embebidos/STM32CubeIDE/sensor_blue_pill/App/Scr" -I"E:/Embebidos/STM32CubeIDE/sensor_blue_pill/MyDrv/Inc" -I"E:/Embebidos/STM32CubeIDE/sensor_blue_pill/MyDrv/Scr" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-App-2f-Scr

clean-App-2f-Scr:
	-$(RM) ./App/Scr/app_debounce.cyclo ./App/Scr/app_debounce.d ./App/Scr/app_debounce.o ./App/Scr/app_debounce.su ./App/Scr/app_delay_unlock.cyclo ./App/Scr/app_delay_unlock.d ./App/Scr/app_delay_unlock.o ./App/Scr/app_delay_unlock.su ./App/Scr/app_eeprom.cyclo ./App/Scr/app_eeprom.d ./App/Scr/app_eeprom.o ./App/Scr/app_eeprom.su ./App/Scr/app_leds.cyclo ./App/Scr/app_leds.d ./App/Scr/app_leds.o ./App/Scr/app_leds.su ./App/Scr/app_principal.cyclo ./App/Scr/app_principal.d ./App/Scr/app_principal.o ./App/Scr/app_principal.su

.PHONY: clean-App-2f-Scr

