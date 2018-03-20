/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 3 $
 * $Date: 15/12/18 1:57p $
 * @brief    Show how to transmit and receive UART data in UART IrDA mode.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano103.h"
#include "uart.h"
#include "sys.h"
#include "clk.h"

/**
 *  @brief  The function is UART IrDA mode demo code.
 *  @param  None
 *  @return None
 */
void IRDA_FunctionTest()
{
    uint8_t bInChar[1]= {0xFF};

    printf("\n\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|               IRDA Function Test                          |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| 1). Input char by UART0 terminal.                         |\n");
    printf("| 2). UART1 will send a char according to step 1.           |\n");
    printf("| 3). Return step 1. (Press '0' to exit)                    |\n");
    printf("+-----------------------------------------------------------+\n\n");
    getchar();

    printf("\nIRDA Sample Demo. \n");

    /* Please call UART_Open() to configure UART before using UART_SelectIrDAMode() */
    UART_Open(UART1, 57600);

    /* Set IrDA mode */
    UART_SelectIrDAMode(UART1, 57600, 1); /* Select IrDA mode and set baudrate */

    do
    {
        bInChar[0] = getchar(); /* Get data from debug port */
        printf("   Input: %c , Send %c out\n",bInChar[0],bInChar[0]);
        UART_Write(UART1,bInChar,1); /* Send data */
    }
    while(bInChar[0] !='0');   /* wait user input '0' to exit test */

    printf("\nIrDA Sample Demo End.\n");

}

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

    CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk); /* Waiting for 12MHz clock ready */

    CLK->CLKSEL0 = (CLK->CLKSEL0 &~ CLK_CLKSEL0_HCLKSEL_Msk) | (CLK_CLKSEL0_HCLKSEL_HXT); /* Switch HCLK clock source to XTAL */

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
    UART_Open(UART0, 115200); /* set UART baudrate is 115200 */
}

/**
 *  @brief  Config UART1.
 *  @param  None
 *  @return None
 */
void UART1_Init()
{
    UART_Open(UART1, 57600); /* set UART baudrate is 57600 */
}

/**
 *  @brief  main function.
 *  @param  None
 *  @return None
 */
int32_t main()
{

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init UART0 for printf */
    UART0_Init();
    /* Init UART1 for printf */
    UART1_Init();

    printf("\n\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|               UART Sample Program                         |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| IrDA function test                                         |\n");
    printf("+-----------------------------------------------------------+\n");

    IRDA_FunctionTest();    /* IrDA Function Test */

    while(1);
}



/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/



