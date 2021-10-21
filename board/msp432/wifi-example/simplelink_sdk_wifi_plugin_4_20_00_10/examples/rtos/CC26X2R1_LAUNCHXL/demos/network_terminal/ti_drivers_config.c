/*
 *  ======== ti_drivers_config.c ========
 *  Configured TI-Drivers module definitions
 *
 *  DO NOT EDIT - This file is generated for the CC26X2R1_LAUNCHXL
 *  by the SysConfig tool.
 */

#include <stddef.h>
#include <stdint.h>

#ifndef DeviceFamily_CC26X2
#define DeviceFamily_CC26X2
#endif

#include <ti/devices/DeviceFamily.h>

#include "ti_drivers_config.h"

/*
 *  ============================= Display =============================
 */

#include <ti/display/Display.h>
#include <ti/display/DisplayUart.h>

#define CONFIG_Display_COUNT 1

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

const Display_Config Display_config[CONFIG_Display_COUNT] = {
    /* CONFIG_Display_0 */
    /* XDS110 UART */
    {
        .fxnTablePtr = &DisplayUartMin_fxnTable,
        .object = &displayUartObject,
        .hwAttrs = &displayUartHWAttrs
    },
};

const uint_least8_t Display_count = CONFIG_Display_COUNT;

/*
 *  =============================== DMA ===============================
 */

#include <ti/drivers/dma/UDMACC26XX.h>
#include <ti/devices/cc13x2_cc26x2/driverlib/udma.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_memmap.h>

UDMACC26XX_Object udmaCC26XXObject;

const UDMACC26XX_HWAttrs udmaCC26XXHWAttrs = {
    .baseAddr = UDMA0_BASE,
    .powerMngrId = PowerCC26XX_PERIPH_UDMA,
    .intNum = INT_DMA_ERR,
    .intPriority = (~0)
};

const UDMACC26XX_Config UDMACC26XX_config[1] = {
    {
        .object = &udmaCC26XXObject,
        .hwAttrs = &udmaCC26XXHWAttrs,
    },
};

/*
 *  =============================== GPIO ===============================
 */

#include <ti/drivers/GPIO.h>
#include <ti/drivers/gpio/GPIOCC26XX.h>

#define CONFIG_GPIO_COUNT 7

/*
 *  ======== gpioPinConfigs ========
 *  Array of Pin configurations
 */
GPIO_PinConfig gpioPinConfigs[] = {
    /* Board_GPIO_LED0 : LaunchPad LED Red */
    GPIOCC26XX_DIO_06 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED |
    GPIO_CFG_OUT_LOW,
    /* Board_GPIO_LED1 : LaunchPad LED Green */
    GPIOCC26XX_DIO_07 | GPIO_CFG_OUT_STD | GPIO_CFG_OUT_STR_MED |
    GPIO_CFG_OUT_LOW,
    /* CC26X2R1_LAUNCHXL_GPIO_HOST_IRQ */
    GPIOCC26XX_DIO_12 | GPIO_CFG_IN_PD | GPIO_CFG_IN_INT_RISING,
    /* CC26X2R1_LAUNCHXL_GPIO_nHIB_pin */
    GPIOCC26XX_DIO_22 | GPIO_DO_NOT_CONFIG,
    /* CC26X2R1_LAUNCHXL_GPIO_CS_pin */
    GPIOCC26XX_DIO_11 | GPIO_DO_NOT_CONFIG,
    /* CC26X2R1_LAUNCHXL_GPIO_S1 : LaunchPad Button BTN-1 (Left) */
    GPIOCC26XX_DIO_13 | GPIO_DO_NOT_CONFIG,
    /* CC26X2R1_LAUNCHXL_GPIO_S2 : LaunchPad Button BTN-2 (Right) */
    GPIOCC26XX_DIO_14 | GPIO_DO_NOT_CONFIG,
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
    /* Board_GPIO_LED0 : LaunchPad LED Red */
    NULL,
    /* Board_GPIO_LED1 : LaunchPad LED Green */
    NULL,
    /* CC26X2R1_LAUNCHXL_GPIO_HOST_IRQ */
    NULL,
    /* CC26X2R1_LAUNCHXL_GPIO_nHIB_pin */
    NULL,
    /* CC26X2R1_LAUNCHXL_GPIO_CS_pin */
    NULL,
    /* Board_GPIO_BUTTON0 : LaunchPad Button BTN-1 (Left) */
    NULL,
    /* Board_GPIO_BUTTON1 : LaunchPad Button BTN-2 (Right) */
    NULL,
};

