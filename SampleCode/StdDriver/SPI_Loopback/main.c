/**************************************************************************//**
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/12/28 4:55p $
 * @brief    Use two SPI interfaces, one is master and another is slave, transfer
 *           data to each other.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "Nano103.h"

/* Define test pattern count */
#define TEST_COUNT 64

uint32_t g_au32SourceData[TEST_COUNT]; /* Initialize source buffer */
uint32_t g_au32DestinationData0[TEST_COUNT]; /* Initialize destination data0 buffer */
uint32_t g_au32DestinationData1[TEST_COUNT]; /* Initialize destination data1 buffer */
volatile uint32_t SPI1_INT_Flag; //SPI1 interrupt flag

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
    
    /* Select SPI1 clock source */
    CLK_SetModuleClock(SPI1_MODULE, CLK_CLKSEL2_SPI1SEL_HCLK, 0);

    /* Enable UART0 clock */
    CLK_EnableModuleClock(UART0_MODULE);
    
    /* Enable SPI0 clock */
    CLK_EnableModuleClock(SPI0_MODULE);
    
    /* Enable SPI1 clock */
    CLK_EnableModuleClock(SPI1_MODULE);

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

    /* First, Clear PC11, PC10, PC9 and PC8 multi function pin to '0', and then      */
    /* Set PC11, PC10, PC9 and PC8 multi function pin to SPI1 MOSI0, MISO0, CLK, SS0 */
    SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC11MFP_Msk | SYS_GPC_MFPH_PC10MFP_Msk | SYS_GPC_MFPH_PC9MFP_Msk | SYS_GPC_MFPH_PC8MFP_Msk);
    SYS->GPC_MFPH |= (SYS_GPC_MFPH_PC11MFP_SPI1_MOSI0 | SYS_GPC_MFPH_PC10MFP_SPI1_MISO0 | SYS_GPC_MFPH_PC9MFP_SPI1_CLK | SYS_GPC_MFPH_PC8MFP_SPI1_SS0);
    
    /* First Clear PD6 and PD7 multi function pin to '0', and then */
    /* Set PD6 and PD7 multi function pin to SPI1 MOSI1, MISO1     */
    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD6MFP_Msk | SYS_GPD_MFPL_PD7MFP_Msk);
    SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD6MFP_SPI1_MOSI1 | SYS_GPD_MFPL_PD7MFP_SPI1_MISO1);

    /* Lock protected registers */
    SYS_LockReg();
}


/**
 * @brief       SPI1 interrupt handler
 * @param[in]   None
 * @return      None
 * @details     This function services SPI1 interrupts. 
 */
void SPI1_IRQHandler(void)
{
    /* Check the unit transfer interrupt flag */
    if( SPI_GET_STATUS(SPI1) & SPI_STATUS_UNITIF_Msk ) {
        /* write '1' to clear SPI1 uint transfer interrupt flag */
        SPI_CLR_UNIT_TRANS_INT_FLAG(SPI1);   
        
        /* Set SPI interrupt flag to 1 */
        SPI1_INT_Flag = 1;
    }
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
    uint32_t u32DataCount, u32TestCount, u32Err;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    /* Configure SPI0 as a master, MSB first, 32-bit transaction, SPI Mode-0 timing, clock is 2MHz */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 32, 2000000);

    /* Enable the automatic hardware slave select function. Select the SPI0_SS0 pin and configure as low-active. */
    SPI_EnableAutoSS(SPI0, SPI_SS0, SPI_SS0_ACTIVE_LOW);

    /* Enable SPI0 two bit mode */
    SPI_ENABLE_2BIT_MODE(SPI0);

    /* Configure SPI1 as a slave, MSB first, 32-bit transaction, SPI Mode-0 timing, clock is 4Mhz */
    SPI_Open(SPI1, SPI_SLAVE, SPI_MODE_0, 32, 4000000);

    /* Configure SPI1 as a low level active device. */
    SPI_SET_SS0_LOW(SPI1);

    /* Enable SPI1 two bit mode */
    SPI_ENABLE_2BIT_MODE(SPI1);

    /* Show massage to UART */
    printf("\n+----------------------------------------------------------------------+\n"
           "|              Nano103 Series SPI Driver Sample Code                   |\n"
           "+----------------------------------------------------------------------+\n"
           "Configure SPI0 as a master and SPI1 as a slave.\n"
           "Please connect SPI0 with SPI1, and press any key to start transmission ...");

