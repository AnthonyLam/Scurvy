################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
LD_SRCS += \
../src/lscript.ld 

C_SRCS += \
../src/mcp2515.c \
../src/can_bus.c 

OBJS += \
./src/mcp2515.o \
./src/can_bus.o 

C_DEPS += \
./src/mcp2515.d \
./src/can_bus.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MicroBlaze gcc compiler'
	mb-gcc -Wall -O0 -g3 -c -fmessage-length=0 -MT"$@" -I../../bsp_arduino/iop3_mb/include -mlittle-endian -mcpu=v9.6 -mxl-soft-mul -Wl,--no-relax -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


