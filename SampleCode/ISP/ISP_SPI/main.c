/***************************************************************************//**
 * @file     main.c
 * @brief    ISP tool main function
 * @version  2.0.0
 *
 * Copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include <string.h>
#include "targetdev.h"
#include "spi_transfer.h"

#define PLL_CLOCK       36000000

__attribute__((weak)) uint32_t TIMER_Open(TIMER_T *timer, uint32_t u32Mode, uint32_t u32Freq)
{
    uint32_t u32Clk = __HXT; // TIMER_GetModuleClock(timer);
    uint32_t u32Cmpr = 0UL, u32Prescale = 0UL;

    /* Fastest possible timer working freq is (u32Clk / 2). While cmpr = 2, prescaler = 0. */
    if (u32Freq > (u32Clk / 2UL))
    {
        u32Cmpr = 2UL;
    }
    else
    {
        u32Cmpr = u32Clk / u32Freq;
        u32Prescale = (u32Cmpr >> 24);  /* for 24 bits CMPDAT */

        if (u32Prescale > 0UL)
        {
            u32Cmpr = u32Cmpr / (u32Prescale + 1UL);
        }
    }

    timer->CTL = u32Mode | u32Prescale;
    timer->CMP = u32Cmpr;
    return (u32Clk / (u32Cmpr * (u32Prescale + 1UL)));
}

void TIMER3_Init(void)
{
    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_TMR3_EN;
    /* Select IP clock source */
    CLK->CLKSEL2 = (CLK->CLKSEL2 & (~CLK_CLKSEL2_TMR3SEL_Msk)) | CLK_CLKSEL2_TMR3SEL_HXT;
    // Set timer frequency to 3HZ
    TIMER_Open(TIMER3, TIMER_PERIODIC_MODE, 3);
    // Enable timer interrupt
    TIMER_EnableInt(TIMER3);
    // Start Timer 3
    TIMER_Start(TIMER3);
}

uint32_t CLK_EnablePLL(uint32_t u32PllClkSrc, uint32_t u32PllFreq)
{
    uint32_t u32PllCr,u32PLL_N,u32PLL_M,u32PLLReg;

    /* PLL source clock from MIRC (4MHz) */
    CLK->PLLCTL = (CLK->PLLCTL & ~CLK_PLLCTL_PLL_SRC_MIRC) | (CLK_PLLCTL_PLL_SRC_MIRC);
    u32PllCr =__MIRC;

    u32PLL_N=u32PllCr/1000000;
    u32PLL_M=u32PllFreq/1000000;
    while(1)
    {
        if(u32PLL_M<=48 && u32PLL_N<=36 ) break;
        u32PLL_M >>=1;
        u32PLL_N >>=1;
    }
    u32PLLReg = (u32PLL_M<<CLK_PLLCTL_PLLMLP_Pos) | ((u32PLL_N-1)<<CLK_PLLCTL_INDIV_Pos);
    CLK->PLLCTL = ( CLK->PLLCTL & ~(CLK_PLLCTL_PLLMLP_Msk | CLK_PLLCTL_INDIV_Msk ) )| u32PLLReg;

    CLK->PLLCTL = (CLK->PLLCTL & ~CLK_PLLCTL_PLLSRC_Msk) | (CLK_PLLCTL_PLL_SRC_MIRC);

    CLK->PLLCTL &= ~CLK_PLLCTL_PD_Msk;

    return 1;
}

void SYS_Init(void)
{
    SYS_UnlockReg();

    CLK->PWRCTL |= (CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_MIRCEN_Msk);
    while ((!(CLK->STATUS & CLK_STATUS_MIRCSTB_Msk)));
    CLK_EnablePLL(CLK_PLLCTL_PLL_SRC_MIRC, PLL_CLOCK);
    while ((!(CLK->STATUS & CLK_STATUS_PLLSTB_Msk)));
    CLK->CLKDIV0 = (CLK->CLKDIV0 & ~CLK_CLKDIV0_HCLKDIV_Msk) | CLK_HCLK_CLK_DIVIDER(1);
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~(CLK_CLKSEL0_HIRCSEL_Msk | CLK_CLKSEL0_HCLKSEL_Msk)) | CLK_CLKSEL0_HCLKSEL_PLL;

    /* Enable SPI1 peripheral clock */
    CLK->APBCLK |= CLK_APBCLK_SPI1_EN;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* First, Clear PB0, PB1, PB2 and PB3 multi-function pins to '0', and then        */
    /* Set PB0, PB1, PB2 and PB3 multi function pin to SPI1 MISO0, MOSI0, CLK and SS0 */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk | SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_SPI1_MOSI0 | SYS_GPB_MFPL_PB1MFP_SPI1_MISO0 | SYS_GPB_MFPL_PB2MFP_SPI1_CLK | SYS_GPB_MFPL_PB3MFP_SPI1_SS0);
}

int main(void)
{
    uint32_t cmd_buff[16];
    SYS_Init();
    CLK->AHBCLK |= CLK_AHBCLK_ISPCKEN_Msk;
    //FMC->ISPCTL |= FMC_ISPCTL_ISPEN_Msk | FMC_ISPCTL_APUEN_Msk;    // (1ul << 0)
    FMC->ISPCTL |= FMC_ISPCTL_ISPEN_Msk;    // (1ul << 0)
    g_apromSize = GetApromSize();
    GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);
    SPI_Init();
    GPIO_Init();
    TIMER3_Init();

    while (1)
    {
        if (bSpiDataReady == 1)
        {
            goto _ISP;
        }

        if (TIMER3->INTSTS & TIMER_INTSTS_CNTIF_Msk)
        {
            goto _APROM;
        }
    }

_ISP:

    while (1)
    {
        if (bSpiDataReady == 1)
        {
            memcpy(cmd_buff, spi_rcvbuf, 64);
            bSpiDataReady = 0;
            ParseCmd((unsigned char *)cmd_buff, 64);
        }
    }

_APROM:
    outpw(&SYS->RSTSTS, 3);//clear bit
    outpw(&FMC->ISPCTL, inpw(&FMC->ISPCTL) & 0xFFFFFFFC);
    outpw(&SCB->AIRCR, (V6M_AIRCR_VECTKEY_DATA | V6M_AIRCR_SYSRESETREQ));

    /* Trap the CPU */
    while (1);
}
