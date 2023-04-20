################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.c \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_audio.c \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_camera.c \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_eeprom.c \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.c \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_qspi.c \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sd.c \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.c \
../Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.c 

OBJS += \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.o \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_audio.o \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_camera.o \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_eeprom.o \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.o \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_qspi.o \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sd.o \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.o \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.o 

C_DEPS += \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.d \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_audio.d \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_camera.d \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_eeprom.d \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.d \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_qspi.d \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sd.d \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.d \
./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32746G-Discovery/%.o Drivers/BSP/STM32746G-Discovery/%.su: ../Drivers/BSP/STM32746G-Discovery/%.c Drivers/BSP/STM32746G-Discovery/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DUSE_STM32F746G_DISCO -D__FPU_PRESENT -DARM_MATH_CM7 -DUSE_IOEXPANDER -DSTM32F746xx -DUSE_HAL_DRIVER -c -I../Core/Inc -I../Drivers/BSP/STM32746g-Discovery -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/STM32F7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/DSP/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Drivers-2f-BSP-2f-STM32746G-2d-Discovery

clean-Drivers-2f-BSP-2f-STM32746G-2d-Discovery:
	-$(RM) ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.d ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.o ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery.su ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_audio.d ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_audio.o ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_audio.su ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_camera.d ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_camera.o ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_camera.su ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_eeprom.d ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_eeprom.o ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_eeprom.su ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.d ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.o ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_lcd.su ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_qspi.d ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_qspi.o ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_qspi.su ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sd.d ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sd.o ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sd.su ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.d ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.o ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_sdram.su ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.d ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.o ./Drivers/BSP/STM32746G-Discovery/stm32746g_discovery_ts.su

.PHONY: clean-Drivers-2f-BSP-2f-STM32746G-2d-Discovery

