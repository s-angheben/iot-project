SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR ?= $(abspath ../../../../../..)

include $(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/imports.mak

CC = "$(IAR_ARMCOMPILER)/bin/iccarm"
LNK = "$(IAR_ARMCOMPILER)/bin/ilinkarm"

OBJECTS = uart_term.obj ti_drivers_net_wifi_config.obj main_nortos.obj power_measure_nortos.obj platform.obj ti_drivers_config.obj

NAME = power_measurement

CFLAGS = -I.. \
    "-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source" \
    "-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/third_party/CMSIS/Include" \
    "-I$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/third_party/mbedtls/include/" \
    -D__MSP432P401R__ \
    -DDeviceFamily_MSP432P401x \
    -DNORTOS_SUPPORT \
    "-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/kernel/nortos" \
    "-I$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/kernel/nortos/posix" \
    --cpu=Cortex-M4F \
    --debug \
    --silent \
    -e \
    --aeabi \
    --thumb \
    --diag_suppress=Pa050

LFLAGS = "$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/ti/display/lib/display.arm4f" \
    "$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/ti/drivers/lib/drivers_msp432p401x.arm4f" \
    "$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/drivers/net/wifi/slnetif/iar/Release/slnetifwifi.a" \
    "$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/net/lib/iar/m4f/slnetsock_release.a" \
    "$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/third_party/fatfs/lib/iar/m4f/fatfs.a" \
    "$(SIMPLELINK_MSP432_SDK_WIFI_PLUGIN_INSTALL_DIR)/source/ti/drivers/net/wifi/iar/nortos/simplelink.a" \
    "$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/kernel/nortos/lib/nortos_msp432p401x.arm4f" \
    "$(SIMPLELINK_MSP432_SDK_INSTALL_DIR)/source/ti/devices/msp432p4xx/driverlib/iar/msp432p4xx_driverlib.a" \
    --config ../iar/MSP_EXP432P401R_NoRTOS.icf \
    --map "$(NAME).map" \
    --silent \
    --cpu=Cortex-M4F \
    --semihosting=iar_breakpoint

all: $(NAME).out

uart_term.obj: ../uart_term.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ti_drivers_net_wifi_config.obj: ../ti_drivers_net_wifi_config.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

main_nortos.obj: ../main_nortos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

power_measure_nortos.obj: ../power_measure_nortos.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

platform.obj: ../platform.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

ti_drivers_config.obj: ../ti_drivers_config.c
	@ echo Building $@
	@ $(CC) $(CFLAGS) $< -o $@

$(NAME).out: $(OBJECTS)
	@ echo linking $@
	@ $(LNK)  $(OBJECTS) $(LFLAGS) -o $(NAME).out

clean:
	@ echo Cleaning...
	@ $(RM) $(OBJECTS) > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).out > $(DEVNULL) 2>&1
	@ $(RM) $(NAME).map > $(DEVNULL) 2>&1
