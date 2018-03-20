/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 3 $
 * $Date: 15/12/18 2:01p $
 * @brief    Demonstrate UART transmit and receive function with PDMA.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano103.h"

#include "uart.h"

#define TX_CH     1  // Tx select PDMA Channel 1 
#define RX_CH     2  // Rx select PDMA Channel 2

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t TX_Buffer[100];
uint8_t RX_Buffer[100];
uint32_t volatile u32IsTestOver = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void UART1_TEST_HANDLE(void);
void UART_PDMATest(void);

/**
 *  @brief  Init system clock and I/O multi function .
 *  @param  None
 *  @return None
 */
void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable External XTAL (4~24 MHz) */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk; // HXT Enabled

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to XTAL */
    CLK->CLKSEL0 = (CLK->CLKSEL0 &~ CLK_CLKSEL0_HCLKSEL_Msk) | (CLK_CLKSEL0_HCLKSEL_HXT);

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN; // UART0 Clock Enable
    CLK->APBCLK |= CLK_APBCLK_UART1_EN; // UART1 Clock Enable
    CLK->AHBCLK |= CLK_AHBCLK_DMA_EN;   // DMA Clock Enable

    /* Select IP clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART0SEL_Msk;
    CLK->CLKSEL1 |= (0x0 << CLK_CLKSEL1_UART0SEL_Pos);// Clock source from external 12 MHz crystal clock
    CLK->CLKSEL2 &= ~CLK_CLKSEL2_UART1SEL_Msk;
    CLK->CLKSEL2 |= (0x0 << CLK_CLKSEL2_UART1SEL_Pos);// Clock source from external 12 MHz crystal clock

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD  */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk|SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |=  (SYS_GPB_MFPL_PB0MFP_UART0_RXD|SYS_GPB_MFPL_PB1MFP_UART0_TXD);

    /* Set PB multi-function pins for UART1 RXD, TXD, RTS, CTS  */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk |
                       SYS_GPB_MFPL_PB6MFP_Msk | SYS_GPB_MFPL_PB7MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB4MFP_UART1_RXD | SYS_GPB_MFPL_PB5MFP_UART1_TXD |
                      SYS_GPB_MFPL_PB6MFP_UART1_nRTS  | SYS_GPB_MFPL_PB7MFP_UART1_nCTS);

    /* Lock protected registers */
    SYS_LockReg();
}

/**
 *  @brief  Config UART0.
 *  @param  None
 *  @return None
 */
void UART0_Init()
{
    /* set uart baudrate is 115200 */
    UART_Open(UART0, 115200);
}

/**
 *  @brief  Config UART1.
 *  @param  None
 *  @return None
 */
void UART1_Init()
{
    /* set UART baudrate is 57600 */
    UART_Open(UART1, 115200);
}

/**
 *  @brief  main function.
 *  @param  None
 *  @return None
 */
int main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init UART0 for printf */
    UART0_Init();

    /* Init UART1 for test */
    UART1_Init();

    /*---------------------------------------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);

    printf("+--------------------------+\n");
    printf("| UART PDMA function test  |\n");
    printf("+--------------------------+\n");

    UART_PDMATest();

    /* Infinite loop */
    while(1);
}

/**
 *  @brief  ISR to handle UART Channel 1 interrupt event.
 *  @param  None
 *  @return None
 */
void UART1_IRQHandler(void)
{
    UART1_TEST_HANDLE();
}

/**
 *  @brief  UART1 interrupt handle function.
 *  @param  None
 *  @return None
 */
void UART1_TEST_HANDLE()
{
    uint32_t u32IntSts= UART1->INTSTS;

    /* Check Receive line status */
    if(u32IntSts & UART_INTSTS_RLSIF_Msk)
    {
        printf("\n UART1 Receive Line error!!");
        UART1->FIFOSTS = (UART_FIFOSTS_PEF_Msk | UART_FIFOSTS_FEF_Msk | UART_FIFOSTS_BIF_Msk);
    }

    /*Check Buffer error */
    if(u32IntSts & UART_INTSTS_BUFERRIF_Msk)
    {
        printf("\n UART1 Buffer Overflow error");
        UART1->FIFOSTS = (UART_FIFOSTS_RXOVIF_Msk | UART_FIFOSTS_TXOVIF_Msk);
    }
}

/**
 *  @brief  ISR to handle PDMA interrupt event.
 *  @param  None
 *  @return None
 */
