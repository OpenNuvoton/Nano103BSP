/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * $Revision: 9 $
 * $Date: 16/02/17 4:54p $
 * @brief   Demonstrate how to output system clock to CLKO (PB.2) pin with the system clock / 4 frequency,
 *          test wakeup from powerdown and enable BOD detect.
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
 ******************************************************************************/
#include <stdio.h>
#include "Nano103.h"

#define SIGNATURE       0x125ab234
#define FLAG_ADDR       0x20001FFC

__IO uint32_t u32PWDU_WakeFlag = 0;
__IO uint32_t u32WDT_Ticks = 0;
/*---------------------------------------------------------------------------------------------------------*/
/*  Brown Out Detector IRQ Handler                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void BOD_IRQHandler(void)
{
    SYS_CLEAR_BOD_INT_FLAG(); /* Clear BOD Interrupt Flag */
    printf("Brown Out is Detected\n");
}

/*---------------------------------------------------------------------------------------------------------*/
/*  WDT_IRQ IRQ Handler                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
void WDT_IRQHandler(void)
{
    __IO uint32_t u32IntSts;
	
    /* Get Watchdog Timer Status*/
    u32IntSts = WDT->STATUS;                

    if (u32IntSts & WDT_STATUS_WKF_Msk)           /* Get Watchdog Timer Wake-up Status Flag */
        printf("WDT Wake Up Interrupt Occurs.\n");
    else                                          /* Get Watchdog Timer Time-out Interrupt */
        printf("WDT Interrupt Occurs <%d>.\n", u32WDT_Ticks);

    if (u32IntSts & WDT_STATUS_WDTIF_Msk) /* Get Watchdog Timer Time-out Interrupt */
        u32WDT_Ticks++;

    WDT->STATUS = u32IntSts; /* Clear Watchdog Timer Interrupts */
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Power Down Wake Up IRQ Handler                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
void PDWU_IRQHandler(void)
{
    printf("PDWU_IRQHandler running...\n");
    u32PWDU_WakeFlag = 1;  
    CLK->WKINTSTS = CLK_WK_INTSTS_IS; /* Clear Wake-up Interrupts */
}

/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{    
    SYS_UnlockReg(); /* Unlock protected registers */

    /* Enable external 12MHz HXT, 32KHz LXT, HIRC and MIRC*/
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_HIRC0STB_Msk | CLK_STATUS_HIRC1STB_Msk | CLK_STATUS_MIRCSTB_Msk); 
    
    CLK_SetCoreClock(32000000); /*  Set HCLK frequency 32MHz */

    CLK_EnableModuleClock(UART0_MODULE); /* Enable IP clock */

    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HIRC,CLK_UART0_CLK_DIVIDER(1)); /* Select IP clock source */
																															
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );

    /* Set PB multi-function pins for Clock Output */
    SYS->GPB_MFPL = ( SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB2MFP_Msk ) |  SYS_GPB_MFPL_PB2MFP_CLKO;  

    SYS_LockReg(); /* Lock protected registers */
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/    
    SYS_ResetModule(UART0_RST); /* Reset IP */    
    UART_Open(UART0, 115200);   /* Configure UART0 and set UART0 Baudrate */
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
    printf("\n\nCPU @ %dHz\n", SystemCoreClock);

    /*
        This sample code will show some function about system manager controller and clock controller:
        1. Read PDID
        2. Get and clear reset source
        3. Setting about BOD
        4. Output system clock from CKO pin, and the output frequency = system clock / 4
    */    
    printf(" Nano103 System Driver Sample Code \n");
    if (M32(FLAG_ADDR) == SIGNATURE) {
        printf("  CPU Reset success!\n");
        M32(FLAG_ADDR) = 0;
        printf("  Press any key to continue ...\n");
        getchar();
    }

    /*---------------------------------------------------------------------------------------------------------*/
    /* Misc system function test                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Read Part Device ID */
    printf("Product ID 0x%x\n", SYS->PDID);

    /* Get reset source from last operation */
    u32data = SYS_GetResetSrc();
    printf("Reset Source 0x%x\n", u32data);

    /* Clear reset source */
    SYS_ClearResetSrc(u32data);

    /* Unlock protected registers for Brown-Out Detector and power down settings */
    SYS_UnlockReg();

    /* Check if the write-protected registers are unlocked before BOD setting and CPU Reset */
    if (SYS->REGLCTL != 0) {
        printf("Protected Address is Unlocked\n");
    }

    /* Enable Brown-Out Detector and Low Voltage Reset function, and set Brown-Out Detector voltage 2.5V ,
       Enable Brown-Out Interrupt function */
    SYS_EnableBOD(SYS_BODCTL_BODIE_Msk, SYS_BODCTL_BODVL_2_5V);
    
    NVIC_EnableIRQ(BOD_IRQn);  /* Enable BOD IRQ */

    /* Waiting for message send out */
    UART_WAIT_TX_EMPTY(UART0);

    /* Switch HCLK clock source to Internal 12MHz */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC,CLK_HCLK_CLK_DIVIDER(1));

    /* Enable CLKO and output frequency = system clock / 2^(1 + 1) */
    CLK_EnableCKO(CLK_CLKSEL2_CLKOSEL_HCLK, 1, 0); /* CKO = HCLK / 4 */

    CLK_EnableModuleClock(WDT_MODULE);  /* Enable WDT clock */
		
    WDT->CTL = 0x00000050 | 0x00000004 | 0x00000008; /* Enable WDT  */
    WDT->INTEN |=  0x00000001;                       /* Enable Enable WDT interrupt */
    NVIC_EnableIRQ(WDT_IRQn);                        /* Enable WDT NVIC */

    /* Enable wake up interrupt source */
    CLK->PWRCTL |= CLK_PWRCTL_WAKEINT_EN;  
    /* Enable IRQ request for PDWU interrupt */
    NVIC_EnableIRQ(PDWU_IRQn);

    printf("u32PWDU_WakeFlag = %x\n",u32PWDU_WakeFlag);
    printf("Enter Power Down Mode >>>>>>>>>>>\n");
    /* clear software semaphore */
    u32PWDU_WakeFlag = 0;                   
    /* waits for message send out */
    while(!(UART0->FIFOSTS & UART_FIFOSTS_TXENDF_Msk));
    /* Let system enter to Power-down mode */
    CLK_PowerDown();

    /* CPU Reset test */
    printf("Waits for 5 times WDT interrupts.....\n");
    while (u32WDT_Ticks <= 5);

    printf("<<<<<<<<<< Program resumes execution.\n");
    printf("u32PWDU_WakeFlag = %x\n",u32PWDU_WakeFlag);

    /* Write a signature work to SRAM to check if it is reset by software */
    M32(FLAG_ADDR) = SIGNATURE;
    printf("\n\n  >>> Reset CPU <<<\n");

    /* Reset CPU */
    SYS_ResetCPU();
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
