################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /home/sam/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS

GEN_OPTS__FLAG := --cmd_file="/home/sam/workspace_final/tirtos_builds_MSP_EXP432P4111_release_ccs/Debug/configPkg/compiler.opt" 
GEN_CMDS__FLAG := -l"/home/sam/workspace_final/tirtos_builds_MSP_EXP432P4111_release_ccs/Debug/configPkg/linker.cmd" 

ORDERED_OBJS += \
"./dht11.obj" \
"./main_tirtos.obj" \
"./mqtt_client_app.obj" \
"./mqtt_if.obj" \
"./network_if.obj" \
"./photoresistor.obj" \
"./ti_drivers_config.obj" \
"./ti_drivers_net_wifi_config.obj" \
"./uart_term.obj" \
"../MSP_EXP432P4111_TIRTOS.cmd" \
-l"/home/sam/ti/simplelink_sdk_wifi_plugin_4_20_00_10/source/ti/net/mqtt/lib/ccs/m4f/mqtt_release.a" \
-l"/home/sam/ti/simplelink_msp432p4_sdk_3_40_01_02/source/ti/display/lib/display.aem4f" \
-l"/home/sam/ti/simplelink_msp432p4_sdk_3_40_01_02/source/ti/drivers/lib/drivers_msp432p4x1xi.aem4f" \
-l"/home/sam/ti/simplelink_sdk_wifi_plugin_4_20_00_10/source/ti/net/lib/ccs/m4f/slnetsock_release.a" \
-l"/home/sam/ti/simplelink_sdk_wifi_plugin_4_20_00_10/source/ti/drivers/net/wifi/slnetif/ccs/Release/slnetifwifi.a" \
-l"/home/sam/ti/simplelink_msp432p4_sdk_3_40_01_02/source/third_party/fatfs/lib/ccs/m4f/fatfs.a" \
-l"/home/sam/ti/simplelink_sdk_wifi_plugin_4_20_00_10/source/ti/drivers/net/wifi/ccs/rtos/simplelink.a" \
-l"/home/sam/ti/simplelink_msp432p4_sdk_3_40_01_02/kernel/tirtos/packages/ti/dpl/lib/dpl_msp432p4x1xi.aem4f" \
$(GEN_CMDS__FLAG) \
-l"/home/sam/ti/simplelink_msp432p4_sdk_3_40_01_02/source/ti/devices/msp432p4xx/driverlib/ccs/msp432p4xx_driverlib.lib" \
-llibc.a \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
project_ok.out \

EXE_OUTPUTS__QUOTED += \
"project_ok.out" \

BIN_OUTPUTS += \
project_ok.hex \

BIN_OUTPUTS__QUOTED += \
"project_ok.hex" \


# All Target
all: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "project_ok.out"

dependents:
	-cd /home/sam/workspace_final/tirtos_builds_MSP_EXP432P4111_release_ccs/Debug && $(MAKE) all

# Tool invocations
project_ok.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS) /home/sam/workspace_final/tirtos_builds_MSP_EXP432P4111_release_ccs/Debug/tirtos_builds_MSP_EXP432P4111_release_ccs.lib
	@echo 'Building target: "$@"'
	@echo 'Invoking: Arm Linker'
	"/home/sam/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --advice:power=none --define=__MSP432P4111__ --define=DeviceFamily_MSP432P4x1xI -g --printf_support=full --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on -z -m"project_ok.map" -i"/home/sam/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/lib" --diag_wrap=off --display_error_number --warn_sections --xml_link_info="project_ok_linkInfo.xml" --rom_model -o "project_ok.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

project_ok.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: Arm Hex Utility'
	"/home/sam/ti/ccs1040/ccs/tools/compiler/ti-cgt-arm_20.2.5.LTS/bin/armhex" --diag_wrap=off -o "project_ok.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(BIN_OUTPUTS__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "dht11.obj" "main_tirtos.obj" "mqtt_client_app.obj" "mqtt_if.obj" "network_if.obj" "photoresistor.obj" "ti_drivers_config.obj" "ti_drivers_net_wifi_config.obj" "uart_term.obj" 
	-$(RM) "dht11.d" "main_tirtos.d" "mqtt_client_app.d" "mqtt_if.d" "network_if.d" "photoresistor.d" "ti_drivers_config.d" "ti_drivers_net_wifi_config.d" "uart_term.d" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/sam/workspace_final/tirtos_builds_MSP_EXP432P4111_release_ccs/Debug/tirtos_builds_MSP_EXP432P4111_release_ccs.lib:

-include ../makefile.targets

