################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/ADC.c \
../source/App.c \
../source/DAC.c \
../source/Modulador.c \
../source/PIT.c \
../source/SysTick.c \
../source/demodulatorFSK.c \
../source/gpio.c \
../source/timer.c \
../source/uart.c 

OBJS += \
./source/ADC.o \
./source/App.o \
./source/DAC.o \
./source/Modulador.o \
./source/PIT.o \
./source/SysTick.o \
./source/demodulatorFSK.o \
./source/gpio.o \
./source/timer.o \
./source/uart.o 

C_DEPS += \
./source/ADC.d \
./source/App.d \
./source/DAC.d \
./source/Modulador.d \
./source/PIT.d \
./source/SysTick.d \
./source/demodulatorFSK.d \
./source/gpio.d \
./source/timer.d \
./source/uart.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


