/******************************************************************************
 * @file     main.c
 * @version  V3.00
 * $Revision: 6 $
 * $Date: 16/02/22 11:03a $
 * @brief    Demonstrate analog comparator (ACMP) comparison by comparing 
 *           ACMP0_P input and ACMP0_N voltage and shows the result on UART console
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano103.h"

void ACMP_IRQHandler(void)
{
    static uint32_t u32Cnt = 0;

    ACMP_CLR_INT_FLAG(ACMP, 0);     /* Clear ACMP 0 interrupt flag */
    if(ACMP_GET_OUTPUT(ACMP, 0))    /* Check Comparator 0 Output Status */
        printf("ACMP0_P voltage > ACMP0_N voltage (%d)\n", u32Cnt);     /* P > N => output 1 */
    else
        printf("ACMP0_P voltage <= ACMP0_N voltage (%d)\n", u32Cnt);    /* P <= N => output 0 */
    u32Cnt++;

}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    SYS_UnlockReg();    /* Unlock protected registers */

    /* Enable External XTAL (4~24 MHz) */
    CLK->PWRCTL = (CLK->PWRCTL & ~CLK_PWRCTL_HXTEN_Msk) | (0x1 << CLK_PWRCTL_HXTEN_Pos); // HXT Enabled

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to XTAL */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk)|CLK_CLKSEL0_HCLKSEL_HXT;

    /* Enable IP clock */
    CLK->APBCLK |= (CLK_APBCLK_UART0_EN | CLK_APBCLK_ACMP0_EN); /* Enable UART0 and ACMP0 clock */

    /* Select IP clock source */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART0SEL_Msk)|(0x0 << CLK_CLKSEL1_UART0SEL_Pos);  /* Clock source from external 12 MHz */

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD  */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD);

    /* Set PA.0 multi-function pin for ACMP0 positive input pin */
    /* Set PA.1 multi-function pin for ACMP0 negative input pin */
    SYS->GPA_MFPL = (SYS_GPA_MFPL_PA0MFP_ACMP0_P | SYS_GPA_MFPL_PA1MFP_ACMP0_N);

    /* Disable digital input path of analog pin ACMP0_P, ACMP0_N to prevent leakage */
    GPIO_DISABLE_DIGITAL_PATH(PA, 0x3);

    SYS_LockReg();  /* Register write-protection enabled */
}

int32_t main(void)
{
    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary. */
    SYS_Init();

    /* Configure UART0: 115200, 8-bit word, no parity bit, 1 stop bit. */
    UART_Open(UART0, 115200);

    /* Reset ACMP */
    SYS_ResetModule(ACMP0_RST);

    //ACMP CPO0 : PA6
    //ACMP CPN0 : PA1
    //ACMP CPP0 : PA0
    /*********************
        P > N => 1
        P = N => 0
        P < N => 0
    **********************/

    printf("\n\n");
    printf("+---------------------------------------+\n");
    printf("|         ACMP Sample Code              |\n");
    printf("+---------------------------------------+\n");

    printf("\nThis sample code demonstrates ACMP0 function. Using ACMP0_P (PA.0) as ACMP0\n");
    printf("positive input and using ACMP0_N (PA.1) as the negative input\n");

    /* Configure ACMP0. Enable ACMP0 and select internal reference voltage as negative input. */
    ACMP_Open(ACMP, 0, ACMP_VNEG_PIN, ACMP_HYSTERESIS_DISABLE);
    ACMP_ENABLE_INT(ACMP, 0);   /* Enable ACMP0 interrupt */
    NVIC_EnableIRQ(ACMP_IRQn);  /* Enable ACMP0 NVIC */

    while(1);
}


/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/


