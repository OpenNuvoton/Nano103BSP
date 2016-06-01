/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 1 $
 * $Date: 16/02/24 10:51a $
 * @brief    Demonstrate the RTC snoop detrction function and display the result to the UART console.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano103.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

volatile uint32_t  g_u32SnoopDetection = 0;


/**
  * @brief  RTC ISR to handle interrupt event
  * @param  None
  * @retval None
  */
void RTC_IRQHandler(void)
{
	
    if (RTC->INTSTS & RTC_INTSTS_SNPDIF_Msk) {      /* snoop interrupt occurred */
        RTC->INTSTS = RTC_INTSTS_SNPDIF_Msk; /* clear interrupt status */

        printf("\n Snoop detection interrupt !! ");
		g_u32SnoopDetection = 1;
    }

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
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk; // XTAL12M (HXT) Enabled

    /* Enable External LXT (32 KHz) */
    CLK->PWRCTL |= CLK_PWRCTL_LXTEN_Msk;

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to XTAL */
	CLK->CLKSEL0 = (CLK->CLKSEL0 &~ CLK_CLKSEL0_HCLKSEL_Msk) | (CLK_CLKSEL0_HCLKSEL_HXT);

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_UART0CKEN_Msk; // UART0 Clock Enable
    CLK->APBCLK |= CLK_APBCLK_RTCCKEN_Msk; // RTC Clock Enable

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
    UART_Open(UART0, 115200); /* set uart baudrate is 115200 */
}

/**
 *  @brief  main function.
 *  @param  None
 *  @return None
 */
int32_t main(void)
{
    S_RTC_TIME_DATA_T sInitTime;

    SYS_Init();
    UART0_Init();

    /* Time Setting */
    sInitTime.u32Year       = 2015; // Set Year
    sInitTime.u32Month      = 10;   // Set Month
    sInitTime.u32Day        = 15;   // Set Day
    sInitTime.u32Hour       = 12;   // Set Hour
    sInitTime.u32Minute     = 30;   // Set Minute
    sInitTime.u32Second     = 0;    // Set Second
    sInitTime.u32DayOfWeek  = RTC_TUESDAY; // Set Day of Week
    sInitTime.u32TimeScale  = RTC_CLOCK_24; // Set Time scale 12 or 24 hr

    RTC_Open(&sInitTime); // start RTC and set initial time

    printf("\n RTC Snoop Detection Test \n\n");
	printf(" Please connect PB.13(snoop pin) to Low level(ex: GND) \n");
	printf(" Press any key to continue \n");
	getchar();

	/* Set PB.13 is snoop pin */
	SYS->GPB_MFPH &= ~SYS_GPB_MFPH_PB13MFP_Msk;
    SYS->GPB_MFPH |= (SYS_GPB_MFPH_PB13MFP_SNOOPER);
	
    /* Set snoop detection */
	RTC_EnableTamperDetection(0); // set rising detect 

	g_u32SnoopDetection = 0; // clear snoop detection flag

    /* Enable RTC Tick Interrupt */
    RTC_EnableInt(RTC_INTEN_SNPDIEN_Msk);
	RTC->INTSTS = RTC_INTSTS_SNPDIF_Msk; // clear status 
    NVIC_EnableIRQ(RTC_IRQn);

	printf(" Please switch PB.13(snoop pin) to High level(ex: Vcc) \n");
	printf(" Wait snoop detection interrupt \n");

    while(g_u32SnoopDetection == 0); // wait snoop detected

    printf("\n RTC Snoop Detection Demo End !!\n");

    /* Disable RTC Tick Interrupt */
    RTC_DisableInt(RTC_INTEN_SNPDIEN_Msk);
    NVIC_DisableIRQ(RTC_IRQn);

    while(1);

}






/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/



