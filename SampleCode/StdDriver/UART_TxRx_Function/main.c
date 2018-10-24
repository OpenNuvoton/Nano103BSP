/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 3 $
 * $Date: 15/12/18 2:07p $
 * @brief    Transmit and receive data from PC terminal through RS232 interface.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano103.h"

#include "uart.h"

#define RXBUFSIZE 1024

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t g_u8SendData[12] = {0};
uint8_t g_u8RecData[RXBUFSIZE]  = {0};

volatile uint32_t g_u32comRbytes = 0;
volatile uint32_t g_u32comRhead  = 0;
volatile uint32_t g_u32comRtail  = 0;
volatile int32_t g_bWait         = TRUE;
volatile int32_t g_i32pointer = 0;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
int32_t main(void);
void UART1_TEST_HANDLE(void);
void UART_FunctionTest(void);

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
    /* set uart baudrate is 57600 */
    UART_Open(UART1, 57600);
}

/**
 *  @brief  main function.
 *  @param  None
 *  @return None
 */
int32_t main(void)
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

    printf("+---------------------+\n");
    printf("| UART function test  |\n");
    printf("+---------------------+\n");

    /*---------------------------------------------------------------------------------------------------------*/
    /* UART Test Sample                                                                                        */
    /* It sends the received data to HyperTerminal.                                                            */
    /*---------------------------------------------------------------------------------------------------------*/
    UART_FunctionTest();

    /* Infinite loop */
    while(1);
}

/**
 *  @brief ISR to handle UART Channel 1 interrupt event.
 *  @param[in] None
 *  @return None
 */
void UART1_IRQHandler(void)
{
    UART1_TEST_HANDLE();
}

/**
 *  @brief UART1 interrupt handle function.
 *  @param[in] None
 *  @return None
 */
void UART1_TEST_HANDLE()
{
    uint8_t u8InChar=0xFF;
    uint32_t u32IntSts= UART1->INTSTS;

    /* Check Receive Data */
    if((u32IntSts & UART_INTSTS_RDAIF_Msk) || (u32IntSts & UART_INTSTS_RXTOIF_Msk))
    {
        printf("\nInput:");

        /* Get all the input characters */
        while(UART_IS_RX_READY(UART1))
        {
            /* Get the character from UART Buffer */
            u8InChar = UART_READ(UART1);           /* Rx trigger level is 1 byte*/

            printf("%c ", u8InChar);

            if(u8InChar == '0')
            {
                g_bWait = FALSE;
            }

            /* Check if buffer full */
            if(g_u32comRbytes < RXBUFSIZE)
            {
                /* Enqueue the character */
                g_u8RecData[g_u32comRtail] = u8InChar;
                g_u32comRtail = (g_u32comRtail == (RXBUFSIZE-1)) ? 0 : (g_u32comRtail+1);
                g_u32comRbytes++;
            }
        }
        printf("\nTransmission Test:");
    }

    /* Check Tx empty */
    if(u32IntSts & UART_INTSTS_THREIF_Msk)
    {
        uint16_t tmp;
        tmp = g_u32comRtail;
        if(g_u32comRhead != tmp)
        {
            u8InChar = g_u8RecData[g_u32comRhead];
            /* print the received char on screen */
            UART_WRITE(UART1,u8InChar);
            g_u32comRhead = (g_u32comRhead == (RXBUFSIZE-1)) ? 0 : (g_u32comRhead+1);
            g_u32comRbytes--;
        }
    }
}

/**
 *  @brief The function is UART demo code.
 *  @param[in] None
 *  @return None
 */
void UART_FunctionTest()
{
    printf("+-----------------------------------------------------------+\n");
    printf("|  UART Function Test                                       |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|  Description :                                            |\n");
    printf("|    The sample code will print input char on terminal      |\n");
    printf("|    Please enter any to start     (Press '0' to exit)      |\n");
    printf("+-----------------------------------------------------------+\n");

    /*
        Using a RS232 cable to connect UART1 and PC.
        UART0 is set to debug port. UART1 is enable RDA and RLS interrupt.
        When inputting char to terminal screen, RDA interrupt will happen and
        UART1 will print the received char on screen.
    */

    /* Set RX Trigger Level = 8 */
    UART1->LINE = (UART1->LINE &~ UART_LINE_RFITL_Msk) | UART_LINE_RFITL_8BYTES;

    /* Set Timeout time 0x3E bit-time */
    UART_SetTimeoutCnt(UART1,0x3E);

    /* Enable Interrupt */
    UART_ENABLE_INT(UART1, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_RXTOIEN_Msk));
    NVIC_EnableIRQ(UART1_IRQn);
    while(g_bWait); // wait user press '0' to exit test

    /* Disable Interrupt */
    UART_DISABLE_INT(UART1, (UART_INTEN_RDAIEN_Msk | UART_INTEN_THREIEN_Msk | UART_INTEN_RXTOIEN_Msk));
    NVIC_DisableIRQ(UART1_IRQn);

    /* Reset RX Trigger Level */
    UART1->LINE &= ~UART_LINE_RFITL_Msk;

    g_bWait =TRUE;
    printf("\nUART Sample Demo End.\n");

}


