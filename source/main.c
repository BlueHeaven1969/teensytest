/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 *
 * EXTERNAL CONNECTIONS TO TEENSY 3.6 BOARD:
 * SDCard is onboard, connected to PTE0-5
 * SDCard Insertion switch is on PTC8, pin 35 on Teensy
 * Debug UART is on UART0, PTB17 (Tx Only), pin 1 on Teensy
 * IRDA is on PTC3, pin 9 on Teensy
 * Character LCD is on I2C0, PTD2-3, pins 7, 8 on Teensy
 * Character LCD GPIO for backlight is on PTD4, pin 6 on Teensy
 * Graphic LCD is on SPI0 (Tx only), PTA14-16, pins 26, 27, 28 on Teensy
 * Graphic LCD GPIO for Cmd/Data is on PTB18, pin 29 on Teensy
 * Graphic LCD GPIO for Reset is on PTB19, Pin 30 on Teensy
 * Piano Audio is on I2C1, PTC10-11, pins 37, 38 on Teensy
 * Piano Audio is on I2S0, MCLK on PTA17, pin 39 on Teensy
 *                         BCLK on PTA5,  pin 25 on Teensy
 *                         FCLK on PTC2,  pin 23 on Teensy
 *                         TXD0 on PTC1,  pin 22 on Teensy
 * Piano Audio GPIO for Mute is on PTB2, pin 19 on Teensy
 * Piano Audio GPIO for Amp Shutdown is on PTB3, pin 18 on Teensy
 * RGB LED is on PTD0, PTA12-13, pins 2, 3, 4 on Teensy
 *
 * POWER CONNECTIONS:
 * Power provided by USB connection on Teensy
 * Debug UART powered externally
 * IRDA powered from 3.3V
 * Character LCD powered from 5.0V
 * Graphic LCD powered from 3.3V
 * Piano Audio powered from 5.0V
 *
 **/
// INCLUDES

// ARM includes

// SDK includes
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_sysmpu.h"
#include "fsl_lptmr.h"

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

// Project includes
#include "board.h"
#include "clock_config.h"
#include "drv_uart.h"
#include "drv_irda.h"
#include "drv_sdcard.h"
#include "player.h"
#include "drv_clcd.h"

// DEFINES  ====================================================================
/* Task priorities. */
#define main_task_PRIORITY (configMAX_PRIORITIES - 1)
#define TIMER_CLOCK    (CLOCK_GetFreq(kCLOCK_BusClk)/64)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
uint8_t ucHeap[ configTOTAL_HEAP_SIZE ] __attribute__ ( ( section( ".my_heap" ) ) );

// FUNCTIONS  ==================================================================
/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void main_task(void *handle) {
    const TickType_t Delay = pdMS_TO_TICKS( 500UL );

    UART__SendASCII("\r\n\r\n\r\nHello World!!!\r\n", UART_COLOR_YELLOW);

    UART__SendASCII("MP3 Player (in progress)\r\n", UART_COLOR_DEFAULT);

    UART__SendASCII("\r\nPlease insert SDCARD...", UART_COLOR_DEFAULT);

    if (!SDCARD__WaitForInsert()) {

        UART__SendASCII("\r\nLaunching Player...\r\n", UART_COLOR_DEFAULT);

        if (xTaskCreate(player_task, "Player Main", 0x800, NULL,
                player_main_task_PRIORITY, NULL) != pdPASS)
        {
            UART__SendASCII("ERROR STARTING PLAYER THREAD!!\r\n", UART_COLOR_RED);
        }
    }
    else
    {
        UART__SendASCII("\r\nProblem with card insertion, player not started!\r\n", UART_COLOR_RED);

    }

    for (;;)
    {

        vTaskDelay(Delay);
    }
}

/*!
 * @brief Application entry point.
 */
