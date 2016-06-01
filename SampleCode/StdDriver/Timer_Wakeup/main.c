/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 5 $
 * $Date: 15/12/29 2:52p $
 * @brief    Use timer to wake up system from Power-down mode periodically.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano103.h"

void TMR0_IRQHandler(void)
{    
    TIMER_ClearWakeupFlag(TIMER0); /* Clear wakeup interrupt flag */   
    TIMER_ClearIntFlag(TIMER0); /* Clear time-out interrupt flag */
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS_UnlockReg(); /* Unlock protected registers */

    /* Enable External LXT (32 kHz) */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk;

    /* Waiting for LXT clock ready */
    CLK_WaitClockReady(CLK_STATUS_LXTSTB_Msk);
      
      /* Enable TIMER0 and UART0 clock */
    CLK->APBCLK |= CLK_APBCLK_TMR0CKEN_Msk | CLK_APBCLK_UART0CKEN_Msk;

    /* Select Timer0 clock source from LXT */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_TMR0SEL_Msk) | CLK_CLKSEL1_TMR0SEL_LXT;
      /* Select UART0 clock source from HXT */
        CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART0SEL_Msk) | CLK_CLKSEL1_UART0SEL_HXT;
    
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();
    
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD);
    
    SYS_LockReg(); /* Lock protected registers */
}

int main(void)
{
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();
   
    UART_Open(UART0, 115200); /* Init UART to 115200-8n1 for print message */

    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1); /* Initial Timer0 to periodic mode with 1Hz */
    TIMER_EnableWakeup(TIMER0); /* Enable timer wake up system */    
    TIMER_EnableInt(TIMER0);    /* Enable Timer0 interrupt */	  
    NVIC_EnableIRQ(TMR0_IRQn);  /* Enable Timer0 IRQ */
    
    TIMER_Start(TIMER0); /* Start Timer0 counting */
    
    SYS_UnlockReg(); /* Unlock protected registers */

    /* Timer0 Wake up Test */    
        while(1)
        {            
            CLK_PowerDown(); /* Let system enter to Power-down mode */              
            printf("System has been waken-up.\n"); /* System wake-up from Power-down mode */
            while(!UART_IS_TX_EMPTY(UART0));
    }        
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