const uint_least8_t Board_GPIO_LED0_CONST = Board_GPIO_LED0;
const uint_least8_t Board_GPIO_LED1_CONST = Board_GPIO_LED1;
const uint_least8_t CC26X2R1_LAUNCHXL_GPIO_HOST_IRQ_CONST =
    CC26X2R1_LAUNCHXL_GPIO_HOST_IRQ;
const uint_least8_t CC26X2R1_LAUNCHXL_GPIO_nHIB_pin_CONST =
    CC26X2R1_LAUNCHXL_GPIO_nHIB_pin;
const uint_least8_t CC26X2R1_LAUNCHXL_GPIO_CS_pin_CONST =
    CC26X2R1_LAUNCHXL_GPIO_CS_pin;
const uint_least8_t CC26X2R1_LAUNCHXL_GPIO_S1_CONST = CC26X2R1_LAUNCHXL_GPIO_S1;
const uint_least8_t CC26X2R1_LAUNCHXL_GPIO_S2_CONST = CC26X2R1_LAUNCHXL_GPIO_S2;

/*
 *  ======== GPIOCC26XX_config ========
 */
const GPIOCC26XX_Config GPIOCC26XX_config = {
    .pinConfigs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .numberOfPinConfigs = 7,
    .numberOfCallbacks = 7,
    .intPriority = (~0)
};

/*
 *  =============================== PIN ===============================
 */
#include <ti/drivers/PIN.h>
#include <ti/drivers/pin/PINCC26XX.h>

#define CONFIG_PIN_COUNT 12

const PIN_Config BoardGpioInitTable[CONFIG_PIN_COUNT + 1] = {
    /* LaunchPad LED Red, Parent Signal: Board_GPIO_LED0 GPIO Pin, (DIO6) */
    CONFIG_PIN_6 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL |
    PIN_DRVSTR_MED,
    /* LaunchPad LED Green, Parent Signal: Board_GPIO_LED1 GPIO Pin, (DIO7) */
    CONFIG_PIN_7 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL |
    PIN_DRVSTR_MED,
    /* Parent Signal: CC26X2R1_LAUNCHXL_GPIO_HOST_IRQ GPIO Pin, (DIO12) */
    CONFIG_PIN_8 | PIN_INPUT_EN | PIN_PULLDOWN,
    /* Parent Signal: CC26X2R1_LAUNCHXL_GPIO_nHIB_pin GPIO Pin, (DIO22) */
    CONFIG_PIN_9 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,
    /* Parent Signal: CC26X2R1_LAUNCHXL_GPIO_CS_pin GPIO Pin, (DIO11) */
    CONFIG_PIN_10 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL |
    PIN_DRVSTR_MIN,
    /* LaunchPad SPI Bus, Parent Signal: CC26X2R1_LAUNCHXL_SPI0 SCLK, (DIO10) */
    CONFIG_PIN_2 | PIN_INPUT_EN | PIN_PULLDOWN,
    /* LaunchPad SPI Bus, Parent Signal: CC26X2R1_LAUNCHXL_SPI0 MISO, (DIO8) */
    CONFIG_PIN_3 | PIN_INPUT_EN | PIN_PULLDOWN,
    /* LaunchPad SPI Bus, Parent Signal: CC26X2R1_LAUNCHXL_SPI0 MOSI, (DIO9) */
    CONFIG_PIN_4 | PIN_INPUT_EN | PIN_PULLDOWN,
    /* XDS110 UART, Parent Signal: Board_UART0 TX, (DIO3) */
    CONFIG_PIN_0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_HIGH | PIN_PUSHPULL,
    /* XDS110 UART, Parent Signal: Board_UART0 RX, (DIO2) */
    CONFIG_PIN_1 | PIN_INPUT_EN | PIN_PULLDOWN,
    /* LaunchPad Button BTN-1 (Left), Parent Signal: CC26X2R1_LAUNCHXL_GPIO_S1 GPIO Pin, (DIO13) */
    CONFIG_PIN_11 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_DIS,
    /* LaunchPad Button BTN-2 (Right), Parent Signal: CC26X2R1_LAUNCHXL_GPIO_S2 GPIO Pin, (DIO14) */
    CONFIG_PIN_12 | PIN_INPUT_EN | PIN_PULLUP | PIN_IRQ_DIS,

    PIN_TERMINATE
};