int main(void) {
    gpio_pin_config_t led_config = { kGPIO_DigitalOutput, 0 };
    lptmr_config_t lptmrConfig;

    // Initialize low level hardware
    BOARD_InitPins();
    BOARD_BootClockRUN();
    SYSMPU_Enable(SYSMPU, false);

    // Initialize peripherals
    // *** UART -- debugging UART (UART0)
    UART__Init();

    // *** GPIO -- on board LED
    GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_PIN, &led_config);

    // *** SDHC -- SDCARD
    SDCARD__Init();

    // *** IRDA -- Infrared receiver
    IRDA__Init();

    // *** SPI2 -- Character LCD
    CLCD_Init();

    // Set up simple timer for LED
    LPTMR_GetDefaultConfig(&lptmrConfig);
    LPTMR_Init(LPTMR0, &lptmrConfig);
    LPTMR_SetTimerPeriod(LPTMR0, USEC_TO_COUNT(500000U,
                                 CLOCK_GetFreq(kCLOCK_LpoClk)));
    LPTMR_EnableInterrupts(LPTMR0, kLPTMR_TimerInterruptEnable);
    EnableIRQ(LPTMR0_IRQn);
    LPTMR_StartTimer(LPTMR0);

    // Create Main thread (512 byte allocation from my_heap)
    xTaskCreate(main_task, "Main_task", 0x200, NULL, main_task_PRIORITY, NULL);
    // Create IRDA receive thread
    xTaskCreate(IRDA__Receive, "IRDA Rx", 0x200, NULL, irda_task_PRIORITY, NULL);
    // start RTOS
    vTaskStartScheduler();

    for(;;)
    { /* Infinite loop to avoid leaving the main function */
        __asm("NOP"); /* something to use as a breakpoint stop while looping */
    }
}

/* FUNCTION ************************************************************************************************************
 *
 * Function Name : BOARD_InitPins
 * Description   : Configures pin routing and optionally pin electrical features.
 *
 * END ****************************************************************************************************************/
