SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR ?= $(abspath ../../../../../../..)

include $(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/imports.mak

KERNEL_BUILD := $(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/kernel/freertos/builds/MSP_EXP432E401Y/release

CC = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"
LNK = "$(GCC_ARMCOMPILER)/bin/arm-none-eabi-gcc"

OBJECTS = ti_drivers_net_wifi_config.obj main_freertos.obj ndk_config_freertos.obj tcp_socket.obj sl_host.obj ethernet_tcp_handler_hooks.obj network_if.obj wifi_ethernet_sockets.obj ti_drivers_config.obj

NAME = wifi_ethernet_sockets

CFLAGS = -I../.. \
    -DENABLE_EMAC \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/net/bsd" \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/CMSIS/Include" \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/mbedtls/include/" \
    "-I$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/posix/gcc" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/include" \
    "-I$(FREERTOS_INSTALL_DIR)/FreeRTOS/Source/portable/GCC/ARM_CM4F" \
    "-I$(KERNEL_BUILD)" \
    -mcpu=cortex-m4 \
    -march=armv7e-m \
    -mthumb \
    -std=c99 \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -ffunction-sections \
    -fdata-sections \
    -g \
    -gstrict-dwarf \
    -Wall \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include/newlib-nano" \
    "-I$(GCC_ARMCOMPILER)/arm-none-eabi/include"

LFLAGS = "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/net/lib/gcc/m4f" \
    -l:slnetsock_release.a \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/slnetif/lib" \
    -l:slnetifndk.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/mbedtls/ti/lib/gcc/m4f" \
    -l:mbedtls.a \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/timer_bios/lib" \
    -l:hal_timer.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/eth_stub/lib" \
    -l:hal_eth_stub.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/tools/hdlc/lib" \
    -l:hdlc.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/tools/console/lib" \
    -l:console_min_ipv4.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/netctrl/lib" \
    -l:netctrl_min_ipv4.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/nettools/lib" \
    -l:nettool_ipv4.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/ser_stub/lib" \
    -l:hal_ser_stub.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/tools/servers/lib" \
    -l:servers_min_ipv4.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/hal/userled_stub/lib" \
    -l:hal_userled_stub.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/stack/lib" \
    -l:stk.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/ndk/os/lib" \
    -l:os_freertos.am4fg \
    -Wl,-T,../../freertos/gcc/MSP_EXP432E401Y_FREERTOS.lds \
    "-Wl,-Map,$(NAME).map" \
    -nostartfiles \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/display/lib" \
    -l:display.am4fg \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/drivers/lib" \
    -l:drivers_msp432e4.am4fg \
    "-L$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/net/lib/gcc/m4f/" \
    "-L$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/drivers/net/wifi/slnetif/ccs/Release/" \
    -l:slnetifwifi.a \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/third_party/fatfs/lib/gcc/m4f" \
    -l:fatfs.a \
    "-L$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/drivers/net/wifi/gcc/rtos/" \
    -l:simplelink.a \
    "-L$(KERNEL_BUILD)/gcc" \
    -l:freertos.lib \
    "-L$(SIMPLELINK_MSP432E4_SDK_INSTALL_DIR)/source/ti/devices/msp432e4/driverlib/lib/gcc/m4f" \
    -l:msp432e4_driverlib.a \
    -march=armv7e-m \
    -mthumb \
    -mfloat-abi=hard \
    -mfpu=fpv4-sp-d16 \
    -static \
    -Wl,--gc-sections \
    "-L$(GCC_ARMCOMPILER)/arm-none-eabi/lib/thumb/v7e-m/fpv4-sp/hard" \
    -lgcc \
    -lc \
    -lm \
    -lnosys \
    --specs=nano.specs

all: $(NAME).out

$(KERNEL_BUILD)/gcc/freertos.lib:
	@ $(ECHOBLANKLINE)
	@ echo $@ is not built.
	@ echo You can build it by issuing $(MAKE) in $(dir $@).
	@ $(ECHOBLANKLINE)
ti_drivers_net_wifi_config.obj: ../../ti_drivers_net_wifi_config.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

main_freertos.obj: ../../freertos/main_freertos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

ndk_config_freertos.obj: ../../freertos/ndk_config_freertos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

tcp_socket.obj: ../../tcp_socket.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

sl_host.obj: ../../sl_host.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

ethernet_tcp_handler_hooks.obj: ../../ethernet_tcp_handler_hooks.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

network_if.obj: ../../network_if.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

wifi_ethernet_sockets.obj: ../../wifi_ethernet_sockets.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

ti_drivers_config.obj: ../../ti_drivers_config.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -c -o $@

$(NAME).out: $(OBJECTS) $(KERNEL_BUILD)/gcc/freertos.lib
	@ echo linking $@
	@ $(LNK)  $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
