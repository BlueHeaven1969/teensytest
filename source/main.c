/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/

#include <string.h>
#include <stdio.h>

#include "hw_config.h"
#include "clock_config.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_debug_console.h"


/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"


// DEFINES  ====================================================================
/* Task priorities. */
#define main_task_PRIORITY (configMAX_PRIORITIES - 1)

// GLOBAL VARIABLES  ===========================================================


// PROTOTYPES  =================================================================

// FUNCTIONS  ==================================================================
/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void main_task(void *handle) {
    const TickType_t Delay = pdMS_TO_TICKS( 500UL );

    PRINTF("Hello World!!!\r\n");
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

	/* Init board hardware. */
	BOARD_InitPins();
	BOARD_BootClockRUN();
	BOARD_InitDebugConsole();
	//InitPeripherals();

	// Init on board LED
	GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_PIN, &led_config);

	/* Create RTOS task */
	xTaskCreate(main_task, "Main_task",
	            5000L/sizeof(portSTACK_TYPE), NULL, main_task_PRIORITY,
	            NULL);
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

    // PORTE0-5 configured for SDHC0
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_D1_PIN, kPORT_MuxAlt4);
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_D0_PIN, kPORT_MuxAlt4);
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_CLK_PIN, kPORT_MuxAlt4);
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_CMD_PIN, kPORT_MuxAlt4);
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_D3_PIN, kPORT_MuxAlt4);
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_D2_PIN, kPORT_MuxAlt4);

}

void BOARD_InitDebugConsole(void)
{
    uint32_t uartClkSrcFreq = BOARD_DEBUG_UART_CLK_FREQ;
    DbgConsole_Init(BOARD_DEBUG_UART_BASEADDR, BOARD_DEBUG_UART_BAUDRATE, BOARD_DEBUG_UART_TYPE, uartClkSrcFreq);
}

