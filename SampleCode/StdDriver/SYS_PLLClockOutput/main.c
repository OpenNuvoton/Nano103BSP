/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * $Revision: 9 $
 * $Date: 16/02/17 4:54p $
 * @brief    Change system clock to different PLL frequency and output system clock from CLKO pin.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/

#include <stdio.h>
#include "Nano103.h"

#define SIGNATURE       0x125ab234
#define FLAG_ADDR       0x20001FFC

__IO uint32_t u32PWDU_WakeFlag = 0;

/*---------------------------------------------------------------------------------------------------------*/
/*  Brown Out Detector IRQ Handler                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void BOD_IRQHandler(void)
{
    SYS_CLEAR_BOD_INT_FLAG(); /* Clear BOD Interrupt Flag */
    printf("Brown Out is Detected\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Simple calculation test function                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
#define PI_NUM  256
int32_t f[PI_NUM + 1];

uint32_t piTbl[19] = {3141, 5926, 5358, 9793, 2384, 6264, 3383, 2795, 288, 4197,
                      1693, 9937, 5105, 8209, 7494, 4592, 3078, 1640, 6284
                     };

int32_t piResult[19];

int32_t pi(void)
{
    int32_t i = 0, i32Err = 0;
    int32_t a = 10000, b = 0, c = PI_NUM, d = 0, e = 0, g = 0;

    /* Simple numerical methods for calculating the digits of pi. */
    for(; b - c;)
        f[b++] = a / 5;

    for(; d = 0, g = c * 2; c -= 14, piResult[i++] = e + d / a, e = d % a)
    {
        if(i == 19)
            break;

        for(b = c; d += f[b] * a, f[b] = d % --g, d /= g--, --b; d *= b);
    }

    for(i = 0; i < 19; i++)
    {
        if(piTbl[i] != piResult[i])
            i32Err = -1;
    }

    return i32Err;
}

void Delay(uint32_t x)
{
    int32_t i;

    /* Generate a delay loop */
    for(i = 0; i < x; i++)
    {
        __NOP();
        __NOP();
    }
}

uint32_t g_au32PllSetting[] =
{
    16000000,  /* PLL = 16MHz */
    18000000,  /* PLL = 18MHz */
    20000000,  /* PLL = 20MHz */
    22000000,  /* PLL = 22MHz */
    24000000,  /* PLL = 24MHz */
    28000000,  /* PLL = 28MHz */
    32000000,  /* PLL = 32MHz */
    36000000,  /* PLL = 36MHz */
};


void SYS_PLL_Test(void)
{
    int32_t  i;

    /*---------------------------------------------------------------------------------------------------------*/
    /* PLL clock configuration test                                                                            */
    /*---------------------------------------------------------------------------------------------------------*/

    printf("\n-----------------------[ Test PLL ]---------------------------\n");

    for(i = 0; i < sizeof(g_au32PllSetting) / sizeof(g_au32PllSetting[0]) ; i++)
    {
        /* Select HCLK clock source from PLL.
           PLL will be configured to twice specified frequency.
           And HCLK clock source divider will be set to 2.
        */
        CLK_SetCoreClock(g_au32PllSetting[i]);
        printf("Change system clock to %d Hz ...................... ", SystemCoreClock); /* Display system core clock */

        CLK_EnableCKO(CLK_CLKSEL2_CLKOSEL_HCLK, 1, 0); /* Output selected clock to CLKO, CLKO = HCLK / 2^(1 + 1) */

        Delay(0x400000); /* The delay loop is used to check if the CPU speed is increasing */

        if(pi())   /* Calculating lots of digits of pi */
        {
            printf("[FAIL]\n");
        }
        else
        {
            printf("[OK]\n");
        }

        CLK_DisableCKO(); /* Disable CLKO clock */
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{
    SYS_UnlockReg(); /* Unlock protected registers */

    /* Enable external 12MHz HXT, 32KHz LXT, HIRC and MIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_HIRC0STB_Msk | CLK_STATUS_HIRC1STB_Msk  | CLK_STATUS_MIRCSTB_Msk);

    CLK_SetCoreClock(32000000); /* Set HCLK frequency 32MHz */

    CLK_EnableModuleClock(UART0_MODULE); /* Enable IP clock */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HIRC,CLK_UART0_CLK_DIVIDER(1)); /* Select IP clock source */

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD);

    SYS->GPB_MFPL = ( SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB2MFP_Msk ) |  SYS_GPB_MFPL_PB2MFP_CLKO; /* Set PB multi-function pins for Clock Output */

    SYS_LockReg(); /* Lock protected registers */
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS_ResetModule(UART0_RST); /* Reset IP */
    UART_Open(UART0, 115200); /* Configure UART0 and set UART0 Baudrate */
}

int32_t main (void)
{
    uint32_t u32data;

    /* HCLK will be set to 32MHz in SYS_Init(void)*/
    if(SYS->REGLCTL == 1) // In end of main function, program issued CPU reset and write-protection will be disabled.
        SYS_LockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init(); //In the end of SYS_Init() will issue SYS_LockReg() to lock protected register. If user want to write protected register, please issue SYS_UnlockReg() to unlock protected register.

    UART0_Init(); /* Init UART0 for printf */
    printf("\n\nCPU @ %dHz\n", SystemCoreClock); /* Display system core clock */

    /*
        This sample code will show some function about system manager controller and clock controller:
        1. Read PDID
        2. Get and clear reset source
        3. Setting about BOD
        4. Output system clock from CLKO pin, and the output frequency = system clock / 4
    */
    printf("+----------------------------------------+\n");
    printf("|    Nano103 System Driver Sample Code   |\n");
    printf("+----------------------------------------+\n");
    if (M32(FLAG_ADDR) == SIGNATURE)
    {
        printf("  CPU Reset success!\n");
        M32(FLAG_ADDR) = 0;
        printf("  Press any key to continue ...\n");
        getchar();
    }

    /*---------------------------------------------------------------------------------------------------------*/
    /* Misc system function test                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    printf("Product ID 0x%x\n", SYS->PDID); /* Read Part Device ID */

    u32data = SYS_GetResetSrc(); /* Get reset source from last operation */
    printf("Reset Source 0x%x\n", u32data); /* Get reset source data */
    SYS_ClearResetSrc(u32data); /* Clear reset source */

    SYS_UnlockReg(); /* Unlock protected registers for Brown-Out Detector and power down settings */

    /* Check if the write-protected registers are unlocked before BOD setting and CPU Reset */
    if (SYS->REGLCTL != 0)
    {
        printf("Protected Address is Unlocked\n");
    }

    /* Enable Brown-Out Detector and Low Voltage Reset function, and set Brown-Out Detector voltage 2.5V ,
       Enable Brown-Out Interrupt function */
    SYS_EnableBOD(SYS_BODCTL_BODIE_Msk,SYS_BODCTL_BODVL_2_5V);
    NVIC_EnableIRQ(BOD_IRQn); /* Enable BOD IRQ */

    SYS_PLL_Test(); /* Run PLL Test */

    UART_WAIT_TX_EMPTY(UART0); /* Waiting for message send out */

    /* Enable CLKO and output frequency = system clock / 2^(1 + 1) */
    CLK_EnableCKO(CLK_CLKSEL2_CLKOSEL_HCLK, 1, 0); /* CLKO = HCLK / 4 */

    CLK->CLKDIV0 = (CLK->CLKDIV0 & ~CLK_CLKDIV0_HCLKDIV_Msk) | CLK_CLKSEL0_HCLKSEL_HIRC; /* Switch HCLK clock source to Internal 12MHz */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_HCLK_CLK_DIVIDER(1);

    CLK_DisablePLL(); /* Set PLL to Power down mode and HW will also clear PLLSTB bit in CLKSTATUS register */

    M32(FLAG_ADDR) = SIGNATURE; /* Write a signature work to SRAM to check if it is reset by software */
    printf("\n\n  >>> Reset CPU <<<\n");

    SYS_ResetCPU(); /* Reset CPU */
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
