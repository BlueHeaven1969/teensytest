/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/
// INCLUDES

// ARM includes

// SDK includes
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_sysmpu.h"

// FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

// Project includes
#include "board.h"
#include "clock_config.h"
#include "uart.h"
#include "sdcard.h"

// DEFINES  ====================================================================
/* Task priorities. */
#define main_task_PRIORITY (configMAX_PRIORITIES - 1)

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
// FUNCTIONS  ==================================================================
/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void main_task(void *handle) {
    const TickType_t Delay = pdMS_TO_TICKS( 500UL );

    UART__SendASCII("\r\n\r\n\r\nHello World!!!\r\n");

    UART__SendASCII("\r\nMP3 Player (in progress)\r\n");

    UART__SendASCII("\r\nPlease insert SDCARD...");

    if (SDCARD__WaitForInsert()) return;

    SDCARD__ListRootDir();

    UART__SendASCII("\r\nEntering blinky...\r\n\r\n");

    for (;;)
    {
        GPIO_PortToggle(BOARD_LED_GPIO, 1<<BOARD_LED_PIN);
        vTaskDelay(Delay);
    }
}

/*!
 * @brief Application entry point.
 */
int main(void) {
    gpio_pin_config_t led_config = { kGPIO_DigitalOutput, 0 };

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

	// Create Main thread
	xTaskCreate(main_task, "Main_task",
	            5000L/sizeof(portSTACK_TYPE), NULL, main_task_PRIORITY,
	            NULL);
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
    CLOCK_EnableClock(kCLOCK_PortE);

    // PORTE9-10 is configured for UART0 RX/TX
    PORT_SetPinMux(BOARD_UART0_PORT, BOARD_UART0_TX_PIN, kPORT_MuxAlt3);
    PORT_SetPinMux(BOARD_UART0_PORT, BOARD_UART0_RX_PIN, kPORT_MuxAlt3);

    // PORTC5 (pin D8) is configured as GPIO (LED)
    PORT_SetPinMux(BOARD_LED_PORT, BOARD_LED_PIN, kPORT_MuxAsGpio);

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