/* Set PB0, PB1, PB2 and PB3 multi function pin to SPI1 MOSI0, MISO0, CLK, SS0 */
/* Set PD6 and PD7 multi function pin to SPI1 MOSI1, MISO1     */

    /* Configure SPI0 as a master and SPI1 as a slave   */
    /* The I/O connection for SPI0 loopback :           */
    /*        SPI0_MISO0(PC2)  <--> SPI1_MISO0(PC10)               */
    /*        SPI0_MOSI0(PC3)  <--> SPI1_MOS10(PC11)               */
    /*        SPI0_MISO1(PD14) <--> SPI1_MISO1(PD7)                */
    /*        SPI0_MOSI1(PD15) <--> SPI1_MOS11(PD6)                */
    /*        SPI0_CLK(PC1)    <--> SPI1_CLK(PC9)                  */
    /*        SPI0_SS0(PC0)    <--> SPI1_SS0(PC8)                  */
    getchar();

    /* Show massage to UART */
    printf("\nSPI0/1 Loopback test ");

    /* Enable the SPI1 unit transfer interrupt. */
    SPI_EnableInt(SPI1, SPI_IE_MASK);
    NVIC_EnableIRQ(SPI1_IRQn);
    
    /* Trigger SPI1 transfer */
    SPI_TRIGGER(SPI1);

    u32Err = 0; 
    for(u32TestCount=0; u32TestCount<10000; u32TestCount++) {
        /* set the source data and clear the destination buffer */
        for(u32DataCount=0; u32DataCount<TEST_COUNT; u32DataCount++) {
            g_au32SourceData[u32DataCount] = u32DataCount;  //set sourc data to 1,2,3,4,5...
            g_au32DestinationData0[u32DataCount] = 0;  //Clear destinationData0 buffer to '0'
            g_au32DestinationData1[u32DataCount] = 0;  //Clear destinationData1 buffer to '0'
        }

        u32DataCount=0;
        SPI1_INT_Flag = 0;  //Clear SPI1 interrupt flag to '0'

        if((u32TestCount&0x1FF) == 0) {
            putchar('.');
        }

        /* write the first data of source buffer to TX0 register of SPI0. And start transmission. */
        SPI_WRITE_TX0(SPI0, g_au32SourceData[0]);
        
        /* write the first data of source buffer to TX1 register of SPI0. And start transmission. */
        SPI_WRITE_TX1(SPI0, g_au32SourceData[0]);
        
        /* Trigger SPI0 transfer */
        SPI_TRIGGER(SPI0);

        while(1) {
            if(SPI1_INT_Flag==1) {
                /* Clear SPI1 interrupt flag to '0' */
                SPI1_INT_Flag = 0;

                if(u32DataCount<(TEST_COUNT-1)) {
                    /* Read the previous retrieved data from RX0 register of SPI1 and trigger next transfer. */
                    g_au32DestinationData0[u32DataCount] = SPI_READ_RX0(SPI1);

                    /* Read the previous retrieved data from RX1 register of SPI1 and trigger next transfer. */
                    g_au32DestinationData1[u32DataCount] = SPI_READ_RX1(SPI1);
                    u32DataCount++;

                    /* Trigger SPI1 transfer */
                    SPI_TRIGGER(SPI1);

                    /* Write data to SPI0 TX0 buffer and trigger the transfer */
                    SPI_WRITE_TX0(SPI0, g_au32SourceData[u32DataCount]);
                  
                    /* Write data to SPI0 TX1 buffer and trigger the transfer */
                    SPI_WRITE_TX1(SPI0, g_au32SourceData[u32DataCount]);
                  
                    /* Trigger SPI0 transfer */
                    SPI_TRIGGER(SPI0);
                } else {
                    /* Just read the previous retrieved data but trigger next transfer, because this is the last transfer. */
                    /* Read SPI RX0 data to g_au32DestinationData0 buffer */
                    g_au32DestinationData0[u32DataCount] = SPI_READ_RX0(SPI1);
                  
                    /* Read SPI RX1 data to g_au32DestinationData1 buffer */
                    g_au32DestinationData1[u32DataCount] = SPI_READ_RX1(SPI1);
                  
                    /* Trigger SPI1 transfer */
                    SPI_TRIGGER(SPI1);
                    break;
                }
            }
        }

        /*  Check the received data */
        for(u32DataCount=0; u32DataCount<TEST_COUNT; u32DataCount++) {
            /* Compare data */
            if((g_au32DestinationData0[u32DataCount]!=g_au32SourceData[u32DataCount]) ||
                    (g_au32DestinationData1[u32DataCount]!=g_au32SourceData[u32DataCount]))
                u32Err = 1;
        }

        if(u32Err)
            break;
    }
    /* Disable the SPI1 unit transfer interrupt. */
    SPI_DisableInt(SPI1, SPI_IE_MASK);
    NVIC_DisableIRQ(SPI1_IRQn);

    if(u32Err)
        printf(" [FAIL]\n\n");
    else
        printf(" [PASS]\n\n");

    /* Infinite loop */
    while(1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/

