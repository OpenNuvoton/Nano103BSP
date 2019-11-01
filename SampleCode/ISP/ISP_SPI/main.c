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

uint32_t Pclk0;
uint32_t Pclk1;

#define TEST_COUNT 16

uint32_t u32DataCount;
uint32_t *_response_buff;
uint32_t spi_rcvbuf[TEST_COUNT];

#define PLL_CLOCK       36000000

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
    CLK->PWRCTL |= (CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_MIRCEN_Msk);
    while ((!(CLK->STATUS & CLK_STATUS_MIRCSTB_Msk)));
    CLK_EnablePLL(CLK_PLLCTL_PLL_SRC_MIRC, PLL_CLOCK);
    while ((!(CLK->STATUS & CLK_STATUS_PLLSTB_Msk)));
    CLK->CLKDIV0 = (CLK->CLKDIV0 & ~CLK_CLKDIV0_HCLKDIV_Msk) | CLK_HCLK_CLK_DIVIDER(1);
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~(CLK_CLKSEL0_HIRCSEL_Msk | CLK_CLKSEL0_HCLKSEL_Msk)) | CLK_CLKSEL0_HCLKSEL_PLL;

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_SPI1_EN; // SPI1 Clock Enable

    /* Select SPI1 clock source */
    CLK->CLKSEL2 = (CLK->CLKSEL2 & ~(CLK_CLKSEL2_SPI1SEL_Msk))| CLK_CLKSEL2_SPI1SEL_PLL;

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

void SPI_Init(void)
{
    /* Configure as a slave, clock idle low, 32-bit transaction, drive output on falling clock edge and latch input on rising edge. */
    /* Configure SPI1 as a low level active device. */
    /* Default setting: slave selection signal is low level active. */
    SPI1->SSCTL = SPI_SS0_ACTIVE_LOW | SPI_SSCTL_SSLTRIG_Msk;
    /* Default setting: MSB first, disable unit transfer interrupt, SP_CYCLE = 0. */
    SPI1->CTL = SPI_SLAVE | ((32 & 0x1F) << SPI_CTL_DWIDTH_Pos) | (SPI_MODE_0);
    /* Set DIVIDER = 0 */
    SPI1->CLKDIV = 0U;
}

int main(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    SYS_Init();

    CLK->AHBCLK |= CLK_AHBCLK_ISPCKEN_Msk;
    FMC->ISPCTL |= FMC_ISPCTL_ISPEN_Msk | FMC_ISPCTL_APUEN_Msk;    // (1ul << 0)
    g_apromSize = GetApromSize();
    GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);
    SPI_Init();

    /* Get APROM size, data flash size and address */
    g_apromSize = GetApromSize();
    GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);

_ISP:
    u32DataCount = 0;

    SysTick->CTRL = 0UL;

    /* Check data count */
    while(u32DataCount < TEST_COUNT)
    {
        /* Write to TX register */
        SPI1->TX0 = _response_buff[u32DataCount];
        /* Trigger SPI data transfer */
        SPI1->CTL |= SPI_CTL_GOBUSY_Msk;
        /* Check SPI1 busy status */
        while((SPI1->CTL & SPI_CTL_GOBUSY_Msk))
        {
            if(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
            {
                goto _ISP;
            }
        }

        /* Read RX register */
        spi_rcvbuf[u32DataCount] = SPI1->RX0;
        u32DataCount++;

        SysTick->LOAD = 1000 * CyclesPerUs;
        SysTick->VAL  = (0x00);
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;
    }

    /* Disable SysTick counter */
    SysTick->CTRL = 0UL;

    if((u32DataCount == TEST_COUNT) && ((spi_rcvbuf[0] & 0xFFFFFF00) == 0x53504900))
    {
        spi_rcvbuf[0] &= 0x000000FF;
        ParseCmd((unsigned char *)spi_rcvbuf, 64);
    }

    goto _ISP;

}
