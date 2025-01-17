/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 4 $
 * $Date: 15/12/28 4:57p $
 * @brief    Access SPI Flash using FIFO mode
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "Nano103.h"

#define TEST_NUMBER 1   /* page numbers */
#define TEST_LENGTH 256 /* length */

#define SPI_FLASH_PORT  SPI0
#define SPI_WRITE_TX    SPI_WRITE_TX0
#define SPI_READ_RX     SPI_READ_RX0

uint8_t SrcArray[TEST_LENGTH];
uint8_t DestArray[TEST_LENGTH];


/**
 * @brief       Read Manufacturer/Device ID from SPI flash
 * @param       None
 * @return      Manufacturer/Device ID from SPI flash
 * @details     This function read Manufacturer/Device ID from SPI flash
 */
uint16_t SpiFlash_ReadMidDid(void)
{
    uint8_t u8RxData[6], u8IDCnt = 0;

    // /CS: active
    SPI_SET_SS0_LOW(SPI_FLASH_PORT);

    // send Command: 0x90, Read Manufacturer/Device ID
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x90);

    // send 24-bit '0', dummy
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // receive 16-bit
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);

    while(!SPI_GET_RX_FIFO_EMPTY_FLAG(SPI_FLASH_PORT))
        u8RxData[u8IDCnt ++] = SPI_READ_RX(SPI_FLASH_PORT);

    return ( (u8RxData[4]<<8) | u8RxData[5] );
}

/**
 * @brief       Erase all data on the SPI flash
 * @param       u8Value the is SPI flash status value.
 * @return      None
 * @details     This function erase all SPI flash
 */
void SpiFlash_ChipErase(void)
{
    // /CS: active
    SPI_SET_SS0_LOW(SPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);

    //////////////////////////////////////////

    // /CS: active
    SPI_SET_SS0_LOW(SPI_FLASH_PORT);

    // send Command: 0xC7, Chip Erase
    SPI_WRITE_TX(SPI_FLASH_PORT, 0xC7);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);

    // Clear Rx FIFO buffer
    SPI_ClearRxFIFO(SPI0);
}

/**
 * @brief       Read SPI flash status register
 * @param       u8Value the is spi flash status value.
 * @return      None
 * @details     This function read SPI flash status register
 */
uint8_t SpiFlash_ReadStatusReg(void)
{
    // /CS: active
    SPI_SET_SS0_LOW(SPI_FLASH_PORT);

    // send Command: 0x05, Read status register
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x05);

    // read status
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);

    // skip first rx data
    SPI_READ_RX(SPI_FLASH_PORT);

    // Return spi receiving data
    return (SPI_READ_RX(SPI_FLASH_PORT) & 0xff);
}

/**
 * @brief       Write SPI flash status register
 * @param       u8Value the is spi flash status value.
 * @return      None
 * @details     This function write SPI flash status register
 */
void SpiFlash_WriteStatusReg(uint8_t u8Value)
{
    // /CS: active
    SPI_SET_SS0_LOW(SPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);

    ///////////////////////////////////////

    // /CS: active
    SPI_SET_SS0_LOW(SPI_FLASH_PORT);

    // send Command: 0x01, Write status register
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x01);

    // write status
    SPI_WRITE_TX(SPI_FLASH_PORT, u8Value);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);
}


/**
 * @brief       Wait until SPI flash is finished sending/receiving data
 * @param       None
 * @return      None
 * @details     This function wait until SPI flash is finished sending/receiving data
 */
void SpiFlash_WaitReady(void)
{
    uint8_t ReturnValue;

    do
    {
        //Read SPI flash status
        ReturnValue = SpiFlash_ReadStatusReg();
        ReturnValue = ReturnValue & 1;
    }
    while(ReturnValue!=0);   // check the BUSY bit
}

/**
 * @brief       Write data into SPI flash
 * @param[in]   StartAddress    The base address of SPI flash.
 * @param[out]  u8DataBuffer    Write u8DataBuffer[256] into SPI flash
 * @return      None
 * @details     This function write data into SPI flash
 */
