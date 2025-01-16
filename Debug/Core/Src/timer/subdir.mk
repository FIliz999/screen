################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/timer/time.c 

OBJS += \
./Core/Src/timer/time.o 

C_DEPS += \
./Core/Src/timer/time.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/timer/%.o Core/Src/timer/%.su Core/Src/timer/%.cyclo: ../Core/Src/timer/%.c Core/Src/timer/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-timer

clean-Core-2f-Src-2f-timer:
	-$(RM) ./Core/Src/timer/time.cyclo ./Core/Src/timer/time.d ./Core/Src/timer/time.o ./Core/Src/timer/time.su

.PHONY: clean-Core-2f-Src-2f-timer

