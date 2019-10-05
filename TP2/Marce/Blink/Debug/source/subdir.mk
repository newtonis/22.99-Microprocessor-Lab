################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/App.c \
../source/CAN.c \
../source/ExternalManager.c \
../source/Posicionamiento.c \
../source/SPI.c \
../source/Sensores.c \
../source/SysTick.c \
../source/UART.c \
../source/gpio.c \
../source/i2c.c \
../source/timer.c 

OBJS += \
./source/App.o \
./source/CAN.o \
./source/ExternalManager.o \
./source/Posicionamiento.o \
./source/SPI.o \
./source/Sensores.o \
./source/SysTick.o \
./source/UART.o \
./source/gpio.o \
./source/i2c.o \
./source/timer.o 

C_DEPS += \
./source/App.d \
./source/CAN.d \
./source/ExternalManager.d \
./source/Posicionamiento.d \
./source/SPI.d \
./source/Sensores.d \
./source/SysTick.d \
./source/UART.d \
./source/gpio.d \
./source/i2c.d \
./source/timer.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


