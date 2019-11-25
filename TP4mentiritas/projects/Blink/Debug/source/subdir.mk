################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/App.c \
../source/DisplayBoard.c \
../source/Encoder.c \
../source/InternalControl.c \
../source/Lector.c \
../source/PIT.c \
../source/SysTick.c \
../source/Users.c \
../source/gpio.c \
../source/timer.c 

OBJS += \
./source/App.o \
./source/DisplayBoard.o \
./source/Encoder.o \
./source/InternalControl.o \
./source/Lector.o \
./source/PIT.o \
./source/SysTick.o \
./source/Users.o \
./source/gpio.o \
./source/timer.o 

C_DEPS += \
./source/App.d \
./source/DisplayBoard.d \
./source/Encoder.d \
./source/InternalControl.d \
./source/Lector.d \
./source/PIT.d \
./source/SysTick.d \
./source/Users.d \
./source/gpio.d \
./source/timer.d 


# Each subdirectory must supply rules for building sources it contributes
source/%.o: ../source/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -DCPU_MK64FN1M0VLL12 -D__USE_CMSIS -DDEBUG -I../source -I../ -I../SDK/CMSIS -I../SDK/startup -O0 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


