/******************************************************************************
 * @file     LDROM_main.c
 * @version  V1.00
 * $Revision: 6 $
 * $Date: 16/02/16 9:21a $
 * @brief    FMC LDROM IAP sample program for Nano103 MCU
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>

#include "Nano103.h"

typedef void (FUNC_PTR)(void);

/**
 * @brief    Routine to get a char
 * @param    None
 * @returns  Get value from UART debug port or semihost
 * @details  Wait UART debug port or semihost to input a char.
 */
static char GetChar(void)
{
    while(1)
    {
        if ((UART0->FIFOSTS & UART_FIFOSTS_RXEMPTY_Msk) == 0)
        {
            return (UART0->DAT);
        }
    }
}

extern void SendChar_ToUART(int ch);

static void PutString(char *str)
{
    while (*str != '\0')
    {
        SendChar_ToUART(*str++);
    }
}

void SYS_Init(void)
{
    SYS_UnlockReg();                             /* Unlock protected registers */

    /* Set HCLK source form HXT and HCLK source divide 1  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_HCLK_CLK_DIVIDER(1));

    /* Enable external 12MHz HXT, 32KHz LXT and HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_HIRC0STB_Msk | CLK_STATUS_HIRC1STB_Msk | CLK_STATUS_MIRCSTB_Msk);

    CLK_SetCoreClock(32000000);                  /*  Set HCLK frequency 32MHz */

    CLK_EnableModuleClock(UART0_MODULE);         /* Enable UART0 input clock */

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HIRC,CLK_UART0_CLK_DIVIDER(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PA multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );

    SYS_LockReg();                               /* Lock protected registers */
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS_ResetModule(UART0_RST);        /* Reset UART0 */
    UART_Open(UART0, 115200);          /* Configure UART0 and set UART0 as baudrate 115200, N,8,1 */
}


/*
 *  Set stack base address to SP register.
 */
#ifdef __ARMCC_VERSION                 /* for Keil compiler */
__asm __set_SP(uint32_t _sp)
{
    MSR MSP, r0
    BX lr
}
#endif


int main()
{
    FUNC_PTR    *func;                 /* function pointer                                */
    uint32_t    sp;                    /* stack base address                              */

    SYS_Init();                        /* Init System, IP clock and multi-function I/O    */
    UART0_Init();                      /* Initialize UART 0.                              */

    /*---------------------------------------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/

    PutString("\n\n");
    PutString("+----------------------------------------+\n");
    PutString("|     Nano103 FMC IAP Sample Code        |\n");
    PutString("|           [LDROM code]                 |\n");
    PutString("+----------------------------------------+\n");

    SYS_UnlockReg();                   /* Unlock protected registers */

    FMC_Open();                        /* Enable FMC ISP function */

    PutString("\n\nPress any key to branch to APROM...\n");
    GetChar();                         /* block on waiting for any one character input from UART0 */

    PutString("\n\nChange VECMAP and branch to APROM...\n");
    while (!(UART0->FIFOSTS & UART_FIFOSTS_TXEMPTY_Msk));       /* wait until UART0 TX FIFO is empty */

    /*
     *  The stack base address of an executable image is located at offset 0x0.
     *  Thus, this sample get stack base address of APROM code from FMC_APROM_BASE + 0x0.
     */
    sp = FMC_Read(FMC_APROM_BASE);

    /*
     *  The reset handler address of an executable image is located at offset 0x4.
     *  Thus, this sample get reset handler address of APROM code from FMC_APROM_BASE + 0x4.
     */
    func =  (FUNC_PTR *)FMC_Read(FMC_APROM_BASE+4);

    /*  NOTE!
     *     Before change VECMAP, user MUST disable all interrupts.
     */

    FMC->ISPCMD  = FMC_ISPCMD_VECMAP;              /* Vector remap APROM page 0 to address 0. */
    FMC->ISPADDR = FMC_APROM_BASE;
    FMC->ISPTRG  = FMC_ISPTRG_ISPGO_Msk;
    while (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk) ;
    SYS_LockReg();                                /* Lock protected registers */

#if defined (__GNUC__) && !defined(__ARMCC_VERSION) /* for GNU C compiler */
    asm("msr msp, %0" : : "r" (sp));
#else
    __set_SP(sp);
#endif
    /*
     *  Brach to the LDROM code's reset handler in way of function call.
     */
    func();

    while (1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
