/*
   Character LCD
*/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h>

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
edma_handle_t clcd_EDMA_rx;
edma_handle_t clcd_EDMA_im;
dspi_master_edma_handle_t clcd_DSPI_tx;
uint8_t rowStarts[CLCD_NUM_ROWS] = { 0, 0x40, 0x14, 0x54};
uint8_t txtBuffer[64];
dspi_transfer_t clcd_buffer;

// FUNCTIONS
void CLCD_Init(void)
{
    dspi_master_config_t masterConfig;
    edma_config_t clcd_edma_config;
    uint32_t masterTxChannel = 1;
    uint32_t masterRxChannel = 0;
    uint32_t masterImChannel = 2;
    uint32_t srcFreq;

    // Set up DMA Mux for SPI2
    DMAMUX_Init(BOARD_SPI2_DMA_MUX_BASE);
    DMAMUX_SetSource(BOARD_SPI2_DMA_MUX_BASE, masterTxChannel, BOARD_SPI2_DMA_MUX_SOURCE);
    DMAMUX_EnableChannel(BOARD_SPI2_DMA_MUX_BASE, masterTxChannel);
    DMAMUX_SetSource(BOARD_SPI2_DMA_MUX_BASE, masterRxChannel, BOARD_SPI2_DMA_RX_SOURCE);
    DMAMUX_EnableChannel(BOARD_SPI2_DMA_MUX_BASE, masterRxChannel);

    EDMA_GetDefaultConfig(&clcd_edma_config);
    EDMA_Init(BOARD_SPI2_DMA_BASE, &clcd_edma_config);

    // Master config
    masterConfig.whichCtar = kDSPI_Ctar0;
    masterConfig.ctarConfig.baudRate = BOARD_SPI2_BAUDRATE;
    masterConfig.ctarConfig.bitsPerFrame = 8;
    masterConfig.ctarConfig.cpol = kDSPI_ClockPolarityActiveLow;
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
    memset(&clcd_EDMA_tx, 0, sizeof(clcd_EDMA_tx));
    memset(&clcd_EDMA_rx, 0, sizeof(clcd_EDMA_rx));
    memset(&clcd_EDMA_im, 0, sizeof(clcd_EDMA_im));

    EDMA_CreateHandle(&clcd_EDMA_tx, BOARD_SPI2_DMA_BASE, masterTxChannel);
    EDMA_CreateHandle(&clcd_EDMA_im, BOARD_SPI2_DMA_BASE, masterImChannel);
    EDMA_CreateHandle(&clcd_EDMA_rx, BOARD_SPI2_DMA_BASE, masterRxChannel);
    DSPI_MasterTransferCreateHandleEDMA(BOARD_SPI2_BASEADDR, &clcd_DSPI_tx, CLCD_MasterUserCallback,
                                        NULL, &clcd_EDMA_rx, &clcd_EDMA_im, &clcd_EDMA_tx);

    clcd_buffer.txData = txtBuffer;
    clcd_buffer.rxData = NULL;
    clcd_buffer.configFlags = kDSPI_MasterCtar0 | kDSPI_MasterPcsContinuous | BOARD_SPI2_MASTER_PCS ;

    CLCD_ClearScreen();
}

void CLCD_MasterUserCallback(SPI_Type *base, dspi_master_edma_handle_t *handle, 
                             status_t status, void *userData)
{

}

void CLCD_ClearScreen(void)
{
    txtBuffer[0] = 0xFE;
    txtBuffer[1] = 0x51;
    clcd_buffer.dataSize = 2;
//    DSPI_MasterTransferEDMA(BOARD_SPI2_BASEADDR, &clcd_DSPI_tx, &clcd_buffer);
    DSPI_MasterTransferBlocking(BOARD_SPI2_BASEADDR, &clcd_buffer);
}

void CLCD_PrintLine(uint8_t *text, uint8_t row, uint8_t pos)
{
    uint8_t rowPos;
    size_t len = strlen((char *)text);

    len = MIN(len, CLCD_NUM_COLS);
    row = MIN(row, CLCD_NUM_ROWS - 1);

    rowPos = rowStarts[row];

    if (pos == CLCD_POS_CENTERED )
    {
        pos = (CLCD_NUM_COLS - pos) >> 1;
    }
    else if (pos == CLCD_POS_RIGHT)
    {
        pos = CLCD_NUM_COLS - pos;
    }
    else
    {
        pos = MIN(pos, CLCD_NUM_ROWS - 1);
    }
    rowPos += pos;
    txtBuffer[0]=0xFE;
    txtBuffer[1]= rowPos;
    strncpy((char *)&(txtBuffer[2]), (char *)text, len);

    clcd_buffer.dataSize = len+2;
//    DSPI_MasterTransferEDMA(BOARD_SPI2_BASEADDR, &clcd_DSPI_tx, &clcd_buffer);
    DSPI_MasterTransferBlocking(BOARD_SPI2_BASEADDR, &clcd_buffer);

}



