################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Assembler.c \
../assembler_entry.c \
../directive.c \
../instruction.c \
../logs.c \
../main.c \
../symbols.c 

OBJS += \
./Assembler.o \
./assembler_entry.o \
./directive.o \
./instruction.o \
./logs.o \
./main.o \
./symbols.o 

C_DEPS += \
./Assembler.d \
./assembler_entry.d \
./directive.d \
./instruction.d \
./logs.d \
./main.d \
./symbols.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


