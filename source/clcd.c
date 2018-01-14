/*
   Character LCD
*/
#include <stdio.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "fsl_dmamux.h"
#include "fsl_dspi.h"
#include "fsl_dspi_edma.h"
#include "fsl_edma.h"

#include "board.h"
#include "clcd.h"

// PROTOTYPES
void CLCD_MasterUserCallback(SPI_Type *base, dspi_master_edma_handle_t *handle, 
                             status_t status, void *userData);

// VARIABLES
edma_handle_t clcd_EDMA_tx;
dspi_master_edma_handle_t clcd_DSPI_tx;

// FUNCTIONS
void CLCD_Init(void)
{
    dspi_master_config_t masterConfig;
    edma_config_t clcd_edma_config;
    uint32_t masterTxChannel = 0;
    uint32_t srcFreq;

    // Set up DMA Mux for SPI2
    DMAMUX_Init(BOARD_SPI2_DMA_MUX_BASE);
    DMAMUX_SetSource(BOARD_SPI2_DMA_MUX_BASE, masterTxChannel, BOARD_SPI2_DMA_MUX_SOURCE);
    DMAMUX_EnableChannel(BOARD_SPI2_DMA_MUX_BASE, masterTxChannel);

    EDMA_GetDefaultConfig(&clcd_edma_config);
    EDMA_Init(BOARD_SPI2_DMA_BASE, &clcd_edma_config);


    // Master config `
    masterConfig.whichCtar = kDSPI_Ctar0;
    masterConfig.ctarConfig.baudRate = BOARD_SPI2_BAUDRATE;
    masterConfig.ctarConfig.bitsPerFrame = 8;
    masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveHigh;
    masterConfig.ctarConfig.cpha = kDSPI_ClockPhaseFirstEdge;
    masterConfig.ctarConfig.direction = kDSPI_MsbFirst;
    masterConfig.ctarConfig.pcsToSckDelayInNanoSec = 1000000000U / BOARD_SPI2_BAUDRATE;
    masterConfig.ctarConfig.lastSckToPcsDelayInNanoSec = 1000000000U / BOARD_SPI2_BAUDRATE;
    masterConfig.ctarConfig.betweenTransferDelayInNanoSec = 1000000000U / BOARD_SPI2_BAUDRATE;
    masterConfig.whichPcs = BOARD_SPI2_PCS;
    masterConfig.pcsActiveHighOrLow = kDSPI_PcsActiveLow;
    masterConfig.enableContinuousSCK = false;
    masterConfig.enableRxFifoOverWrite = false;
    masterConfig.enableModifiedTimingFormat = false;
    masterConfig.samplePoint = kDSPI_SckToSin0Clock;
    srcFreq = BOARD_SPI2_CLKSRC;
    DSPI_MasterInit(BOARD_SPI2_BASEADDR, &masterConfig, srcFreq);

    // Set up dspi master
    EDMA_CreateHandle(&clcd_EDMA_tx, BOARD_SPI2_DMA_BASE, masterTxChannel);
    DSPI_MasterTransferCreateHandleEDMA(BOARD_SPI2_BASEADDR, &clcd_DSPI_tx, CLCD_MasterUserCallback,
                                        NULL, NULL, NULL, &clcd_EDMA_tx);


}

void CLCD_MasterUserCallback(SPI_Type *base, dspi_master_edma_handle_t *handle, 
                             status_t status, void *userData)
{

}

