/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 16/03/31 7:43p $
 * @brief    Sample code which implements a function to test system power consumption.
 *           If a system consumes more power than expected in operating or idle mode
 *           , this function can be used to check if there is any system setting that
 *           may cause power leakage.
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

#ifdef __DEBUG_MSG
#define DEBUG_MSG   printf
#else
#define DEBUG_MSG(...)
#endif

#define MODE       (1) // 0: Idle,  1: Operating

/*
 *  After removed LED on Tiny board, operating current should be around 0.5 mA.
 *  Idle current should be around 0.3 mA
 *
 */


extern void CPU_PC_Test(void);

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
    CLK->PWRCTL =  CLK->PWRCTL & ~(CLK_PWRCTL_LIRCEN_Msk);

    CLK->PWRCTL =  CLK->PWRCTL | CLK_PWRCTL_MIRCEN_Msk;
    CLK_WaitClockReady(CLK_STATUS_MIRCSTB_Msk);
    /* Switch HCLK Clock Source to MIRC. HCLK divided into 2 because LDO 1.2V operating speed should be smaller than 2MHz. */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_MIRC, CLK_HCLK_CLK_DIVIDER(2));

    SYS->RPDBCLK &= ~(SYS_RPDBCLK_RSTPDBCLK_Msk); // Reset pin setting, MIRC is selected as reset pin debounce clock.
    CLK->PWRCTL =  CLK->PWRCTL & ~(CLK_PWRCTL_HIRC0EN_Msk); // HIRC0 disable

    CLK->CLKSEL0 |=  CLK_CLKSEL0_ISPSEL_Msk; // ISP Clock Source from MIRC
    SYS->LDOCTL = (SYS->LDOCTL & ~(SYS_LDOCTL_FMCLVEN_Msk)) | SYS_LDOCTL_FMCLVEN_Msk; // Flash memory low voltage(1.2V) mode Enabled
    SYS->LDOCTL = (SYS->LDOCTL & ~(SYS_LDOCTL_LDOLVL_Msk)) | SYS_LDOCTL_LDO_LEVEL12; // LDO 1.2V
}

int32_t main(void)
{
    SYS_Init();

    SYS->BODCTL = SYS->BODCTL & ~(SYS_BODCTL_LVREN_Msk);
    SYS->BODCTL = SYS->BODCTL & ~(SYS_BODCTL_BODEN_Msk);

    FMC->ISPCTL |= FMC_ISPCTL_ISPEN_Msk;  // Enable FMC ISP function
    FMC->FTCTL |= 0x10; // FMC Frequency <= 20MHz

    SYS->RCCFCTL = 0;  // HIRC0, HIRC1 and MIRC Clock Filter disable

    /* Set function pin to GPIO mode */
    SYS->GPA_MFPL = 0;
    SYS->GPA_MFPH = 0;
    SYS->GPB_MFPL = 0;
    SYS->GPB_MFPH = 0;
    SYS->GPC_MFPL = 0;
    SYS->GPC_MFPH = 0;
    SYS->GPD_MFPL = 0;
    SYS->GPD_MFPH = 0;
    SYS->GPE_MFPL = 0;

    /* Enable GPIO pull up */
    PA->PUEN = 0xFFFF;
    PB->PUEN = 0xFFFF;
    PC->PUEN = 0xFFFF;
    PD->PUEN = 0xFFFF;
    PE->PUEN = 0xFFFF;

    SYS_UnlockReg(); /* Unlock protected registers */

    CLK->AHBCLK = 0;
    CLK->APBCLK = 0;

#if(MODE == 1)
    CPU_PC_Test();//while(1);
#else
    __WFI();
#endif

}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
