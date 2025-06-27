################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MyDrv/Scr/drv_MRF24J40.c \
../MyDrv/Scr/drv_MRF24J40_port.c 

OBJS += \
./MyDrv/Scr/drv_MRF24J40.o \
./MyDrv/Scr/drv_MRF24J40_port.o 

C_DEPS += \
./MyDrv/Scr/drv_MRF24J40.d \
./MyDrv/Scr/drv_MRF24J40_port.d 


# Each subdirectory must supply rules for building sources it contributes
MyDrv/Scr/%.o MyDrv/Scr/%.su MyDrv/Scr/%.cyclo: ../MyDrv/Scr/%.c MyDrv/Scr/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"E:/Embebidos/STM32CubeIDE/sensor_blue_pill/App/Inc" -I"E:/Embebidos/STM32CubeIDE/sensor_blue_pill/App/Scr" -I"E:/Embebidos/STM32CubeIDE/sensor_blue_pill/MyDrv/Inc" -I"E:/Embebidos/STM32CubeIDE/sensor_blue_pill/MyDrv/Scr" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-MyDrv-2f-Scr

clean-MyDrv-2f-Scr:
	-$(RM) ./MyDrv/Scr/drv_MRF24J40.cyclo ./MyDrv/Scr/drv_MRF24J40.d ./MyDrv/Scr/drv_MRF24J40.o ./MyDrv/Scr/drv_MRF24J40.su ./MyDrv/Scr/drv_MRF24J40_port.cyclo ./MyDrv/Scr/drv_MRF24J40_port.d ./MyDrv/Scr/drv_MRF24J40_port.o ./MyDrv/Scr/drv_MRF24J40_port.su

.PHONY: clean-MyDrv-2f-Scr

