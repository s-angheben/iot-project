SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/kernel/freertos/builds/MSP_EXP432E401Y/release

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = ti_drivers_net_wifi_config.obj main_freertos.obj ndk_config_freertos.obj sl_host.obj ethernet_wifi_tcp_echo.obj ethernet_tcp_handler.obj ethernet_tcp_handler_hooks.obj wifi_tcp_handler.obj network_if.obj ti_drivers_config.obj

NAME = ethernet_wifi_tcp_echo

CFLAGS = -I../.. \
    -DENABLE_EMAC \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/net/bsd" \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/CMSIS/Include" \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/mbedtls/include/" \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/posix/iar" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/IAR/ARM_CM4F" \
    "-I$(KERNEL_BUILD)" \
    --cpu=Cortex-M4F \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050

LFLAGS = "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/net/lib/iar/m4f/slnetsock_release.a" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/slnetif/lib/slnetifndk.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/mbedtls/ti/lib/iar/m4f/mbedtls.a" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/timer_bios/lib/hal_timer.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/eth_stub/lib/hal_eth_stub.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/tools/hdlc/lib/hdlc.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/tools/console/lib/console_min_ipv4.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/netctrl/lib/netctrl_min_ipv4.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/nettools/lib/nettool_ipv4.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/ser_stub/lib/hal_ser_stub.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/tools/servers/lib/servers_min_ipv4.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/userled_stub/lib/hal_userled_stub.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/stack/lib/stk.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/os/lib/os_freertos.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/display/lib/display.arm4f" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/drivers/lib/drivers_msp432e4.arm4f" \
    "$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/net/lib/iar/m4f/slnetsock_release.a" \
    "$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/drivers/net/wifi/slnetif/iar/Release/slnetifwifi.a" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/fatfs/lib/iar/m4f/fatfs.a" \
    "$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/drivers/net/wifi/iar/rtos/simplelink.a" \
    --threaded_lib \
    "$(KERNEL_BUILD)/iar/freertos.lib" \
    "$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/devices/msp432e4/driverlib/lib/iar/m4f/msp432e4_driverlib.a" \
    --config ../../freertos/iar/MSP_EXP432E401Y_FREERTOS.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4F \
    --semihosting=iar_breakpoint

all: $(NAME).out

$(KERNEL_BUILD)/iar/freertos.lib:
	@ $(ECHOBLANKLINE)
	@ echo $@ is not built.
	@ echo You can build it by issuing $(MAKE) in $(dir $@).
	@ $(ECHOBLANKLINE)
ti_drivers_net_wifi_config.obj: ../../ti_drivers_net_wifi_config.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

main_freertos.obj: ../../freertos/main_freertos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ndk_config_freertos.obj: ../../freertos/ndk_config_freertos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

sl_host.obj: ../../sl_host.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ethernet_wifi_tcp_echo.obj: ../../ethernet_wifi_tcp_echo.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ethernet_tcp_handler.obj: ../../ethernet_tcp_handler.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ethernet_tcp_handler_hooks.obj: ../../ethernet_tcp_handler_hooks.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

wifi_tcp_handler.obj: ../../wifi_tcp_handler.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

network_if.obj: ../../network_if.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ti_drivers_config.obj: ../../ti_drivers_config.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/iar/freertos.lib
	@ echo linking $@
	@ $(LNK)  $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
