/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 5 $
 * $Date: 15/12/28 4:56p $
 * @brief    Demonstrate the usage of PDMA transfer. One SPI interface
 *           is enabled in loopback mode. Two PDMA channels are used
 *           in this sample, one for transmit, the other for receive.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "Nano103.h"

/* Define test pattern count */
#define PDMA_TEST_COUNT 16

/*  Initialize global variables */
uint32_t g_au32SrcData[PDMA_TEST_COUNT]; //Source data buffer
uint32_t g_au32DstData[PDMA_TEST_COUNT]; //Destination data buffer
uint32_t volatile u32IsTestOver = 0;  //Check whether PDMA channel 2 tranfer is finished


/**
 * @brief       PDMA interrupt handler
 * @param[in]   None
 * @return      None
 * @details     This function services PDMA interrupts. 
 */
void PDMA_IRQHandler(void)
{
    uint32_t status = PDMA_GET_INT_STATUS();

    if (status & 0x2) { /* done */
        PDMA_CLR_CH_INT_FLAG(1, PDMA_CH_INTSTSn_TDIF_Msk); // Clear PDMA Channel 1 Interrupt Flag 
    } else if (status & 0x4) { /* done */
        if (PDMA_GET_CH_INT_STS(2) & 0x2) // Get PDMA Channel 2 Interrupt Status 
            u32IsTestOver = 1;
        PDMA_CLR_CH_INT_FLAG(2, PDMA_CH_INTSTSn_TDIF_Msk); //Clear PDMA Channel2 Interrupt Flag
    } else
        printf("unknown interrupt, status=0x%x !!\n", status); //Unknown status
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

    /*  Set HCLK frequency 32MHz */
    CLK_SetCoreClock(32000000);

    /* Select UART0 clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_UART0_CLK_DIVIDER(1));
  
    /* Select SPI0 clock source */
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL1_SPI0SEL_HCLK, 0);

    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);
    
    /* Enable SPI0 clock */
    CLK_EnableModuleClock(SPI0_MODULE);

    /* Enable PDMA clock */
    CLK_EnableModuleClock(PDMA_MODULE);
    
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
    /*  Initialize variables */
    uint32_t u32Err=0,i;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /* Configure SPI0 as a master, MSB first, 32-bit transaction, SPI Mode-0 timing, clock is 2MHz */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 32, 2000000);

    /* Enable the automatic hardware slave select function. Select the SPI0_SS0 pin and configure as low-active. */
    SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS0_ACTIVE_LOW);

    /* The I/O connection for SPI1 loopbak:            */
    /*      SPI_MISO0(PC.2) <---> SPI0_MOSI0(PC.3)     */
    /* Please connect SPI0 MISO0 and MOSI0 pin         */
    printf("Please connect SPI0 MISO and MOSI pin, and press any key to start transmission ...");
    getchar();
    printf("\n");

    /* Initailize g_au32SrcData buffer */
    for(i=0; i<PDMA_TEST_COUNT; i++)
        g_au32SrcData[i] = 0x55550000 + i;

    /* Open Channel 1 for SPI0 TX, channel 2 for SPI0 RX */
    PDMA_Open(3 << 1);

    /* Configure channel 1 */
    PDMA_SetTransferCnt(1, PDMA_WIDTH_32, PDMA_TEST_COUNT);
    
    /* Set channel 1 Transfer Address */
    PDMA_SetTransferAddr(1, (uint32_t)g_au32SrcData, PDMA_SAR_INC, (uint32_t)&SPI0->TX0, PDMA_DAR_FIX);
    
    /* Set channel 1 time-out */
    PDMA_SetTimeOut(1, 0, 0x5555);
    
    /* Enable channel 1 interrupt */
    PDMA_EnableInt(1, PDMA_CH_INTSTSn_TDIF_Msk);

    /* Configure channel 2 */
    PDMA_SetTransferCnt(2, PDMA_WIDTH_32, PDMA_TEST_COUNT);
    
    /* Set channel 2 Transfer Address */
    PDMA_SetTransferAddr(2, (uint32_t)&SPI0->RX0, PDMA_SAR_FIX, (uint32_t)g_au32DstData, PDMA_DAR_INC);
    
    /* Set channel 2 time-out */
    PDMA_SetTimeOut(2, 0, 0x5555);
    
    /* Enable channel 2 interrupt */
    PDMA_EnableInt(2, PDMA_CH_INTENn_TDIEN_Msk);

    /* Set Channel 1 for SPI0 TX, then start timeout counting */
    PDMA_SetTransferMode(1, PDMA_SPI0_TX, 0, 0);
    
    /* Set Channel 2 for SPI0 RX, then start timeout counting */
    PDMA_SetTransferMode(2, PDMA_SPI0_RX, 0, 0);

    /* Trigger PDMA ch1 */
    PDMA_Trigger(1);
    
    /* Trigger PDMA ch2 */
    PDMA_Trigger(2);

    /* Enable PDMA IRQ */
    NVIC_EnableIRQ(PDMA_IRQn);

    /* Trigger SPI0 Rx PDMA */
    SPI_TRIGGER_RX_PDMA(SPI0);
    
    /* Trigger SPI0 Tx PDMA */
    SPI_TRIGGER_TX_PDMA(SPI0);

    /* Wait for PDMA operation finish */
    while(u32IsTestOver == 0);

    /* Check PDMA status */
    if(u32IsTestOver != 1)
        printf("PDMA error !\n");

    /* Check Rx Data */
    for(i=0; i<PDMA_TEST_COUNT; i++) {
        if(g_au32SrcData[i] != g_au32DstData[i]) {
            u32Err ++;
        }
    }

    /* Check whether compare data has an error */
    /* if compare data is correct, show [OK]   */
    /* if compare data is error, show [FAILE]  */
    if(u32Err)
        printf(" [FAIL]\n\n");
    else
        printf(" [PASS]\n\n");

    /* Infinite loop */
    while(1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