const PINCC26XX_HWAttrs PINCC26XX_hwAttrs = {
    .intPriority = (~0),
    .swiPriority = 0
};

/*
 *  =============================== Power ===============================
 */
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
#include "ti_drivers_config.h"

extern void PowerCC26XX_standbyPolicy(void);
extern bool PowerCC26XX_calibrate(unsigned int);

const PowerCC26X2_Config PowerCC26X2_config = {
    .enablePolicy = true,
    .policyInitFxn = NULL,
    .policyFxn = PowerCC26XX_standbyPolicy,
    .calibrateFxn = PowerCC26XX_calibrate,
    .calibrateRCOSC_LF = true,
    .calibrateRCOSC_HF = true,
    .enableTCXOFxn = NULL
};

/*
 *  =============================== SPI DMA ===============================
 */
#include <ti/drivers/SPI.h>
#include <ti/drivers/spi/SPICC26X2DMA.h>

#define CONFIG_SPI_COUNT 1

/*
 *  ======== spiCC26X2DMAObjects ========
 */
SPICC26X2DMA_Object spiCC26X2DMAObjects[CONFIG_SPI_COUNT];

/*
 *  ======== spiCC26X2DMAHWAttrs ========
 */
const SPICC26X2DMA_HWAttrs spiCC26X2DMAHWAttrs[CONFIG_SPI_COUNT] = {
    /* CC26X2R1_LAUNCHXL_SPI0 */
    /* LaunchPad SPI Bus */
    {
        .baseAddr = SSI0_BASE,
        .intNum = INT_SSI0_COMB,
        .intPriority = (~0),
        .swiPriority = 0,
        .powerMngrId = PowerCC26XX_PERIPH_SSI0,
        .defaultTxBufValue = ~0,
        .rxChannelBitMask = 1 << UDMA_CHAN_SSI0_RX,
            .txChannelBitMask = 1 << UDMA_CHAN_SSI0_TX,
            .minDmaTransferSize = 10,
            .mosiPin = IOID_9,
            .misoPin = IOID_8,
            .clkPin = IOID_10,
            .csnPin = PIN_UNASSIGNED
    },
};

/*
 *  ======== SPI_config ========
 */
const SPI_Config SPI_config[CONFIG_SPI_COUNT] = {
    /* CC26X2R1_LAUNCHXL_SPI0 */
    /* LaunchPad SPI Bus */
    {
        .fxnTablePtr = &SPICC26X2DMA_fxnTable,
        .object = &spiCC26X2DMAObjects[CC26X2R1_LAUNCHXL_SPI0],
        .hwAttrs = &spiCC26X2DMAHWAttrs[CC26X2R1_LAUNCHXL_SPI0]
    },
};

const uint_least8_t CC26X2R1_LAUNCHXL_SPI0_CONST = CC26X2R1_LAUNCHXL_SPI0;
const uint_least8_t SPI_count = CONFIG_SPI_COUNT;

/*
 *  =============================== UART ===============================
 */

#include <ti/drivers/UART.h>
#include <ti/drivers/uart/UARTCC26XX.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/power/PowerCC26X2.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_memmap.h>
#include <ti/devices/cc13x2_cc26x2/inc/hw_ints.h>

