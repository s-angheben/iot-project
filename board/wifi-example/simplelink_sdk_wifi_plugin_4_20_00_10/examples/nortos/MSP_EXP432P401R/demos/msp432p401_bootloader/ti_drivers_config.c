/*
 *  ======== ti_drivers_config.c ========
 *  Configured TI-Drivers module definitions
 *
 *  DO NOT EDIT - This file is generated for the MSP_EXP432P401R
 *  by the SysConfig tool.
 */

#include <stddef.h>

#ifndef DeviceFamily_MSP432P401x
#define DeviceFamily_MSP432P401x
#endif

#include <ti/devices/DeviceFamily.h>

#include "ti_drivers_config.h"

/*
 *  ============================= Display =============================
 */

#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>

#define Display_UARTBUFFERSIZE 1024
static char displayUARTBuffer[Display_UARTBUFFERSIZE];

DisplayUart_Object displayUartObject;

const DisplayUart_HWAttrs displayUartHWAttrs = {
    .uartIdx = Board_UART0,
    .baudRate = 115200,
    .mutexTimeout = (unsigned int)(-1),
    .strBuf = displayUARTBuffer,
    .strBufLen = Display_UARTBUFFERSIZE
};

const Display_Config Display_config[] = {
    /* CONFIG_Display_0 */
    /* XDS110 UART */
    {
        .fxnTablePtr = &DisplayUartMin_fxnTable,
        .object = &displayUartObject,
        .hwAttrs = &displayUartHWAttrs
    },
};

const uint_least8_t Display_count = 1;

/*
 *  =============================== DMA ===============================
 */

#include <ti/drivers/dma/UDMAMSP432.h>
#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/rom.h>
#include <ti/devices/msp432p4xx/driverlib/rom_map.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>

/* Ensure DMA control table is aligned as required by the uDMA Hardware */
static DMA_ControlTable dmaControlTable[16] __attribute__ ((aligned (256)));

/* This is the handler for the uDMA error interrupt. */
static void dmaErrorFxn(uintptr_t arg)
{
    int status = MAP_DMA_getErrorStatus();
    MAP_DMA_clearErrorStatus();

    /* Suppress unused variable warning */
    (void)status;

    while(1)
    {
        ;
    }
}

UDMAMSP432_Object udmaMSP432Object;

const UDMAMSP432_HWAttrs udmaMSP432HWAttrs = {
    .controlBaseAddr = (void *)dmaControlTable,
    .dmaErrorFxn = (UDMAMSP432_ErrorFxn)dmaErrorFxn,
    .intNum = INT_DMA_ERR,
    .intPriority = (~0)
};

const UDMAMSP432_Config UDMAMSP432_config = {
    .object = &udmaMSP432Object,
    .hwAttrs = &udmaMSP432HWAttrs
};

/*
 *  =============================== GPIO ===============================
 */

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOMSP432.h>

/*
 *  ======== gpioPinConfigs ========
 *  Array of Pin configurations
 */
GPIO_PinConfig gpioPinConfigs[] = {
    /* MSP_EXP432P401R_HOST_IRQ */
    GPIOMSP432_P2_5 | GPIO_CFG_IN_PD | GPIO_CFG_IN_INT_RISING,
    /* MSP_EXP432P401R_nHIB_pin */
    GPIOMSP432_P4_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
    GPIO_CFG_OUT_LOW,
    /* MSP_EXP432P401R_CS_pin */
    GPIOMSP432_P3_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_HIGH |
    GPIO_CFG_OUT_HIGH,
    /* Board_GPIO_LED0 : LaunchPad LED 1 Red */
    GPIOMSP432_P1_0 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED |
    GPIO_CFG_OUT_LOW,
    /* Board_GPIO_LED1 : LaunchPad LED 2 Green */
    GPIOMSP432_P2_1 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED |
    GPIO_CFG_OUT_LOW,
    /* Board_GPIO_LED2 : LaunchPad LED 2 Blue */
    GPIOMSP432_P2_2 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED |
    GPIO_CFG_OUT_LOW,
    /* MSP_EXP432P401R_GPIO_S1 : LaunchPad Button S1 (Left) */
    GPIOMSP432_P1_1 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
    /* MSP_EXP432P401R_GPIO_S2 : LaunchPad Button S2 (Right) */
    GPIOMSP432_P1_4 | GPIO_CFG_IN_PU | GPIO_CFG_IN_INT_FALLING,
};

/*
 *  ======== gpioCallbackFunctions ========
 *  Array of callback function pointers
 *
 *  NOTE: Unused callback entries can be omitted from the callbacks array to
 *  reduce memory usage by enabling callback table optimization
 *  (GPIO.optimizeCallbackTableSize = true)
 */
