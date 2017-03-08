################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Assembler.c \
../directive.c \
../main.c \
../symbols.c \
../utils.c 

OBJS += \
./Assembler.o \
./directive.o \
./main.o \
./symbols.o \
./utils.o 

C_DEPS += \
./Assembler.d \
./directive.d \
./main.d \
./symbols.d \
./utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