void PDMA_IRQHandler(void)
{
    uint32_t status = PDMA_GET_INT_STATUS();

    if (status & 0x2)   /* CH1 */
    {
        if (PDMA_GET_CH_INT_STS(1) & 0x2)
            u32IsTestOver = 1;
        PDMA_CLR_CH_INT_FLAG(1, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else if (status & 0x4)     /* CH2 */
    {
        if (PDMA_GET_CH_INT_STS(2) & 0x2)
            u32IsTestOver = 2;
        PDMA_CLR_CH_INT_FLAG(2, PDMA_CH_INTSTSn_TDIF_Msk);
    }
    else
        printf("unknown interrupt !!\n");
}

/**
 *  @brief  UART PDMA demo function.
 *  @param  None
 *  @return None
 */
void UART_PDMATest()
{
    uint32_t i;
    uint32_t TEST_SIZE = 100;

    printf("+-----------------------------------------------------------+\n");
    printf("|  UART PDMA Test                                           |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|  Description :                                            |\n");
    printf("|    The sample code will demo UART PDMA function.          |\n");
    printf("|    Please connect UART1 Tx, Rx pin.                       |\n");
    printf("+-----------------------------------------------------------+\n");

    /*
        connect UART1 Tx, Rx pin.
        UART1 will transfer data from Tx PDMA buffer.
        And will receive data to Rx PDMA buffer.
        This test function will compare Tx and Rx buffer data.
    */

    for(i = 0; i < TEST_SIZE; i++)
    {
        TX_Buffer[i] = (i & 0xff);
        RX_Buffer[i] = 0;
    }

    /* Enable Interrupt and config NVIC */
    UART_ENABLE_INT(UART1, (UART_INTSTS_RLSIF_Msk | UART_INTSTS_BUFERRIF_Msk) );
    NVIC_EnableIRQ(UART1_IRQn);

    // Rx PDMA Setting
    PDMA_Open(1 << RX_CH); // Enable PDMA channel 1
    PDMA_SetTransferCnt(RX_CH, PDMA_WIDTH_8, TEST_SIZE); // Set PDMA Transfer Count and Data width
    PDMA_SetTransferAddr(RX_CH, UART1_BASE, PDMA_SAR_FIX, (uint32_t)RX_Buffer, PDMA_DAR_INC); // Set PDMA Transfer Address
    PDMA_SetTransferMode(RX_CH, PDMA_UART1_RX, 0, 0); //Select PDMA selected peripheral is UART1_RX
    PDMA_SetTimeOut(RX_CH, 0, 0x5555); // set PDMA time-out
    PDMA_EnableInt(RX_CH, PDMA_CH_INTENn_TDIEN_Msk); // Enable PDMA transfer done interrupt
    NVIC_EnableIRQ(PDMA_IRQn); // Enable PDMA_IRQ Interrupt PDMA_IRQ
    u32IsTestOver = 0;

    // Tx PDMA Setting
    PDMA_Open(1 << TX_CH);
    PDMA_SetTransferCnt(TX_CH, PDMA_WIDTH_8, TEST_SIZE); // Set PDMA Transfer Count and Data width
    PDMA_SetTransferAddr(TX_CH, (uint32_t)TX_Buffer, PDMA_SAR_INC, UART1_BASE, PDMA_DAR_FIX); // Set PDMA Transfer Address
    PDMA_SetTransferMode(TX_CH, PDMA_UART1_TX, 0, 0); //Select PDMA selected peripheral is UART1_TX
    PDMA_SetTimeOut(TX_CH, 0, 0x5555); // set PDMA time-out
    PDMA_EnableInt(TX_CH, PDMA_CH_INTENn_TDIEN_Msk); // Enable PDMA transfer done interrupt
    NVIC_EnableIRQ(PDMA_IRQn); // Enable PDMA_IRQ Interrupt PDMA_IRQ
    u32IsTestOver = 0;

    PDMA_Trigger(RX_CH); // Trigger PDMA Rx channel
    PDMA_Trigger(TX_CH); // Trigger PDMA Tx channel

    UART1->CTRL |= UART_CTRL_RXDMAEN_Msk; // Enable UART Rx PDMA mode
    UART1->CTRL |= UART_CTRL_TXDMAEN_Msk; // Enable UART Tx PDMA mode

    while(u32IsTestOver != 2); // wait transfer finish

    /* Compare Data */
    for(i = 0; i < TEST_SIZE; i++)
    {
        if(TX_Buffer[i] != RX_Buffer[i])
        {
            printf("\n Test fail!!");
            while(1);
        }
    }

    printf("\n Tx/Rx data compare pass!!");

    /* Disable Interrupt */
    UART_DISABLE_INT(UART1, (UART_INTSTS_RLSIF_Msk | UART_INTSTS_BUFERRIF_Msk));
    NVIC_DisableIRQ(UART1_IRQn);

    printf("\n UART PDMASample Demo End.\n");

}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