GPIO_CallbackFxn gpioCallbackFunctions[] = {
    /* MSP_EXP432P401R_HOST_IRQ */
    NULL,
    /* MSP_EXP432P401R_nHIB_pin */
    NULL,
    /* MSP_EXP432P401R_CS_pin */
    NULL,
    /* CONFIG_GPIO_0 : LaunchPad Button S1 (Left) */
    NULL,
    /* CONFIG_GPIO_1 : LaunchPad Button S2 (Right) */
    NULL,
    /* Board_GPIO_LED0 : LaunchPad LED 1 Red */
    NULL,
    /* Board_GPIO_LED1 : LaunchPad LED 2 Green */
    NULL,
    /* Board_GPIO_LED2 : LaunchPad LED 2 Blue */
    NULL,
};

/*
 *  ======== GPIOMSP432_config ========
 */
const GPIOMSP432_Config GPIOMSP432_config = {
    .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = 8,
    .numberOfCallbacks = 8,
    .intPriority = (~0)
};

/*
 *  =============================== Power ===============================
 */

#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerMSP432.h>
extern void PowerMSP432_initPolicy(void);
extern void PowerMSP432_sleepPolicy(void);

const PowerMSP432_ConfigV1 PowerMSP432_config = {
    .policyInitFxn = PowerMSP432_initPolicy,
    .policyFxn = PowerMSP432_sleepPolicy,
    .initialPerfLevel = 2,
    .enablePolicy = true,
    .enablePerf = true,
    .enableParking = false,
    .resumeShutdownHookFxn = NULL,
    .customPerfLevels = NULL,
    .numCustom = 0,
    .useExtendedPerf = false,
    .configurePinHFXT = false,
    .HFXTFREQ = 0,
    .bypassHFXT = false,
    .configurePinLFXT = false,
    .bypassLFXT = false,
    .LFXTDRIVE = 0,
    .enableInterruptsCS = false,
    .priorityInterruptsCS = (~0),
    .isrCS = NULL
};

/*
 *  =============================== SPI ===============================
 */

#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPIMSP432DMA.h>

#include <ti/devices/msp432p4xx/inc/msp.h>
#include <ti/devices/msp432p4xx/driverlib/dma.h>
#include <ti/devices/msp432p4xx/driverlib/interrupt.h>
#include <ti/devices/msp432p4xx/driverlib/spi.h>

#define CONFIG_SPI_COUNT 1

/*
 *  ======== spiMSP432DMAObjects ========
 */
SPIMSP432DMA_Object spiMSP432DMAObjects[CONFIG_SPI_COUNT];

/*
 *  ======== spiMSP432DMAHWAttrs ========
 */
const SPIMSP432DMA_HWAttrsV1 spiMSP432DMAHWAttrs[CONFIG_SPI_COUNT] = {
    /* MSP_EXP432P401R_SPIB0 */
    {
        .baseAddr = EUSCI_B0_BASE,
        .bitOrder = EUSCI_B_SPI_MSB_FIRST,
        .clockSource = EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
        .defaultTxBufValue = 0xFF,
        .intPriority = (~0),
        .dmaIntNum = INT_DMA_INT1,
        .rxDMAChannelIndex = DMA_CH1_EUSCIB0RX0,
        .txDMAChannelIndex = DMA_CH0_EUSCIB0TX0,
        .pinMode = EUSCI_SPI_3PIN,
        .clkPin = SPIMSP432DMA_P1_5_UCB0CLK,
        .simoPin = SPIMSP432DMA_P1_6_UCB0SIMO,
        .somiPin = SPIMSP432DMA_P1_7_UCB0SOMI,
        .stePin = SPIMSP432DMA_PIN_NO_CONFIG,
        .minDmaTransferSize = 10,
    },
};

/*
 *  ======== SPI_config ========
 */
const SPI_Config SPI_config[CONFIG_SPI_COUNT] = {
    /* MSP_EXP432P401R_SPIB0 */
    {
        .fxnTablePtr = &SPIMSP432DMA_fxnTable,
        .object = &spiMSP432DMAObjects[MSP_EXP432P401R_SPIB0],
        .hwAttrs = &spiMSP432DMAHWAttrs[MSP_EXP432P401R_SPIB0]
    },
};

const uint_least8_t SPI_count = CONFIG_SPI_COUNT;

/*
 *  =============================== Timer ===============================
 */

#include <ti/drivers/Timer.h>
#include <ti/drivers/timer/TimerMSP432.h>
#include <ti/devices/msp432p4xx/driverlib/interrupt.h>
#include <ti/devices/msp432p4xx/driverlib/timer_a.h>
#include <ti/devices/msp432p4xx/driverlib/timer32.h>

#define CONFIG_TIMER_COUNT 1

/*
 *  ======== timerMSP432Objects ========
 */
TimerMSP432_Object timerMSP432Objects[CONFIG_TIMER_COUNT];

/*
 *  ======== timerMSP432HWAttrs ========
 */
const TimerMSP432_HWAttrs TimerMSP432HWAttrs[CONFIG_TIMER_COUNT] = {
    /* Board_TIMER0 */
    {
        .timerBaseAddress = TIMER32_0_BASE,
        .intNum = INT_T32_INT1,
        .intPriority = (~0),
    },
};

/*
 *  ======== Timer_config ========
 */
