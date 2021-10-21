SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/kernel/freertos/builds/MSP_EXP432E401Y/release

CC = "$(CCS_ARMCOMPILER)/bin/armcl"
LNK = "$(CCS_ARMCOMPILER)/bin/armcl"

OBJECTS = ti_drivers_net_wifi_config.obj main_freertos.obj ndk_config_freertos.obj sl_host.obj ethernet_wifi_tcp_echo.obj ethernet_tcp_handler.obj ethernet_tcp_handler_hooks.obj wifi_tcp_handler.obj network_if.obj ti_drivers_config.obj

NAME = ethernet_wifi_tcp_echo

CFLAGS = -I../.. \
    -DENABLE_EMAC \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/net/bsd" \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/CMSIS/Include" \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/mbedtls/include/" \
    --advice:power=none \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/posix/ccs" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/CCS/ARM_CM4F" \
    "-I$(KERNEL_BUILD)" \
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

LFLAGS = "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/net/lib/ccs/m4f/slnetsock_release.a" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/slnetif/lib/slnetifndk.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/mbedtls/ti/lib/ccs/m4f/mbedtls.a" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/timer_bios/lib/hal_timer.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/eth_stub/lib/hal_eth_stub.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/tools/hdlc/lib/hdlc.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/tools/console/lib/console_min_ipv4.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/netctrl/lib/netctrl_min_ipv4.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/nettools/lib/nettool_ipv4.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/ser_stub/lib/hal_ser_stub.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/tools/servers/lib/servers_min_ipv4.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/userled_stub/lib/hal_userled_stub.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/stack/lib/stk.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/os/lib/os_freertos.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/display/lib/display.aem4f" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/drivers/lib/drivers_msp432e4.aem4f" \
    "-l$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/net/lib/ccs/m4f/slnetsock_release.a" \
    "-l$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/drivers/net/wifi/slnetif/ccs/Release/slnetifwifi.a" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/fatfs/lib/ccs/m4f/fatfs.a" \
    "-l$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/drivers/net/wifi/ccs/rtos/simplelink.a" \
    --diag_suppress=10063 \
    "-l$(KERNEL_BUILD)/ccs/freertos.lib" \
    "-l$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/devices/msp432e4/driverlib/lib/ccs/m4f/msp432e4_driverlib.a" \
    ../../freertos/ccs/MSP_EXP432E401Y_FREERTOS.cmd \
    "-m$(NAME).map" \
    --warn_sections \
    --display_error_number \
    --diag_wrap=off \
    --rom_model \
    "-i$(CCS_ARMCOMPILER)/lib" \
    -llibc.a

all: $(NAME).out

$(KERNEL_BUILD)/ccs/freertos.lib:
	@ $(ECHOBLANKLINE)
	@ echo $@ is not built.
	@ echo You can build it by issuing $(MAKE) in $(dir $@).
	@ $(ECHOBLANKLINE)
ti_drivers_net_wifi_config.obj: ../../ti_drivers_net_wifi_config.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

main_freertos.obj: ../../freertos/main_freertos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ndk_config_freertos.obj: ../../freertos/ndk_config_freertos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

sl_host.obj: ../../sl_host.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ethernet_wifi_tcp_echo.obj: ../../ethernet_wifi_tcp_echo.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ethernet_tcp_handler.obj: ../../ethernet_tcp_handler.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ethernet_tcp_handler_hooks.obj: ../../ethernet_tcp_handler_hooks.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

wifi_tcp_handler.obj: ../../wifi_tcp_handler.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

network_if.obj: ../../network_if.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

ti_drivers_config.obj: ../../ti_drivers_config.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< --output_file=$@

$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/ccs/freertos.lib
	@ echo linking $@
	@ $(LNK) $(CFLAGS) -z $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
