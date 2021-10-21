SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/kernel/tirtos/builds/MSP_EXP432P4111/release

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"

OBJECTS = uart_term.obj ti_drivers_net_wifi_config.obj main_tirtos.obj transceiver_cmd.obj radio_tool.obj str.obj cmd_parser.obj netapp_cmd.obj wlan_cmd.obj network_terminal.obj socket_cmd.obj ti_drivers_config.obj

CONFIGPKG = $(KERNEL_BUILD)/ccs

NAME = network_terminal

CFLAGS = -I../.. \
    "-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/third_party/CMSIS/Include" \
    "-I$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/third_party/mbedtls/include/" \
    -D__MSP432P4111__ \
    -DDeviceFamily_MSP432P4x1xI \
    --advice:power=none \
    "-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/ti/posix/ccs" \
    --silicon_version=7M4 \
    --code_state=16 \
    --little_endian \
    --display_error_number \
    --diag_warning=255 \
    --diag_wrap=off \
    --gen_func_subsections=on \
    --float_support=FPv4SPD16 \
    --symdebug:dwarf \
    "-I$(CCS_ARMCOMPILER)/include"

LFLAGS = "-l$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/ti/display/lib/display.aem4f" \
    "-l$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/ti/drivers/lib/drivers_msp432p4x1xi.aem4f" \
    "-l$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/net/lib/ccs/m4f/slnetsock_release.a" \
    "-l$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/drivers/net/wifi/slnetif/ccs/Release/slnetifwifi.a" \
    "-l$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/third_party/fatfs/lib/ccs/m4f/fatfs.a" \
    "-l$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/drivers/net/wifi/ccs/rtos/simplelink.a" \
    "-l$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/kernel/tirtos/packages/ti/dpl/lib/dpl_msp432p4x1xi.aem4f" \
    "-l$(KERNEL_BUILD)/ccs/linker.cmd" \
    "-l$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/ti/devices/msp432p4xx/driverlib/ccs/msp432p4xx_driverlib.lib" \
    ../../tirtos/ccs/MSP_EXP432P4111_TIRTOS.cmd \
    "-m$(NAME).map" \
    --warn_sections \
    --display_error_number \
    --diag_wrap=off \
    --rom_model \
    "-i$(CCS_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

$(CONFIGPKG)/linker.cmd $(CONFIGPKG)/compiler.opt:
	@ $(ECHOBLANKLINE)
	@ echo $(CONFIGPKG) is not built.
	@ echo You can build it by issuing $(MAKE) in $(CONFIGPKG).
	@ $(ECHOBLANKLINE)

uart_term.obj: ../../uart_term.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ti_drivers_net_wifi_config.obj: ../../ti_drivers_net_wifi_config.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

main_tirtos.obj: ../../tirtos/main_tirtos.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

transceiver_cmd.obj: ../../transceiver_cmd.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

radio_tool.obj: ../../radio_tool.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

str.obj: ../../str.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

cmd_parser.obj: ../../cmd_parser.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

netapp_cmd.obj: ../../netapp_cmd.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

wlan_cmd.obj: ../../wlan_cmd.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

network_terminal.obj: ../../network_terminal.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

socket_cmd.obj: ../../socket_cmd.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

ti_drivers_config.obj: ../../ti_drivers_config.c $(CONFIGPKG)/compiler.opt
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --cmd_file=$(CONFIGPKG)/compiler.opt --output_file=$@

$(NAME).out: $(OBJECTS) $(CONFIGPKG)/linker.cmd
	@ echo linking $@
	@ $(LNK) $(CFLAGS) -z $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
