################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1281/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --idiv_support=idiv0 --tmu_support=tmu0 -Ooff --include_path="C:/Users/thain/workspace_v12/controle_mci_bldc" --include_path="C:/ti/c2000/C2000Ware_5_04_00_00" --include_path="C:/Users/thain/workspace_v12/controle_mci_bldc/device" --include_path="C:/ti/c2000/C2000Ware_5_04_00_00/driverlib/f28002x/driverlib" --include_path="C:/ti/ccs1281/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --define=DEBUG --define=RAM --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/thain/workspace_v12/controle_mci_bldc/CPU1_RAM/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-1969896417: ../controle_mci_bldc.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs1281/ccs/utils/sysconfig_1.21.0/sysconfig_cli.bat" --script "C:/Users/thain/workspace_v12/controle_mci_bldc/controle_mci_bldc.syscfg" -o "syscfg" -s "C:/ti/c2000/C2000Ware_5_04_00_00/.metadata/sdk.json" -d "F28002x" --package 80QFP --part F28002x_80QFP --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/board.c: build-1969896417 ../controle_mci_bldc.syscfg
syscfg/board.h: build-1969896417
syscfg/board.cmd.genlibs: build-1969896417
syscfg/board.opt: build-1969896417
syscfg/board.json: build-1969896417
syscfg/pinmux.csv: build-1969896417
syscfg/epwm.dot: build-1969896417
syscfg/adc.dot: build-1969896417
syscfg/c2000ware_libraries.cmd.genlibs: build-1969896417
syscfg/c2000ware_libraries.opt: build-1969896417
syscfg/c2000ware_libraries.c: build-1969896417
syscfg/c2000ware_libraries.h: build-1969896417
syscfg/clocktree.h: build-1969896417
syscfg: build-1969896417

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1281/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/bin/cl2000" -v28 -ml -mt --float_support=fpu32 --idiv_support=idiv0 --tmu_support=tmu0 -Ooff --include_path="C:/Users/thain/workspace_v12/controle_mci_bldc" --include_path="C:/ti/c2000/C2000Ware_5_04_00_00" --include_path="C:/Users/thain/workspace_v12/controle_mci_bldc/device" --include_path="C:/ti/c2000/C2000Ware_5_04_00_00/driverlib/f28002x/driverlib" --include_path="C:/ti/ccs1281/ccs/tools/compiler/ti-cgt-c2000_22.6.1.LTS/include" --define=DEBUG --define=RAM --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/thain/workspace_v12/controle_mci_bldc/CPU1_RAM/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


