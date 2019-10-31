/***************************************************************************//**
 * @file     main.c
 * @brief    Transmit and receive data from PC terminal through RS232 interface
 * @version  2.0.0
 *
 * @copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/

#include <stdio.h>
#include <string.h>
#include "targetdev.h"

#include "uart.h"

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


/**
 *  @brief  Init system clock and I/O multi function .
 *  @param  None
 *  @return None
 */
void SYS_Init(void)
{
    CLK->PWRCTL |= (CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_MIRCEN_Msk);
    while ((!(CLK->STATUS & CLK_STATUS_MIRCSTB_Msk)));
    CLK_EnablePLL(CLK_PLLCTL_PLL_SRC_MIRC, PLL_CLOCK);
    while ((!(CLK->STATUS & CLK_STATUS_PLLSTB_Msk)));
    CLK->CLKDIV0 = (CLK->CLKDIV0 & ~CLK_CLKDIV0_HCLKDIV_Msk) | CLK_HCLK_CLK_DIVIDER(1);
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~(CLK_CLKSEL0_HIRCSEL_Msk | CLK_CLKSEL0_HCLKSEL_Msk)) | CLK_CLKSEL0_HCLKSEL_PLL;

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_I2C1_EN; // I2C1 Clock Enable

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();
    /* Set core clock as PLL_CLOCK from PLL */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PA multi-function pins for I2C  */
    SYS->GPA_MFPH &= ~(SYS_GPA_MFPH_PA10MFP_Msk | SYS_GPA_MFPH_PA11MFP_Msk);
    SYS->GPA_MFPH |= (SYS_GPA_MFPH_PA10MFP_I2C1_SDA | SYS_GPA_MFPH_PA11MFP_I2C1_SCL);
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
    uint32_t cmd_buff[16];
    /* Unlock protected registers */
    SYS_UnlockReg();
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Set I2C Configuration */
    I2C_Init();
    FMC->ISPCTL |= FMC_ISPCTL_ISPEN_Msk;    // (1ul << 0)
    g_apromSize = GetApromSize();
    GetDataFlashInfo(&g_dataFlashAddr, &g_dataFlashSize);
    //printf("%X, %X, %X, %X\n", SYS->PDID, g_apromSize, g_dataFlashAddr, g_dataFlashSize);
    SysTick->LOAD = 300000 * CyclesPerUs;
    SysTick->VAL   = (0x00);
    SysTick->CTRL = SysTick->CTRL | SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_ENABLE_Msk;//using cpu clock

    while (1)
    {
        if (bI2cDataReady == 1)
        {
            goto _ISP;
        }

        //if((SysTick->CTRL & (1 << 16)) != 0)//timeout, then goto APROM
        if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)
        {
            goto _APROM;
        }

    }

_ISP:

    while (1)
    {
        if (bI2cDataReady == 1)
        {
            memcpy(cmd_buff, i2c_rcvbuf, 64);
            bI2cDataReady = 0;
            ParseCmd((unsigned char *)cmd_buff, 64);
            NVIC_EnableIRQ(I2C1_IRQn);
        }
    }

_APROM:
    outpw(&SYS->RSTSTS, 3);//clear bit
    outpw(&FMC->ISPCTL, inpw(&FMC->ISPCTL) & 0xFFFFFFFC);
    outpw(&SCB->AIRCR, (V6M_AIRCR_VECTKEY_DATA | V6M_AIRCR_SYSRESETREQ));

    /* Infinite loop */
    while (1);
}