#define CONFIG_UART_COUNT 1

UARTCC26XX_Object uartCC26XXObjects[CONFIG_UART_COUNT];

static unsigned char uartCC26XXRingBuffer0[32];

static const UARTCC26XX_HWAttrsV2 uartCC26XXHWAttrs[CONFIG_UART_COUNT] = {
    {
        .baseAddr = UART0_BASE,
        .intNum = INT_UART0_COMB,
        .intPriority = (~0),
        .swiPriority = 0,
        .powerMngrId = PowerCC26XX_PERIPH_UART0,
        .ringBufPtr = uartCC26XXRingBuffer0,
        .ringBufSize = sizeof(uartCC26XXRingBuffer0),
        .rxPin = IOID_2,
        .txPin = IOID_3,
        .ctsPin = PIN_UNASSIGNED,
        .rtsPin = PIN_UNASSIGNED,
        .txIntFifoThr = UARTCC26XX_FIFO_THRESHOLD_1_8,
        .rxIntFifoThr = UARTCC26XX_FIFO_THRESHOLD_4_8,
        .errorFxn = NULL
    },
};

const UART_Config UART_config[CONFIG_UART_COUNT] = {
    {   /* Board_UART0 */
        .fxnTablePtr = &UARTCC26XX_fxnTable,
        .object = &uartCC26XXObjects[Board_UART0],
        .hwAttrs = &uartCC26XXHWAttrs[Board_UART0]
    },
};

const uint_least8_t Board_UART0_CONST = Board_UART0;
const uint_least8_t UART_count = CONFIG_UART_COUNT;

/*
 *  =============================== Button ===============================
 */
#include <ti/drivers/apps/Button.h>

#define CONFIG_BUTTON_COUNT 2
Button_Object ButtonObjects[CONFIG_BUTTON_COUNT];

static const Button_HWAttrs ButtonHWAttrs[CONFIG_BUTTON_COUNT] = {
    /* Board_GPIO_BUTTON0 */
    /* LaunchPad Button BTN-1 (Left) */
    {
        .gpioIndex = CC26X2R1_LAUNCHXL_GPIO_S1
    },
    /* Board_GPIO_BUTTON1 */
    /* LaunchPad Button BTN-2 (Right) */
    {
        .gpioIndex = CC26X2R1_LAUNCHXL_GPIO_S2
    },
};

const Button_Config Button_config[CONFIG_BUTTON_COUNT] = {
    /* Board_GPIO_BUTTON0 */
    /* LaunchPad Button BTN-1 (Left) */
    {
        .object = &ButtonObjects[Board_GPIO_BUTTON0],
        .hwAttrs = &ButtonHWAttrs[Board_GPIO_BUTTON0]
    },
    /* Board_GPIO_BUTTON1 */
    /* LaunchPad Button BTN-2 (Right) */
    {
        .object = &ButtonObjects[Board_GPIO_BUTTON1],
        .hwAttrs = &ButtonHWAttrs[Board_GPIO_BUTTON1]
    },
};

const uint_least8_t Board_GPIO_BUTTON0_CONST = Board_GPIO_BUTTON0;
const uint_least8_t Board_GPIO_BUTTON1_CONST = Board_GPIO_BUTTON1;
const uint_least8_t Button_count = CONFIG_BUTTON_COUNT;

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

    /* ==== /ti/drivers/PIN initialization ==== */
    if(PIN_init(BoardGpioInitTable) != PIN_SUCCESS)
    {
        /* Error with PIN_init */
        while(1)
        {
            ;
        }
    }
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
    .spiIndex = CC26X2R1_LAUNCHXL_SPI0,
    .hostIRQPin = CC26X2R1_LAUNCHXL_GPIO_HOST_IRQ,
    .nHIBPin = CC26X2R1_LAUNCHXL_GPIO_nHIB_pin,
    .csPin = CC26X2R1_LAUNCHXL_GPIO_CS_pin,
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