void SpiFlash_NormalPageProgram(uint32_t StartAddress, uint8_t *u8DataBuffer)
{
    uint32_t i = 0;

    // /CS: active
    SPI_SET_SS0_LOW(SPI_FLASH_PORT);

    // send Command: 0x06, Write enable
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x06);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);


    // /CS: active
    SPI_SET_SS0_LOW(SPI_FLASH_PORT);

    // send Command: 0x02, Page program
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x02);

    // send 24-bit start address
    // Bits 23 to 16
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>16) & 0xFF);
    // Bits 15 to 8
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>8)  & 0xFF);
    // Bits 7 to 0
    SPI_WRITE_TX(SPI_FLASH_PORT, StartAddress       & 0xFF);


    while(1)
    {
        //Check SPI Tx FIFO full
        if(!SPI_GET_TX_FIFO_FULL_FLAG(SPI_FLASH_PORT))
        {
            // write data
            SPI_WRITE_TX(SPI_FLASH_PORT, u8DataBuffer[i++]);
            if(i >= 255) break;
        }
    }

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);

    // Clear rx FIFO buffer
    SPI_ClearRxFIFO(SPI_FLASH_PORT);
}

/**
 * @brief       Read SPI flash data
 * @param[in]   StartAddress    The base address of SPI flash.
 * @param[out]  u8DataBuffer    Read SPI flash into u8DataBuffer[256]
 * @return      None
 * @details     This function read data into buffer
 */
void SpiFlash_NormalRead(uint32_t StartAddress, uint8_t *u8DataBuffer)
{
    uint32_t i;

    // /CS: active
    SPI_SET_SS0_LOW(SPI_FLASH_PORT);

    // send Command: 0x03, Read data
    SPI_WRITE_TX(SPI_FLASH_PORT, 0x03);

    // send 24-bit start address
    // Bits 23 to 16
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>16) & 0xFF);

    // Bits 15 to 8
    SPI_WRITE_TX(SPI_FLASH_PORT, (StartAddress>>8)  & 0xFF);

    // Bits 7 to 0
    SPI_WRITE_TX(SPI_FLASH_PORT, StartAddress       & 0xFF);

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));
    // clear RX buffer
    SPI_ClearRxFIFO(SPI_FLASH_PORT);

    // read data
    for(i=0; i<256; i++)
    {

        //send 8-bit '0x00' , dummy
        SPI_WRITE_TX(SPI_FLASH_PORT, 0x00);

        //wait tx finish
        while(SPI_IS_BUSY(SPI_FLASH_PORT));

        //recieve 8-bit data
        u8DataBuffer[i] = SPI_READ_RX(SPI_FLASH_PORT);
    }

    // wait tx finish
    while(SPI_IS_BUSY(SPI_FLASH_PORT));

    // /CS: de-active
    SPI_SET_SS0_HIGH(SPI_FLASH_PORT);
}

/**
 * @brief       Initialize system
 * @param[in]   None
 * @return      None
 * @details     This function set clock and I/O Multi-function
 */
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Set HCLK source form HXT and HCLK source divide 1  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_HCLK_CLK_DIVIDER(1));

    /* Enable external 12MHz HXT, 32KHz LXT and HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC0EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_HIRC0STB_Msk);

    /*  Set HCLK frequency 36MHz */
    CLK_SetCoreClock(36000000);

    /* Select UART0 clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_UART0_CLK_DIVIDER(1));

    /* Select SPI0 clock source */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL1_SPI0SEL_HCLK, 0);

    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable SPI0 clock */
    CLK_EnableModuleClock(SPI0_MODULE);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* First, Clear PB0 and PB1 multi-function pins to '0', and then    */
    /* Set PB0 and PB1 multi-function pins to UART0 RXD and TXD         */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD);

    /* First, Clear PC0, PC1, PC2 and PC3 multi-function pins to '0', and then        */
    /* Set PC0, PC1, PC2 and PC3 multi function pin to SPI0 MISO0, MOSI0, CLK and SS0 */
    SYS->GPC_MFPL &= ~(SYS_GPC_MFPL_PC0MFP_Msk | SYS_GPC_MFPL_PC1MFP_Msk | SYS_GPC_MFPL_PC2MFP_Msk | SYS_GPC_MFPL_PC3MFP_Msk);
    SYS->GPC_MFPL |= (SYS_GPC_MFPL_PC0MFP_SPI0_SS0 | SYS_GPC_MFPL_PC1MFP_SPI0_CLK | SYS_GPC_MFPL_PC2MFP_SPI0_MISO0 | SYS_GPC_MFPL_PC3MFP_SPI0_MOSI0);

    /* First, Clear PD14 and PD15 multi function pin to '0', and then  */
    /* Set PD14 and PD15 multi function pin to SPI0 MISO1, MOSI1       */
    SYS->GPD_MFPH &= ~(SYS_GPD_MFPH_PD14MFP_Msk | SYS_GPD_MFPH_PD15MFP_Msk);
    SYS->GPD_MFPH |= (SYS_GPD_MFPH_PD14MFP_SPI0_MOSI1 | SYS_GPD_MFPH_PD15MFP_SPI0_MISO1);

    /* Lock protected registers */
    SYS_LockReg();
}

