/***************************************************************************//**
 * @file     main.c
 * @brief    Transmit and receive data from PC terminal through RS232 interface
 * @version  2.0.0
 *
 * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include <stdio.h>
#include "targetdev.h"

#include "uart.h"

#define PLL_CLOCK       36000000

#define nRTSPin                 (PA13)
#define RECEIVE_MODE            (0)
#define TRANSMIT_MODE           (1)

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
    /* Enable external HXT and 4MHz MIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_MIRCEN_Msk);;
    /* Waiting for 4MHz MIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_MIRCSTB_Msk);
    CLK_EnablePLL(CLK_PLLCTL_PLL_SRC_MIRC, PLL_CLOCK);
    CLK_WaitClockReady(CLK_STATUS_PLLSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_PLL, CLK_HCLK_CLK_DIVIDER(1));
    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN; // UART0 Clock Enable
    /* Select IP clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART0SEL_Msk;
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART0SEL_HIRC;// Clock source from 4 MHz internal medium speed RC oscillator (HIRC)
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();
    /* Set core clock as PLL_CLOCK from PLL */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PA13 for RTS  */
    PA->MODE = (PA->MODE & ~GPIO_MODE_MODE13_Msk) | (GPIO_PMD_OUTPUT << GPIO_MODE_MODE13_Pos);
    nRTSPin = RECEIVE_MODE;

    /* Set PA multi-function pins for UART0 RXD and TXD  */
    SYS->GPA_MFPH &= ~(SYS_GPA_MFPH_PA14MFP_Msk | SYS_GPA_MFPH_PA15MFP_Msk);
    SYS->GPA_MFPH |= (SYS_GPA_MFPH_PA14MFP_UART0_RXD | SYS_GPA_MFPH_PA15MFP_UART0_TXD);

    /* Lock protected registers */
    // SYS_LockReg();
}

/**
 *  @brief  main function.
 *  @param  None
 *  @return None
 */
int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Set UART Configuration */
    UART_Init();
    FMC->ISPCTL |= FMC_ISPCTL_ISPEN_Msk;    // (1ul << 0)
    g_apromSize = GetApromSize();
    GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);
    //printf("%X, %X, %X, %X\n", SYS->PDID, g_apromSize, g_dataFlashAddr, g_dataFlashSize);
    SysTick->LOAD = 300000 * CyclesPerUs;
    SysTick->VAL   = (0x00);
    SysTick->CTRL = SysTick->CTRL | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;//using cpu clock

    while (1)
    {
        if ((bufhead >= 4) || (bUartDataReady == TRUE))
        {
            uint32_t lcmd;
            lcmd = inpw(uart_rcvbuf);

            if (lcmd == CMD_CONNECT)
            {
                break;
            }
            else
            {
                bUartDataReady = FALSE;
                bufhead = 0;
            }
        }

        //if((SysTick->CTRL & (1 << 16)) != 0)//timeout, then goto APROM
        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
        {
            goto _APROM;
        }
    }

    while (1)
    {
        if (bUartDataReady == TRUE)
        {
            bUartDataReady = FALSE;
            NVIC_DisableIRQ(UART0_IRQn);
            nRTSPin = TRANSMIT_MODE;
            ParseCmd(uart_rcvbuf, 64);
            PutString();

            while ((UART0->FIFOSTS & UART_FIFOSTS_TXENDF_Msk) == 0);

            nRTSPin = RECEIVE_MODE;
            NVIC_EnableIRQ(UART0_IRQn);
        }
    }

_APROM:
    outpw(&SYS->RSTSTS, 3);//clear bit
    outpw(&FMC->ISPCTL, inpw(&FMC->ISPCTL) & 0xFFFFFFFC);
    outpw(&SCB->AIRCR, (V6M_AIRCR_VECTKEY_DATA | V6M_AIRCR_SYSRESETREQ));

    /* Infinite loop */
    while (1);
}
