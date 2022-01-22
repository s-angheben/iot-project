/*
 *  ======== ti_drivers_config.h ========
 *  Configured TI-Drivers module declarations
 *
 *  DO NOT EDIT - This file is generated for the MSP_EXP432P4111
 *  by the SysConfig tool.
 */
#ifndef ti_drivers_config_h
#define ti_drivers_config_h

#define CONFIG_SYSCONFIG_PREVIEW

#define CONFIG_MSP_EXP432P4111

#ifndef DeviceFamily_MSP432P4x1xI
#define DeviceFamily_MSP432P4x1xI
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

/* P2.0, LaunchPad LED 2 Red */
#define Board_GPIO_LED0             0
/* P2.1, LaunchPad LED 2 Green */
#define Board_GPIO_LED1             1
/* P2.2, LaunchPad LED 2 Blue */
#define Board_GPIO_LED2             2
/* P2.5 */
#define MSP_EXP432P4111_HOST_IRQ    3
/* P4.1 */
#define MSP_EXP432P4111_nHIB_pin    4
/* P3.0 */
#define MSP_EXP432P4111_CS_pin      5
/* P1.1, LaunchPad Button S1 (Left) */
#define MSP_EXP432P4111_GPIO_S1     6
/* P1.4, LaunchPad Button S2 (Right) */
#define MSP_EXP432P4111_GPIO_S2     7
/* P5.7 */
#define dht11                       8

/* LEDs are active high */
#define CONFIG_GPIO_LED_ON  (1)
#define CONFIG_GPIO_LED_OFF (0)

#define HIGH (1)
#define LOW  (0)


#define CONFIG_LED_ON  (CONFIG_GPIO_LED_ON)
#define CONFIG_LED_OFF (CONFIG_GPIO_LED_OFF)

/*
 * These macros are provided for backwards compatibility.
 * Please use the corresponding 'Board_GPIO_xxx' macros as the macros
 * below are deprecated.
 */
#define Board_GPIO_LED_ON   (CONFIG_GPIO_LED_ON)
#define Board_GPIO_LED_OFF  (CONFIG_GPIO_LED_OFF)

#define Board_LED_ON    (CONFIG_GPIO_LED_ON)
#define Board_LED_OFF   (CONFIG_GPIO_LED_OFF)

#define Board_LED0  (Board_GPIO_LED0)
#define Board_LED1  (Board_GPIO_LED1)
#define Board_LED2  (Board_GPIO_LED2)

#define MSP_EXP432P4111_GPIO_LED_RED    (Board_GPIO_LED0)
#define MSP_EXP432P4111_GPIO_LED_GREEN  (Board_GPIO_LED1)
#define MSP_EXP432P4111_GPIO_LED_BLUE   (Board_GPIO_LED2)

#define Board_BUTTON0   (MSP_EXP432P4111_GPIO_S1)
#define Board_BUTTON1   (MSP_EXP432P4111_GPIO_S2)

/*
 *  ======== SPI ========
 */

/*
 *  MOSI: P1.6
 *  MISO: P1.7
 *  SCLK: P1.5
 */
#define MSP_EXP432P4111_SPIB0       0

/*
 *  ======== Timer ========
 */

#define Board_TIMER0                0
#define Board_TIMER1                1

/*
 *  ======== ADC ========
 */

/* P6.0 */
#define CONFIG_ADC_0                0


/*
 *  ======== UART ========
 */

/*
 *  TX: P1.3
 *  RX: P1.2
 *  XDS110 UART
 */
#define Board_UART0                 0

/*
 *  ======== Button ========
 */

/* P1.1, LaunchPad Button S1 (Left) */
#define Board_GPIO_BUTTON0          0
/* P1.4, LaunchPad Button S2 (Right) */
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
