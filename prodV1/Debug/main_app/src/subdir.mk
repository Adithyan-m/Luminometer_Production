################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../main_app/src/flash.c \
../main_app/src/levelSense.c \
../main_app/src/main_app.c \
../main_app/src/pulses.c \
../main_app/src/states.c \
../main_app/src/stepperDriver.c 

OBJS += \
./main_app/src/flash.o \
./main_app/src/levelSense.o \
./main_app/src/main_app.o \
./main_app/src/pulses.o \
./main_app/src/states.o \
./main_app/src/stepperDriver.o 

C_DEPS += \
./main_app/src/flash.d \
./main_app/src/levelSense.d \
./main_app/src/main_app.d \
./main_app/src/pulses.d \
./main_app/src/states.d \
./main_app/src/stepperDriver.d 


# Each subdirectory must supply rules for building sources it contributes
main_app/src/%.o main_app/src/%.su main_app/src/%.cyclo: ../main_app/src/%.c main_app/src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../FATFS/Target -I../FATFS/App -I../USB_HOST/App -I../USB_HOST/Target -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Middlewares/Third_Party/FatFs/src -I../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/WD/Lumino_Prod/prodV1/main_app/includes" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-main_app-2f-src

clean-main_app-2f-src:
	-$(RM) ./main_app/src/flash.cyclo ./main_app/src/flash.d ./main_app/src/flash.o ./main_app/src/flash.su ./main_app/src/levelSense.cyclo ./main_app/src/levelSense.d ./main_app/src/levelSense.o ./main_app/src/levelSense.su ./main_app/src/main_app.cyclo ./main_app/src/main_app.d ./main_app/src/main_app.o ./main_app/src/main_app.su ./main_app/src/pulses.cyclo ./main_app/src/pulses.d ./main_app/src/pulses.o ./main_app/src/pulses.su ./main_app/src/states.cyclo ./main_app/src/states.d ./main_app/src/states.o ./main_app/src/states.su ./main_app/src/stepperDriver.cyclo ./main_app/src/stepperDriver.d ./main_app/src/stepperDriver.o ./main_app/src/stepperDriver.su

.PHONY: clean-main_app-2f-src

