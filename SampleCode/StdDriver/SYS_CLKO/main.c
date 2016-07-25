/******************************************************************************
* @file     main.c
* @version  V1.00
* $Revision: 6 $
* $Date: 16/02/17 4:54p $
* @brief    Demonstrate how to output clock to PB.2.
*
* @note
* Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano103.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{
    SYS_UnlockReg(); /* Unlock protected registers */

    /* Enable external 12MHz HXT, 32KHz LXT, HIRC and MIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_LIRCEN_Msk | CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_LIRCSTB_Msk | CLK_STATUS_HIRC0STB_Msk | CLK_STATUS_HIRC1STB_Msk | CLK_STATUS_MIRCSTB_Msk);

    /*  Set HCLK frequency 32MHz */
    CLK_SetCoreClock(32000000);

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HIRC,CLK_UART0_CLK_DIVIDER(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD);

    /* Set PB multi-function pins for Clock Output */
    SYS->GPB_MFPL = ( SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB2MFP_Msk ) |  SYS_GPB_MFPL_PB2MFP_CLKO;
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS_ResetModule(UART0_RST); /* Reset UART0 */
    UART_Open(UART0, 115200); /* Configure UART0 and set UART0 115200-8n1 Baudrate */
}

int32_t main (void)
{
    int tdelay = 1000000; /* delay 1 second */

    /* Init System, IP clock and multi-function I/O */
    SYS_Init(); //In the end of SYS_Init() will issue SYS_LockReg() to lock protected register. If user want to write protected register, please issue SYS_UnlockReg() to unlock protected register.

    /* Init UART0 for print message */
    UART0_Init();

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();
    printf("\n\nCPU @ %dHz\n", SystemCoreClock); /* Display System Core Clock */

    /*
        This sample code will Output module clock from PB.2 pin.
    */
    printf("+-----------------------------------------+\n");
    printf("| Nano103 Clock Output Sample Code        |\n");
    printf("+-----------------------------------------+\n");

    /* Enable PLL clock and set PLL clock to 36Mhz */
    CLK_EnablePLL(CLK_PLLCTL_PLL_SRC_HXT, 36000000);

    printf("This sample code will Output clock from PB.2 pin.\n");

    /* Switch HCLK source every 1 second. */
    printf("Switch clock source every 1 second.\n");

    /* Output selected clock to CLKO, CLKO = HCLK / 2^(1 + 1) */
    CLK_EnableCKO(CLK_CLKSEL2_CLKOSEL_HCLK, 1, 0);

    printf("CLK output = HCLK Clock Source from HXT, CLKO = 12 / 4 = 3MHz. \n");
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT, CLK_HCLK_CLK_DIVIDER(1)); /* Switch HCLK Clock Source to HXT */
    CLK_SysTickDelay(tdelay);                                      /* Wait 1 second to switch HCLK clock source */

    printf("CLK output = HCLK Clock Source from LXT, CLKO = 32k / 4 = 8kHz. \n");
    CLK_EnableCKO(CLK_CLKSEL2_CLKOSEL_HCLK, 1, 0);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_LXT, CLK_HCLK_CLK_DIVIDER(1)); /* Switch HCLK Clock Source to LXT */

    printf("CLK output = HCLK Clock Source from PLL, CLKO = 36M / 4 = 9MHz. \n");
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_PLL, CLK_HCLK_CLK_DIVIDER(1)); /* Switch HCLK Clock Source to PLL */
    CLK_SysTickDelay(tdelay);                                      /* Wait 1 second to switch HCLK clock source */

    printf("CLK output = HCLK Clock Source from LIRC, CLKO = 10k / 4 = 2.5kHz. \n");
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_LIRC, CLK_HCLK_CLK_DIVIDER(1)); /* Switch HCLK Clock Source to LIRC */

    printf("CLK output = HCLK Clock Source from HIRC, CLKO = 12M / 4 = 3MHz. \n");
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_HCLK_CLK_DIVIDER(1)); /* Switch HCLK Clock Source to HIRC */
    CLK_SysTickDelay(tdelay);                                       /* Wait 1 second to switch HCLK clock source */

    printf("CLK output = HCLK Clock Source from MIRC, CLKO = 4M / 4 = 1MHz. \n");
    CLK_EnableXtalRC(CLK_PWRCTL_MIRCEN_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_MIRC,CLK_HCLK_CLK_DIVIDER(1)); /* Switch HCLK Clock Source to MIRC */
    CLK_SysTickDelay(tdelay);                                      /* Wait 1 second to switch HCLK clock source */

    while(1);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
