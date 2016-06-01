
/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 6 $
 * $Date: 16/02/17 4:55p $
 * @brief    Use the timer pin PC.6 to demonstrate timer free counting mode
 *           function. Also display the measured input frequency to UART console.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano103.h"

void TMR0_IRQHandler(void)
{
    /* printf takes long time and affect the freq. calculation, we only print out once a while */
    static int cnt = 0;
    static uint32_t t0, t1;

    if(cnt == 0) {
			  /* Gets the Timer capture data */
        t0 = TIMER_GetCaptureData(TIMER0);
        cnt++;
    } else if(cnt == 1) {
			  /* Gets the Timer capture data */
        t1 = TIMER_GetCaptureData(TIMER0);
        cnt++;
        if(t0 > t1) {
            /* over run, drop this data and do nothing */
        } else {
					  /* Display the measured input frequency */
            printf("Input frequency is %dHz\n", 12000000 / (t1 - t0));
        }
    } else {
        cnt = 0;
    }

    /* Clears the Timer capture interrupt flag */
    TIMER_ClearCaptureIntFlag(TIMER0);
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable External XTAL (4~24 MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to HXT */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_HCLK_CLK_DIVIDER(1));

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(TMR0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_UART0_CLK_DIVIDER(1));
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HXT, CLK_TMR0_CLK_DIVIDER(1));

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB1MFP_UART0_TXD | SYS_GPB_MFPL_PB0MFP_UART0_RXD);

    /* Set GPC multi-function pins for Timer0 external capture input */
    SYS->GPC_MFPL  = (SYS->GPC_MFPL & ~SYS_GPC_MFPL_PC6MFP_Msk) | SYS_GPC_MFPL_PC6MFP_TM0_EXT;
    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    int volatile i;

    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    printf("\nThis sample code demonstrate timer free counting mode.\n");
    printf("Please connect input source with Timer 0 capture pin PC.6, press any key to continue\n");
    getchar();

    /* Give a dummy target frequency here. Will over write capture resolution with macro */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000000);

    /* Update prescale to set proper resolution. */
    TIMER_SET_PRESCALE_VALUE(TIMER0, 0);

    /* Set compare value as large as possible, so don't need to worry about counter overrun too frequently. */
    TIMER_SET_CMP_VALUE(TIMER0, 0xFFFFFF);

    /* Configure Timer 0 free counting mode, capture TDR value on rising edge */
    TIMER_EnableCapture(TIMER0, TIMER_CAPTURE_FREE_COUNTING_MODE, TIMER_CAPTURE_RISING_EDGE);

    /* Start Timer 0 */
    TIMER_Start(TIMER0);

    /* Enable timer interrupt */
    TIMER_EnableCaptureInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);

    while(1);

}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/


