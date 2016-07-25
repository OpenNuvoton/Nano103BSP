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

#define HIRC_TYPE  (0) // 0: 12MHz, 1: 16MHz, 2: 36MHz
#define LDO_VOL    (0) // 0: 1.8v,  1: 1.6v
#define MODE       (1) // 0: Idle,  1: Operating

/*
 *  After removed LED on Tiny board, operating and Idle current consumption should like following table
 *
 *   Mode            | operating  |    Idle
 *  -------------------------------------------
 *  1.8V & RC12MHz   |    2.5 mA  |    1   mA
 *  -------------------------------------------
 *  1.6V & RC12MHz   |    2.2 mA  |    0.9 mA
 *  -------------------------------------------
 *  1.8V & RC16MHz   |    3.1 mA  |    1.2 mA
 *  -------------------------------------------
 *  1.6V & RC16MHz   |    2.8 mA  |    1.1 mA
 *  -------------------------------------------
 *  1.8V & RC36MHz   |    6.7 mA  |    1.9 mA
 *  -------------------------------------------
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

#if(HIRC_TYPE == 2)  // HIRC1 36MHz     

    CLK->PWRCTL =  CLK->PWRCTL | CLK_PWRCTL_HIRC1EN_Msk;
    CLK_WaitClockReady(CLK_STATUS_HIRC1STB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC1, CLK_HCLK_CLK_DIVIDER(1));  // HCLK = HIRC1(36MHz)
    CLK->PWRCTL =  CLK->PWRCTL & ~(CLK_PWRCTL_HIRC0EN_Msk);           // HIRC0 disable

#elif(HIRC_TYPE == 1) // HIRC0 16MHz

    CLK->PWRCTL =  CLK->PWRCTL | CLK_PWRCTL_HIRC0FSEL_Msk;

#endif

}

int32_t main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    SYS->BODCTL = SYS->BODCTL & ~(SYS_BODCTL_LVREN_Msk); // LVR Disable
    SYS->BODCTL = SYS->BODCTL & ~(SYS_BODCTL_BODEN_Msk); // BOD Disable


// LDO 1.6V operate speed should be small than 18MHz, therefore HIRC1(36MHz) can't run on LDO 1.6v.
#if((LDO_VOL == 1) && (HIRC_TYPE != 2) )
    outpw(0x50000070, (inpw(0x50000070) & ~(3 << 2)) | (1 << 2)); // LDO Output Voltage = 1.6v
#endif

    outpw(0x50000070, (inpw(0x50000070)  | (1 << 15)  | (1 << 23)) ); // [15]LVDR Test Mode Enable, [23]LVDR Low-power Test Mode Enable

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
    CPU_PC_Test();  //while(1);
#else
    __WFI();
#endif
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
