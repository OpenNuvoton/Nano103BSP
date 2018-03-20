/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/12/29 3:03p $
 * @brief    Demonstrate how to use auto baud rate detection function.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano103.h"
#include "uart.h"
#include "sys.h"
#include "clk.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---Using in UART Test -------------------*/
volatile int32_t g_bWait    = TRUE;

/*---------------------------------------------------------------------------------------------------------*/
/* Define functions prototype                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
void UART_INT_HANDLE(uint32_t u32IntStatus);
extern char GetChar(void);

/**
  * @brief  Get UART1 clock.
  * @param  None.
  * @return clk  UART clock.
  */
uint32_t GetUartClk(void)
{
    uint32_t clk =0, div;

    div = ( (CLK->CLKDIV0 & CLK_CLKDIV0_UART1DIV_Msk) >> CLK_CLKDIV0_UART1DIV_Pos) + 1; /* Get UART clock divider */

    switch (CLK->CLKSEL2 & CLK_CLKSEL2_UART1SEL_Msk)   /* Get UART selected clock source */
    {
    case 0:
        clk = __HXT; /* HXT */
        break;
    case 1:
        clk = __LXT;  /* LXT */
        break;
    case 2:
        clk = SysGet_PLLClockFreq(); /* PLL */
        break;
    case 3:
        if(CLK->CLKSEL0 & CLK_CLKSEL0_HIRCSEL_Msk)
            clk = __HIRC36M; /* HIRC 36M Hz*/
        else
        {
            if(CLK->PWRCTL & CLK_PWRCTL_HIRC0FSEL_Msk)
                clk = __HIRC16M; /* HIRC 16M Hz*/
            else
                clk = __HIRC12M; /* HIRC 12M Hz*/
        }
        break;
    }

    clk /= div; /* calculate UART clock */

    return clk;
}

/**
  * @brief  UART Auto Baud Rate interrupt Handle function.
  * @param  None.
  * @return None.
  */
void UART_ABAUD_INT_HANDLE(void)
{
    uint32_t result;
    uint32_t clk;

    g_bWait = FALSE;

    if (UART1->TRSR & UART_TRSR_ABRDIF_Msk)   /* Auto baud-rate INT */
    {
        UART1->TRSR = UART_TRSR_ABRDIF_Msk;

        clk = GetUartClk(); /* Get UART clock */
        result = clk / (UART1->BAUD + 1); /* calculate UART baud-rate */
        printf("> BAUD Detect Complete, divider = %d [%xh]\n", UART1->BAUD, UART1->BAUD); /* print baud rate setting */
        printf("> BAUD Rate = %d\n", result); /* print real baud rate */
    }

    if (UART1->TRSR & UART_TRSR_ABRDTOIF_Msk)   /* Auto baud-rate flag */
    {
        UART1->TRSR = UART_TRSR_ABRDTOIF_Msk; /* clear auto baud-rate flag */
        printf("\nFailed -- Auto Baud Rate Time Out...");
    }

}

/**
 *  @brief ISR to handle UART Channel 1 interrupt event.
 *  @param[in] None
 *  @return None
 */
void UART1_IRQHandler(void)
{
    if(UART1->INTSTS & UART_INTSTS_ABRIF_Msk)
    {
        UART_ABAUD_INT_HANDLE();
    }
}

/**
 *  @brief  UART1 Auto Baud Rate Test function.
 *  @param  None
 *  @return None
 */
void UART_AutoBaudRateTest()
{
    printf("\nUART Auto Baud Rate Test. ");
    printf("\n(Press '1' to start test UART1 Auto Baud Rate.)\n");

    /* Set UART Configuration */
    UART_Open(UART1, 115200);

    UART1->CTRL |= UART_CTRL_ABRDEN_Msk; /* Enable UART1 Auto Baud Rate */
    UART1->CTRL &= ~UART_CTRL_ABRDBITS_Msk; /* Set Auto-baud Rate Detect Bit Length */

    /* Enable Interrupt and install the call back function */
    UART_EnableInt(UART1, UART_INTEN_ABRIEN_Msk);
    NVIC_EnableIRQ(UART1_IRQn);

    while(g_bWait);

    /* Disable Interrupt */
    UART_DisableInt(UART1, UART_INTEN_ABRIEN_Msk);

    g_bWait =TRUE;

    printf("\nUART Auto Baud Rate Test End.\n");

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
    //CLK->PWRCTL &= ~CLK_PWRCTL_HXT_EN_Msk;
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
    /* Init UART0 for printf */
    UART1_Init();

    printf("\n\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|               UART Sample Program                         |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| UART Auto Baud Rate test                                  |\n");
    printf("+-----------------------------------------------------------+\n");

    UART_AutoBaudRateTest();    /* Auto Baud Rate test */

    while(1);
}


/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/



