################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
build-1328398049:
	@$(MAKE) --no-print-directory -Onone -f subdir_rules.mk build-1328398049-inproc

build-1328398049-inproc: ../release.cfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: XDCtools'
	"/home/sam/ti/xdctools_3_60_02_34_core/xs" --xdcpath="/home/sam/ti/simplelink_msp432p4_sdk_3_40_01_02/source;/home/sam/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/tirtos/packages;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4F -p ti.platforms.msp432:MSP432P4111 -r release -c "/home/sam/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

configPkg/linker.cmd: build-1328398049 ../release.cfg
configPkg/compiler.opt: build-1328398049
configPkg/: build-1328398049