/**
 * @brief       Main function
 * @param[in]   None
 * @return      None
 * @details     This function is main function
 */
int main(void)
{
    /* Initialize variables */
    uint32_t u32ByteCount, u32FlashAddress, u32PageNumber,nError = 0;
    uint16_t u16ID;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /* Configure SPI_FLASH_PORT as a master, MSB first, 8-bit transaction, SPI Mode-0 timing, clock is 2MHz */
    SPI_Open(SPI_FLASH_PORT, SPI_MASTER, SPI_MODE_0, 8, 2000000);

    /* Enable the automatic hardware slave select function. Select the SS0 pin and configure as low-active. */
    SPI_EnableAutoSS(SPI_FLASH_PORT, SPI_SS0, SPI_SS0_ACTIVE_LOW);

    /* Enable FIFO mode and set FIFO TX/RX threshold */
    SPI_EnableFIFO(SPI_FLASH_PORT, 4, 4);

    /* Show massage to UART */
    printf("\n+------------------------------------------------------------------------+\n"
           "|               Nano103 Series SPI Flash with FIFO mode Sample           |\n"
           "+------------------------------------------------------------------------+\n");
    /*In this sample, Write/Read data from SPI flash, and then compare data */

    /* Wait ready */
    SpiFlash_WaitReady();

    if((u16ID = SpiFlash_ReadMidDid()) != 0xEF14)
    {
        printf("Wrong ID, 0x%x\n", u16ID);
        while(1);
    }
    else
        printf("Flash found: W25X16 ...\n");

    printf("Erase chip ...");

    /* Erase SPI flash */
    SpiFlash_ChipErase();

    /* Wait ready */
    SpiFlash_WaitReady();

    printf("[OK]\n");

    /* init source data buffer */
    for(u32ByteCount=0; u32ByteCount<TEST_LENGTH; u32ByteCount++)
    {
        SrcArray[u32ByteCount] = u32ByteCount;
    }

    /* Start to write data to SPI flash */
    printf("Start to write data to Flash ...");

    /* Program SPI flash */
    u32FlashAddress = 0;
    for(u32PageNumber=0; u32PageNumber<TEST_NUMBER; u32PageNumber++)
    {
        /* page program */
        SpiFlash_NormalPageProgram(u32FlashAddress, SrcArray);

        /* waiting for spi write flah to finish */
        SpiFlash_WaitReady();

        u32FlashAddress += 0x100;
    }

    /* Show [OK] on UART */
    printf("[OK]\n");

    /* clear destination data buffer */
    for(u32ByteCount=0; u32ByteCount<TEST_LENGTH; u32ByteCount++)
    {
        DestArray[u32ByteCount] = 0;
    }

    /* Start to read data to SPI flash, and then compare data */
    printf("Read & Compare ...");

    /* Read SPI flash */
    u32FlashAddress = 0;
    for(u32PageNumber=0; u32PageNumber<TEST_NUMBER; u32PageNumber++)
    {
        /* page read */
        SpiFlash_NormalRead(u32FlashAddress, DestArray);
        u32FlashAddress += 0x100;

        /* Check Rx data */
        for(u32ByteCount=0; u32ByteCount<TEST_LENGTH; u32ByteCount++)
        {
            if(DestArray[u32ByteCount] != SrcArray[u32ByteCount])
                nError ++;
        }
    }

    /* Check whether compare data has an error */
    /* if compare data is correct, show [OK]   */
    /* if compare data is error, show [FAILE]  */
    if(nError == 0)
        printf("[OK]\n");
    else
        printf("[FAIL]\n");

    /* Infinite loop */
    while(1);
}


/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/