void BOARD_InitPins(void)
{
    // Enable port clocks
    CLOCK_EnableClock(kCLOCK_PortB);
    CLOCK_EnableClock(kCLOCK_PortC);
    CLOCK_EnableClock(kCLOCK_PortD);
    CLOCK_EnableClock(kCLOCK_PortE);

    // PORTB17 is configured for UART0 RX/TX
    PORT_SetPinMux(BOARD_UART0_PORT, BOARD_UART0_TX_PIN, kPORT_MuxAlt3);
    //PORT_SetPinMux(BOARD_UART0_PORT, BOARD_UART0_RX_PIN, kPORT_MuxAlt3);

    // PORTC3 is configured as GPIO for IRDA input
    PORT_SetPinMux(BOARD_IRDA_PORT, BOARD_IRDA_CMP1_PIN, kPORT_MuxAsGpio);

    // PORTC5 (pin D8) is configured as GPIO (LED)
    PORT_SetPinMux(BOARD_LED_PORT, BOARD_LED_PIN, kPORT_MuxAsGpio);

    // PORTB20-22 configured as SPI2
    PORT_SetPinMux(BOARD_SPI2_PORT, BOARD_SPI2_CS_PIN, kPORT_MuxAlt2);
    PORT_SetPinMux(BOARD_SPI2_PORT, BOARD_SPI2_CK_PIN, kPORT_MuxAlt2);
    PORT_SetPinMux(BOARD_SPI2_PORT, BOARD_SPI2_TX_PIN, kPORT_MuxAlt2);
    PORT_SetPinMux(BOARD_SPI2_PORT, BOARD_SPI2_RX_PIN, kPORT_MuxAlt2);

    const port_pin_config_t portd10_pinB3_config = {/* Internal pull-up resistor is enabled */
                                                    kPORT_PullUp,
                                                    /* Fast slew rate is configured */
                                                    kPORT_FastSlewRate,
                                                    /* Passive filter is disabled */
                                                    kPORT_PassiveFilterDisable,
                                                    /* Open drain is disabled */
                                                    kPORT_OpenDrainDisable,
                                                    /* Low drive strength is configured */
                                                    kPORT_LowDriveStrength,
                                                    /* Pin is configured as PTD10 */
                                                    kPORT_MuxAsGpio,
                                                    /* Pin Control Register fields [15:0] are not locked */
                                                    kPORT_UnlockRegister};
    /* PORTC8 (pin A8) is configured as GPIO (SDCARD CD) */
    PORT_SetPinConfig(BOARD_SDHC_CD_PORT_BASE, BOARD_SDHC_CD_GPIO_PIN, &portd10_pinB3_config);

    // PORTE0-5 configured for SDHC0
    const port_pin_config_t porte0_pinD3_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Fast slew rate is configured */
                                                   kPORT_FastSlewRate,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Open drain is disabled */
                                                   kPORT_OpenDrainDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as SDHC0_D1 */
                                                   kPORT_MuxAlt4,
                                                   /* Pin Control Register fields [15:0] are not locked */
                                                   kPORT_UnlockRegister};
    /* PORTE0 (pin D3) is configured as SDHC0_D1 */
    PORT_SetPinConfig(PORTE, 0U, &porte0_pinD3_config);

    const port_pin_config_t porte1_pinD2_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Fast slew rate is configured */
                                                   kPORT_FastSlewRate,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Open drain is disabled */
                                                   kPORT_OpenDrainDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as SDHC0_D0 */
                                                   kPORT_MuxAlt4,
                                                   /* Pin Control Register fields [15:0] are not locked */
                                                   kPORT_UnlockRegister};
    /* PORTE1 (pin D2) is configured as SDHC0_D0 */
    PORT_SetPinConfig(PORTE, 1U, &porte1_pinD2_config);

    const port_pin_config_t porte2_pinD1_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Fast slew rate is configured */
                                                   kPORT_FastSlewRate,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Open drain is disabled */
                                                   kPORT_OpenDrainDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as SDHC0_DCLK */
                                                   kPORT_MuxAlt4,
                                                   /* Pin Control Register fields [15:0] are not locked */
                                                   kPORT_UnlockRegister};
    /* PORTE2 (pin D1) is configured as SDHC0_DCLK */
    PORT_SetPinConfig(PORTE, 2U, &porte2_pinD1_config);

    const port_pin_config_t porte3_pinE4_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Fast slew rate is configured */
                                                   kPORT_FastSlewRate,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Open drain is disabled */
                                                   kPORT_OpenDrainDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as SDHC0_CMD */
                                                   kPORT_MuxAlt4,
                                                   /* Pin Control Register fields [15:0] are not locked */
                                                   kPORT_UnlockRegister};
    /* PORTE3 (pin E4) is configured as SDHC0_CMD */
    PORT_SetPinConfig(PORTE, 3U, &porte3_pinE4_config);

    const port_pin_config_t porte4_pinE3_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Fast slew rate is configured */
                                                   kPORT_FastSlewRate,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Open drain is disabled */
                                                   kPORT_OpenDrainDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as SDHC0_D3 */
                                                   kPORT_MuxAlt4,
                                                   /* Pin Control Register fields [15:0] are not locked */
                                                   kPORT_UnlockRegister};
    /* PORTE4 (pin E3) is configured as SDHC0_D3 */
    PORT_SetPinConfig(PORTE, 4U, &porte4_pinE3_config);

    const port_pin_config_t porte5_pinE2_config = {/* Internal pull-up resistor is enabled */
                                                   kPORT_PullUp,
                                                   /* Fast slew rate is configured */
                                                   kPORT_FastSlewRate,
                                                   /* Passive filter is disabled */
                                                   kPORT_PassiveFilterDisable,
                                                   /* Open drain is disabled */
                                                   kPORT_OpenDrainDisable,
                                                   /* Low drive strength is configured */
                                                   kPORT_LowDriveStrength,
                                                   /* Pin is configured as SDHC0_D2 */
                                                   kPORT_MuxAlt4,
                                                   /* Pin Control Register fields [15:0] are not locked */
                                                   kPORT_UnlockRegister};
    /* PORTE5 (pin E2) is configured as SDHC0_D2 */
    PORT_SetPinConfig(PORTE, 5U, &porte5_pinE2_config);

    SIM->SOPT5 = ((SIM->SOPT5 &
                   /* Mask bits to zero which are setting */
                   (~(SIM_SOPT5_UART0TXSRC_MASK)))

                  /* UART 0 transmit data source select: UART0_TX pin. */
                  | SIM_SOPT5_UART0TXSRC(SOPT5_UART0TXSRC_UART_TX));
}

// This is to toggle the on board Teensy LED
void LPTMR0_IRQHandler(void)
{
    LPTMR_ClearStatusFlags(LPTMR0, kLPTMR_TimerCompareFlag);
    GPIO_PortToggle(BOARD_LED_GPIO, 1<<BOARD_LED_PIN);
}
