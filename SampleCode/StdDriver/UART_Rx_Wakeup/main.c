/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/12/29 3:07p $
 * @brief    Demonstrate how to wake up system form Power-down mode by UART interrupt.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "Nano103.h"
#include "sys.h"


/* Global variables */
__IO int32_t   _Wakeup_Flag = 0;    /* 1 indicates system wake up from power down mode */

/**
  * @brief  PDWU Handle function
  * @param  None.
  * @return None.
  */
void PDWU_IRQHandler()
{
    printf("PDWU_IRQHandler running...\n");
    CLK->WKINTSTS = 1; /* clear interrupt status */
    _Wakeup_Flag = 1;
}

/**
  * @brief  GPABC Wake Up Handle function
  * @param  None.
  * @return None.
  */
void GPABC_IRQHandler(void)
{
    /* To check if PB.0 interrupt occurred */
    if (PB->INTSRC & BIT0)
    {
        PB->INTSRC = BIT0;
        printf("PB.0 INT occurred. \n");

    }
}

/**
  * @brief  UART Wake Up Handle function.
  * @param  None.
  * @return None.
  */
void UART0_IRQHandler(void)
{
    uint32_t u32IntStatus;

    u32IntStatus = UART0->INTSTS;

    /* Wake Up */
    if (u32IntStatus & UART_INTSTS_WKUPIF_Msk)
    {
        printf("UART_Wakeup. \n");
        UART0->INTSTS = UART_INTSTS_WKUPIF_Msk; //clear status

        if(UART0->WKUPSTS & UART_WKUPSTS_DATWKSTS_Msk)
            UART0->WKUPSTS = UART_WKUPSTS_DATWKSTS_Msk; //clear status
    }

    while(!(UART0->FIFOSTS & UART_FIFOSTS_TXENDF_Msk)) ;  /* waits for message send out */

}

/**
  * @brief  Save multi-function pin setting and then go to power down.
  * @param  None.
  * @return None.
  */
void Enter_PowerDown()
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable UART wake up interrupt */
    UART_EnableInt(UART0, UART_INTEN_WKUPIEN_Msk);
    /* Enable UART Data Wake up function */
    UART0->WKUPEN |= UART_WKUPEN_WKDATEN_Msk;

    NVIC_EnableIRQ(UART0_IRQn);

#ifdef ENABLE_GPIO_WAKEUP
    NVIC_EnableIRQ(GPABC_IRQn);
#endif

    /* Enable system wake up interrupt */
    CLK->PWRCTL |= CLK_PWRCTL_WAKEINT_EN;
    NVIC_EnableIRQ(PDWU_IRQn);

    /* Enter Power Down mode */
    CLK_PowerDown();
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

    /* Select IP clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART0SEL_Msk;
    CLK->CLKSEL1 |= (0x0 << CLK_CLKSEL1_UART0SEL_Pos);// Clock source from external 12 MHz crystal clock

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD  */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk|SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |=  (SYS_GPB_MFPL_PB0MFP_UART0_RXD|SYS_GPB_MFPL_PB1MFP_UART0_TXD);

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
  * @brief  Main routine.
  * @param  None.
  * @return None.
  */
int32_t main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();

    /*---------------------------------------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);

    printf("+-----------------------------------------------------------+\n");
    printf("|  UART Rx WAkeup Function Test                             |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("|  Description :                                            |\n");
    printf("|    The sample code will demo UART0 Rx(PB.0) wakeup from   |\n");
    printf("|    power down mode.                                       |\n");
    printf("+-----------------------------------------------------------+\n");
    printf("| Please input any data to uart0 Rx pin to wakeup system.   |\n");
    printf("+-----------------------------------------------------------+\n");

#ifdef ENABLE_GPIO_WAKEUP
    /* Enable GPIO interrupt */
    GPIO_EnableInt(PB, 0, GPIO_INT_BOTH_EDGE);
    PB->INTSRC = BIT0;
#endif

    printf("Going to Power Down...\n\n");

    while(!(UART0->FIFOSTS & UART_FIFOSTS_TXENDF_Msk)) ;  /* waits for message send out */

    /* Enter power down mode */
    Enter_PowerDown();

    if (_Wakeup_Flag == 1)
    {
        _Wakeup_Flag = 0;

        printf("\n Wakeup OK!!");

        /* wait print message finish */
        CLK_SysTickDelay(335000);
    }

    printf("\n Wakeup demo end.");

    /* Infinite loop */
    while(1);
}





/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/





