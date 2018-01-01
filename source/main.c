/**
 * This is template for main module created by New Kinetis SDK 2.x Project Wizard. Enjoy!
 **/

#include <string.h>
#include <stdio.h>

#include "pin_mux.h"
#include "clock_config.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
/*#include "fsl_debug_console.h"*/

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"


/* Task priorities. */
#define hello_task_PRIORITY (configMAX_PRIORITIES - 1)

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
static void hello_task(void *pvParameters) {
    const TickType_t Delay = pdMS_TO_TICKS( 500UL );
    printf("Hello World!!!\r\n");
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
	//InitPeripherals();

	/* Add your code here */
	GPIO_PinInit(BOARD_LED_GPIO, BOARD_LED_PIN, &led_config);

	/* Create RTOS task */
	xTaskCreate(hello_task, "Hello_task", configMINIMAL_STACK_SIZE, NULL, hello_task_PRIORITY, NULL);
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
    /* Port C Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortC);
    /* Port E Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortE);

    /* PORTC5 (pin D8) is configured as PTC5 */
    PORT_SetPinMux(BOARD_LED_PORT, BOARD_LED_PIN, kPORT_MuxAsGpio);

    /* PORTE0 (pin D3) is configured as SDHC0_D1 */
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_D1_PIN, kPORT_MuxAlt4);

    /* PORTE1 (pin D2) is configured as SDHC0_D0 */
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_D0_PIN, kPORT_MuxAlt4);

    /* PORTE2 (pin D1) is configured as SDHC0_DCLK */
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_CLK_PIN, kPORT_MuxAlt4);

    /* PORTE3 (pin E4) is configured as SDHC0_CMD */
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_CMD_PIN, kPORT_MuxAlt4);

    /* PORTE4 (pin E3) is configured as SDHC0_D3 */
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_D3_PIN, kPORT_MuxAlt4);

    /* PORTE5 (pin E2) is configured as SDHC0_D2 */
    PORT_SetPinMux(BOARD_SDHC0_PORT, BOARD_SDHC0_D2_PIN, kPORT_MuxAlt4);
}


