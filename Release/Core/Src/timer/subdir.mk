################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/timer/timer.c 

OBJS += \
./Core/Src/timer/timer.o 

C_DEPS += \
./Core/Src/timer/timer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/timer/%.o Core/Src/timer/%.su Core/Src/timer/%.cyclo: ../Core/Src/timer/%.c Core/Src/timer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -Os -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-timer

clean-Core-2f-Src-2f-timer:
	-$(RM) ./Core/Src/timer/timer.cyclo ./Core/Src/timer/timer.d ./Core/Src/timer/timer.o ./Core/Src/timer/timer.su

.PHONY: clean-Core-2f-Src-2f-timer

