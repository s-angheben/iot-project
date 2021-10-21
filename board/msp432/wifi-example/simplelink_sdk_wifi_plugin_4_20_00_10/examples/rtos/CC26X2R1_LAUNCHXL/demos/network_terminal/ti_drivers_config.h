/*
 *  ======== ti_drivers_config.h ========
 *  Configured TI-Drivers module declarations
 *
 *  The macros defines herein are intended for use by applications which
 *  directly include this header. These macros should NOT be hard coded or
 *  copied into library source code.
 *
 *  Symbols declared as const are intended for use with libraries.
 *  Library source code must extern the correct symbol--which is resolved
 *  when the application is linked.
 *
 *  DO NOT EDIT - This file is generated for the CC26X2R1_LAUNCHXL
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_CC26X2R1_LAUNCHXL
#ifndef DeviceFamily_CC26X2
#define DeviceFamily_CC26X2
#endif

#include <ti/devices/DeviceFamily.h>

#include <stdint.h>

/* support C++ sources */
#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== GPIO ========
 */

/* DIO6, LaunchPad LED Red */
extern const uint_least8_t Board_GPIO_LED0_CONST;
#define Board_GPIO_LED0                     0
/* DIO7, LaunchPad LED Green */
extern const uint_least8_t Board_GPIO_LED1_CONST;
#define Board_GPIO_LED1                     1
/* DIO12 */
extern const uint_least8_t CC26X2R1_LAUNCHXL_GPIO_HOST_IRQ_CONST;
#define CC26X2R1_LAUNCHXL_GPIO_HOST_IRQ     2
/* DIO22 */
extern const uint_least8_t CC26X2R1_LAUNCHXL_GPIO_nHIB_pin_CONST;
#define CC26X2R1_LAUNCHXL_GPIO_nHIB_pin     3
/* DIO11 */
extern const uint_least8_t CC26X2R1_LAUNCHXL_GPIO_CS_pin_CONST;
#define CC26X2R1_LAUNCHXL_GPIO_CS_pin       4
/* DIO13, LaunchPad Button BTN-1 (Left) */
extern const uint_least8_t CC26X2R1_LAUNCHXL_GPIO_S1_CONST;
#define CC26X2R1_LAUNCHXL_GPIO_S1           5
/* DIO14, LaunchPad Button BTN-2 (Right) */
extern const uint_least8_t CC26X2R1_LAUNCHXL_GPIO_S2_CONST;
#define CC26X2R1_LAUNCHXL_GPIO_S2           6

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)

#define Board_GPIO_LED_ON   (CONFIG_GPIO_LED_ON)
#define Board_GPIO_LED_OFF  (CONFIG_GPIO_LED_OFF)
#define Board_GPIO_LED2     (Board_GPIO_LED0)

#define Board_BUTTON0   (CC26X2R1_LAUNCHXL_GPIO_S1)
#define Board_BUTTON1   (CC26X2R1_LAUNCHXL_GPIO_S2)

/*
 *  ======== PIN ========
 */
#include <ti/drivers/PIN.h>

extern const PIN_Config BoardGpioInitTable[];

/* LaunchPad LED Red, Parent Signal: Board_GPIO_LED0 GPIO Pin, (DIO6) */
#define CONFIG_PIN_6    0x00000006
/* LaunchPad LED Green, Parent Signal: Board_GPIO_LED1 GPIO Pin, (DIO7) */
#define CONFIG_PIN_7    0x00000007
/* Parent Signal: CC26X2R1_LAUNCHXL_GPIO_HOST_IRQ GPIO Pin, (DIO12) */
#define CONFIG_PIN_8    0x0000000c
/* Parent Signal: CC26X2R1_LAUNCHXL_GPIO_nHIB_pin GPIO Pin, (DIO22) */
#define CONFIG_PIN_9    0x00000016
/* Parent Signal: CC26X2R1_LAUNCHXL_GPIO_CS_pin GPIO Pin, (DIO11) */
#define CONFIG_PIN_10    0x0000000b
/* LaunchPad SPI Bus, Parent Signal: CC26X2R1_LAUNCHXL_SPI0 SCLK, (DIO10) */
#define CONFIG_PIN_2    0x0000000a
/* LaunchPad SPI Bus, Parent Signal: CC26X2R1_LAUNCHXL_SPI0 MISO, (DIO8) */
#define CONFIG_PIN_3    0x00000008
/* LaunchPad SPI Bus, Parent Signal: CC26X2R1_LAUNCHXL_SPI0 MOSI, (DIO9) */
#define CONFIG_PIN_4    0x00000009
/* XDS110 UART, Parent Signal: Board_UART0 TX, (DIO3) */
#define CONFIG_PIN_0    0x00000003
/* XDS110 UART, Parent Signal: Board_UART0 RX, (DIO2) */
#define CONFIG_PIN_1    0x00000002
/* LaunchPad Button BTN-1 (Left), Parent Signal: CC26X2R1_LAUNCHXL_GPIO_S1 GPIO Pin, (DIO13) */
#define CONFIG_PIN_11    0x0000000d
/* LaunchPad Button BTN-2 (Right), Parent Signal: CC26X2R1_LAUNCHXL_GPIO_S2 GPIO Pin, (DIO14) */
#define CONFIG_PIN_12    0x0000000e

/*
 *  ======== SPI ========
 */

/*
 *  MOSI: DIO9
 *  MISO: DIO8
 *  SCLK: DIO10
 *  LaunchPad SPI Bus
 */
extern const uint_least8_t CC26X2R1_LAUNCHXL_SPI0_CONST;
#define CC26X2R1_LAUNCHXL_SPI0      0

/*
 *  ======== UART ========
 */

/*
 *  TX: DIO3
 *  RX: DIO2
 *  XDS110 UART
 */
extern const uint_least8_t Board_UART0_CONST;
#define Board_UART0                 0

/*
 *  ======== Button ========
 */

/* DIO13, LaunchPad Button BTN-1 (Left) */
extern const uint_least8_t Board_GPIO_BUTTON0_CONST;
#define Board_GPIO_BUTTON0          0
/* DIO14, LaunchPad Button BTN-2 (Right) */
extern const uint_least8_t Board_GPIO_BUTTON1_CONST;
#define Board_GPIO_BUTTON1          1

/*
 *  ======== Board_init ========
 *  Perform all required TI-Drivers initialization
 *
 *  This function should be called once at a point before any use of
 *  TI-Drivers.
 */
extern void Board_init(void);

/*
 *  ======== Board_initGeneral ========
 *  (deprecated)
 *
 *  Board_initGeneral() is defined purely for backward compatibility.
 *
 *  All new code should use Board_init() to do any required TI-Drivers
 *  initialization _and_ use <Driver>_init() for only where specific drivers
 *  are explicitly referenced by the application.  <Driver>_init() functions
 *  are idempotent.
 */
#define Board_initGeneral Board_init

#ifdef __cplusplus
}
#endif

#endif /* include guard */
