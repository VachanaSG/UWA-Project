################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/st7789h2/st7789h2.c 

OBJS += \
./Drivers/BSP/Components/st7789h2/st7789h2.o 

C_DEPS += \
./Drivers/BSP/Components/st7789h2/st7789h2.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/st7789h2/%.o Drivers/BSP/Components/st7789h2/%.su: ../Drivers/BSP/Components/st7789h2/%.c Drivers/BSP/Components/st7789h2/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_STM32F746G_DISCO -D__FPU_PRESENT -DARM_MATH_CM7 -DUSE_IOEXPANDER -DSTM32F746xx -DUSE_HAL_DRIVER -c -I../Core/Inc -I../Drivers/BSP/STM32746g-Discovery -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-Components-2f-st7789h2

clean-Drivers-2f-BSP-2f-Components-2f-st7789h2:
	-$(RM) ./Drivers/BSP/Components/st7789h2/st7789h2.d ./Drivers/BSP/Components/st7789h2/st7789h2.o ./Drivers/BSP/Components/st7789h2/st7789h2.su

.PHONY: clean-Drivers-2f-BSP-2f-Components-2f-st7789h2

