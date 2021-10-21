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
 *  DO NOT EDIT - This file is generated for the MSP_EXP432E401Y
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_MSP_EXP432E401Y
#ifndef DeviceFamily_MSP432E401Y
#define DeviceFamily_MSP432E401Y
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

/* PN1, LaunchPad LED D1 */
extern const uint_least8_t Board_GPIO_LED0_CONST;
#define Board_GPIO_LED0                 0
/* PN0, LaunchPad LED D2 */
extern const uint_least8_t Board_GPIO_LED1_CONST;
#define Board_GPIO_LED1                 1
/* PM7 */
extern const uint_least8_t MSP_EXP432E401Y_HOST_IRQ_CONST;
#define MSP_EXP432E401Y_HOST_IRQ        2
/* PD4 */
extern const uint_least8_t MSP_EXP432E401Y_nHIB_pin_CONST;
#define MSP_EXP432E401Y_nHIB_pin        3
/* PD0 */
extern const uint_least8_t MSP_EXP432E401Y_CS_pin_CONST;
#define MSP_EXP432E401Y_CS_pin          4
/* PJ0, LaunchPad Button USR_SW1 (Left) */
extern const uint_least8_t MSP_EXP432E401Y_GPIO_USR_SW1_CONST;
#define MSP_EXP432E401Y_GPIO_USR_SW1    5
/* PJ1, LaunchPad Button USR_SW2 (Right) */
extern const uint_least8_t MSP_EXP432E401Y_GPIO_USR_SW2_CONST;
#define MSP_EXP432E401Y_GPIO_USR_SW2    6

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)

#define Board_GPIO_LED_ON   (CONFIG_GPIO_LED_ON)
#define Board_GPIO_LED_OFF  (CONFIG_GPIO_LED_OFF)
#define Board_GPIO_LED2     (Board_GPIO_LED1)

/*
 * These macros are provided for backwards compatibility.
 * Please use the corresponding 'Board_GPIO_xxx' macros as the macros
 * below are deprecated.
 */
#define Board_LED_ON    (CONFIG_GPIO_LED_ON)
#define Board_LED_OFF   (CONFIG_GPIO_LED_OFF)

#define Board_LED0  (Board_GPIO_LED0)
#define Board_LED1  (Board_GPIO_LED1)
#define Board_LED2  (Board_GPIO_LED2)

#define Board_BUTTON0   (MSP_EXP432E401Y_GPIO_USR_SW1)
#define Board_BUTTON1   (MSP_EXP432E401Y_GPIO_USR_SW2)

/*
 *  ======== SPI ========
 */

/*
 *  MOSI: PQ2
 *  MISO: PQ3
 *  SCLK: PQ0
 *  SS: PQ1
 */
extern const uint_least8_t MSP_EXP432E401Y_SPI3_CONST;
#define MSP_EXP432E401Y_SPI3        0

/*
 *  ======== Timer ========
 */

extern const uint_least8_t Board_TIMER0_CONST;
#define Board_TIMER0                0

/*
 *  ======== UART ========
 */

/*
 *  TX: PA1
 *  RX: PA0
 *  XDS110 UART
 */
extern const uint_least8_t Board_UART0_CONST;
#define Board_UART0                 0

/*
 *  ======== Button ========
 */

/* PJ0, LaunchPad Button USR_SW1 (Left) */
extern const uint_least8_t Board_GPIO_BUTTON0_CONST;
#define Board_GPIO_BUTTON0          0
/* PJ1, LaunchPad Button USR_SW2 (Right) */
extern const uint_least8_t Board_GPIO_BUTTON1_CONST;
#define Board_GPIO_BUTTON1          1

/*
 * These macros are provided for backwards compatibility.
 * Please use the <Driver>_init functions directly rather
 * than Board_init<Driver>.
 */
#define Board_initADC               ADC_init
#define Board_initADCBuf            ADCBuf_init
#define Board_initGPIO              GPIO_init
#define Board_initI2C               I2C_init
#define Board_initPWM               PWM_init
#define Board_initSPI               SPI_init
#define Board_initUART              UART_init
#define Board_initWatchdog          Watchdog_init

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
