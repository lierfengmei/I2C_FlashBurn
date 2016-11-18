################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
D:/Program\ files/CCSproject_source/my_i2cFLASHBURN/I2C_FLASHBURN/Debug/I2C_FLASHBURN_main.obj: D:/Program\ files/CCSproject_source/my_i2cFLASHBURN/I2C_FLASHBURN/I2C_FLASHBURN_main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: C5500 Compiler'
	"D:/Program files/CCS5.5/ccsv5/tools/compiler/c5500_4.4.1/bin/cl55" --memory_model=small -g --include_path="D:/Program files/CCS5.5/ccsv5/tools/compiler/c5500_4.4.1/include" --include_path="D:/Program files/CCS5.5/csl/include" --include_path="D:/Program files/CCS5.5/xdais_7_21_01_07/packages/ti/xdais" --include_path="/include" --define="_DEBUG" --define="CHIP_5509" --display_error_number --diag_warning=225 --obj_directory="D:/Program files/CCSproject_source/my_i2cFLASHBURN/I2C_FLASHBURN/Debug" --preproc_with_compile --preproc_dependency="I2C_FLASHBURN_main.pp" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