const Timer_Config Timer_config[CONFIG_TIMER_COUNT] = {
    /* Board_TIMER0 */
    {
        .fxnTablePtr = &TimerMSP432_Timer32_fxnTable,
        .object = &timerMSP432Objects[Board_TIMER0],
        .hwAttrs = &TimerMSP432HWAttrs[Board_TIMER0]
    },
};

const uint_least8_t Timer_count = CONFIG_TIMER_COUNT;

/*
 *  =============================== UART ===============================
 */

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTMSP432.h>
#include <ti/devices/msp432p4xx/driverlib/interrupt.h>
#include <ti/devices/msp432p4xx/driverlib/uart.h>

#define CONFIG_UART_COUNT 1

UARTMSP432_Object uartMSP432Objects[CONFIG_UART_COUNT];

static const UARTMSP432_BaudrateConfig uartMSP432Baudrates[] = {
    /* {baudrate, input clock, prescalar, UCBRFx, UCBRSx, oversampling} */
    { 115200, 3000000, 1, 10, 0, 1 },
    { 115200, 6000000, 3, 4, 2, 1 },
    { 115200, 12000000, 6, 8, 32, 1 },
    { 115200, 24000000, 13, 0, 37, 1 },
};

static unsigned char uartMSP432RingBuffer0[32];

static const UARTMSP432_HWAttrsV1 uartMSP432HWAttrs[CONFIG_UART_COUNT] = {
    {
        .baseAddr = EUSCI_A0_BASE,
        .intNum = INT_EUSCIA0,
        .intPriority = (~0),
        .clockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK,
        .bitOrder = EUSCI_A_UART_LSB_FIRST,
        .numBaudrateEntries = sizeof(uartMSP432Baudrates) /
                              sizeof(UARTMSP432_BaudrateConfig),
        .baudrateLUT = uartMSP432Baudrates,
        .ringBufPtr = uartMSP432RingBuffer0,
        .ringBufSize = sizeof(uartMSP432RingBuffer0),
        .rxPin = UARTMSP432_P1_2_UCA0RXD,
        .txPin = UARTMSP432_P1_3_UCA0TXD,
        .errorFxn = NULL
    },
};

const UART_Config UART_config[CONFIG_UART_COUNT] = {
    {   /* Board_UART0 */
        .fxnTablePtr = &UARTMSP432_fxnTable,
        .object = &uartMSP432Objects[0],
        .hwAttrs = &uartMSP432HWAttrs[0]
    },
};

const uint_least8_t UART_count = CONFIG_UART_COUNT;

/*
 *  =============================== Button ===============================
 */
#include <ti/drivers/apps/Button.h>

Button_Object ButtonObjects[2];

static const Button_HWAttrs ButtonHWAttrs[2] = {
    /* Board_GPIO_BUTTON0 */
    /* LaunchPad Button S1 (Left) */
    {
        .gpioIndex = MSP_EXP432P401R_GPIO_S1
    },
    /* Board_GPIO_BUTTON1 */
    /* LaunchPad Button S2 (Right) */
    {
        .gpioIndex = MSP_EXP432P401R_GPIO_S2
    },
};

const Button_Config Button_config[2] = {
    /* Board_GPIO_BUTTON0 */
    /* LaunchPad Button S1 (Left) */
    {
        .object = &ButtonObjects[Board_GPIO_BUTTON0],
        .hwAttrs = &ButtonHWAttrs[Board_GPIO_BUTTON0]
    },
    /* Board_GPIO_BUTTON1 */
    /* LaunchPad Button S2 (Right) */
    {
        .object = &ButtonObjects[Board_GPIO_BUTTON1],
        .hwAttrs = &ButtonHWAttrs[Board_GPIO_BUTTON1]
    },
};

const uint_least8_t Button_count = 2;

#include <ti/drivers/Board.h>

/*
 *  ======== Board_initHook ========
 *  Perform any board-specific initialization needed at startup.  This
 *  function is declared weak to allow applications to override it if needed.
 */
void __attribute__((weak)) Board_initHook(void)
{
}

/*
 *  ======== Board_init ========
 *  Perform any initialization needed before using any board APIs
 */
void Board_init(void)
{
    /* ==== /ti/drivers/Power initialization ==== */
    Power_init();

    Board_initHook();
}

/*
 *  =============================== WiFi ===============================
 *
 * This is the configuration structure for the WiFi module that will be used
 * as part of the SimpleLink SDK WiFi plugin. These are configured for SPI mode.
 */
#include <ti/drivers/net/wifi/porting/SIMPLELINKWIFI.h>

const SIMPLELINKWIFI_HWAttrsV1 wifiSimplelinkHWAttrs =
{
    .spiIndex = MSP_EXP432P401R_SPIB0,
    .hostIRQPin = MSP_EXP432P401R_HOST_IRQ,
    .nHIBPin = MSP_EXP432P401R_nHIB_pin,
    .csPin = MSP_EXP432P401R_CS_pin,
    .maxDMASize = 1024,
    .spiBitRate = 12000000
};

const uint_least8_t WiFi_count = 1;

const WiFi_Config WiFi_config[1] =
{
    {
        .hwAttrs = &wifiSimplelinkHWAttrs,
    }
};
