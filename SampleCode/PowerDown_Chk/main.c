/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/12/29 2:56p $
 * @brief    Sample code which implements a function to test system state before
 *           entering power-down mode. If a system consumes more power than
 *           expected in power-down mode, this function can be used to check if
 *           there is any system setting that may cause power leakage.
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


/* External functions */


/* Global variables */
__IO int32_t   _Wakeup_Flag = 0;    /* 1 indicates system wake up from power down mode */
__IO uint32_t  _Pin_Setting[11];    /* store Px_H_MFP and Px_L_MFP */
__IO uint32_t  _PullUp_Setting[6];  /* store GPIOx_PUEN */



/**
  * @brief  Store original setting of multi-function pin selection.
  * @param  None.
  * @return None.
  */
void SavePinSetting()
{
    /* Save Pin selection setting */
    _Pin_Setting[0] = SYS->GPA_MFPL;
    _Pin_Setting[1] = SYS->GPA_MFPH;
    _Pin_Setting[2] = SYS->GPB_MFPL;
    _Pin_Setting[3] = SYS->GPB_MFPH;
    _Pin_Setting[4] = SYS->GPC_MFPL;
    _Pin_Setting[5] = SYS->GPC_MFPH;
    _Pin_Setting[6] = SYS->GPD_MFPL;
    _Pin_Setting[7] = SYS->GPD_MFPH;
    _Pin_Setting[8] = SYS->GPE_MFPL;
    _Pin_Setting[9] = SYS->GPF_MFPL;

    /* Save Pull-up setting */
    _PullUp_Setting[0] =  PA->PUEN;
    _PullUp_Setting[1] =  PB->PUEN;
    _PullUp_Setting[2] =  PC->PUEN;
    _PullUp_Setting[3] =  PD->PUEN;
    _PullUp_Setting[4] =  PE->PUEN;
    _PullUp_Setting[5] =  PF->PUEN;
}

/**
  * @brief  Save multi-function pin setting and then go to power down.
  * @param  None.
  * @return None.
  */
void Enter_PowerDown()
{
    SavePinSetting(); /* Back up original setting */

    /* Set function pin to GPIO mode */
    SYS->GPA_MFPL = 0;
    SYS->GPA_MFPH = 0;
    SYS->GPB_MFPL = (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD); // exclude debug port;
    SYS->GPB_MFPH = 0;
    SYS->GPC_MFPL = 0;
    SYS->GPC_MFPH = 0;
    SYS->GPD_MFPL = 0;
    SYS->GPD_MFPH = 0;
    SYS->GPE_MFPL = 0;
    //SYS->GPE_MFPH = 0;
    //SYS->GPF_MFPL = 0x0000FF00;   // exclude GPF3:HXT_OUT, GPF2:HXT_IN
    SYS->GPF_MFPL = 0x00FFFF00; // exclude GPF5:ICE_DAT, GPF4:ICE_CLK, GPF3:HXT_OUT, GPF2:HXT_IN

    /* Enable GPIO pull up */
    PA->PUEN = 0xFFFF;
    PB->PUEN = 0xFFFF;
    PC->PUEN = 0xFFFF;
    PD->PUEN = 0xFFFF;
    PE->PUEN = 0xFFFF;
    //PF->PUEN = 0x0033;      /* exclude GPF3 and GPF2 which are HXT OUT/IN */
    PF->PUEN = 0x0003;      /* exclude GPF5:ICE_DAT, GPF4:ICE_CLK, GPF3:HXT_OUT, GPF2:HXT_IN */

    //CLK->PWRCTL |= CLK_PWRCTL_PDWKIEN_Msk;  /* Enable wake up interrupt source */
    //NVIC_EnableIRQ(PDWU_IRQn);             /* Enable IRQ request for PDWU interrupt */

    SYS_UnlockReg(); /* Unlock protected registers */
    SCB->SCR = SCB_SCR_SLEEPDEEP_Msk; /* Set Deep sleep */
    CLK->PWRCTL |= (CLK_PWRCTL_PDEN_Msk | CLK_PWRCTL_PDWKDLY_Msk ); /* Enable Power down and wake up */

    //__WFI();   /* system really enter power down here ! */
}

/**
  * @brief  Power Down check function.
  * @param  None.
  * @return None.
  */
void PowerDown_Check()
{
    uint32_t i,j;
    volatile uint32_t Tmp_NVIC_ISER;
    uint32_t gpio_error_count = 0;
    char *symbo_gpio[6] = {"A", "B", "C", "D", "E", "F"};
    GPIO_T *tGPIO;
    uint32_t SYS_MFP = 0xffffffff;

    printf("\n/***** PowerDown Check *****/");

    printf("\n/***** GPIO Check: *****/");

    Tmp_NVIC_ISER = NVIC->ISER[0]; //save NVIC setting

    NVIC->ICER[0] = 0xFFFFFFFF; //disables a device-specific interrupt in the NVIC interrupt controller

    for(j = 0; j < 11; j++) { /* Check MFP is set GPIO mode or not */
        SYS_MFP = *((volatile unsigned int *)(SYS_BASE + 0x30 + (j *4)));

        for(i = 0; i < 8; i++) {
            if(SYS_MFP & (0x7 << (i*4))) {
                gpio_error_count++;
                printf("\nGP%s Pin %d not set GPIO !!", symbo_gpio[(j/2)], (i+(8*(j%2))) );
            }
        }
    }

    if( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF3MFP_XT1_IN) != SYS_GPF_MFPL_PF3MFP_XT1_IN) { /* Check HXT_OUT pin */
        printf("\nGPF Pin 2 not set HXT_OUT !!!");
        printf("\nPlease Check GPF Pin 2 not connect crystal !!!");
    }

    if( (SYS->GPF_MFPL & SYS_GPF_MFPL_PF2MFP_XT1_OUT) != SYS_GPF_MFPL_PF2MFP_XT1_OUT) { /* Check HXT_IN pin */
        printf("\nGPF Pin 3 not set HXT_IN !!!");
        printf("\nPlease Check GPF Pin 3 not connect crystal !!!");
    }

    for(j = 0; j < 6; j++) {
        tGPIO =(GPIO_T*)((uint32_t)PA + (j * (0x40)));

        for(i = 0; i < 16; i++) {
            if((j == 4) & (i > 9)) // GPE
                break;

            if((j == 5) & (i > 5)) // GPF
                break;

            if(!(tGPIO->PIN & (1 << i))) { /* Check Pin status */
                gpio_error_count++;
                printf("\nGP%s Pin %d can not pull high!!", symbo_gpio[j], i);
            }
        }
    }

    if(gpio_error_count)
        printf("\nGPIO Check fail !!");
    else
        printf("\nGPIO Check pass");

    printf("\n");

    printf("\n/***** Clock Check: *****/");

    if(CLK->PWRCTL & CLK_PWRCTL_HXT_EN) /* Check HXT_EN */
        printf("\nHXT Enable");
    else
        printf("\nHXT Disable");

    if(CLK->PWRCTL & CLK_PWRCTL_LXT_EN) /* Check LXT_EN */
        printf("\nLXT Enable");
    else
        printf("\nLXT Disable");

    if(CLK->PWRCTL & CLK_PWRCTL_HIRC0_EN) /* Check HIRCO_EN */
        printf("\nHIRC Enable");
    else
        printf("\nHIRC Disable");

    if(CLK->PWRCTL & CLK_PWRCTL_LIRC_EN) /* Check LICR_EN */
        printf("\nLIRC Enable");
    else
        printf("\nLIRC Disable");

    printf("\n");
    printf("\n/***** Power Down setting check: *****/");
    if(CLK->PWRCTL & CLK_PWRCTL_PWRDOWN_EN) /* Check Power Down Enable */
        printf("\nChip Power-down mode Enable ");
    else
        printf("\nChip Power-down mode Disable !!!");

    if(SCB->SCR & 0x04) /* Check SCB configuration */
        printf("\nSCB->SCR set (0x%x) : ok", SCB->SCR);
    else
        printf("\nSCB->SCR set (0x%x) : error", SCB->SCR);

    //Restore NVIC->ISER setting
    NVIC->ISER[0] = Tmp_NVIC_ISER;

    printf("\n");
    printf("\n/***** Wake-up setting check: *****/");
    if(CLK->PWRCTL & CLK_PWRCTL_DELY_EN) /* Check Wake up delay */
        printf("\nWake-up Delay Counter Enable");
    else
        printf("\nWake-up Delay Counter Disable");

    if(CLK->PWRCTL & CLK_PWRCTL_WAKEINT_EN) /* Check Wake up interrupt */
        printf("\nPower-down Mode Wake-up Interrupt Enable");
    else
        printf("\nPower-down Mode Wake-up Interrupt Disable");

    if( NVIC->ISER[0] & (1 << ((uint32_t)(PDWU_IRQn) & 0x1F)) ) /* Check NVIC IRQ */
        printf("\nEnable PDWU_IRQn request for PDWU interrupt");
    else
        printf("\nDisable PDWU_IRQn request for PDWU interrupt");

    printf("\n");
    printf("\n/***** ISR check: *****/");
    if(NVIC_GetPendingIRQ(PDWU_IRQn)) /* Check Wake up interrupt pending */
        printf("\nPowerDown WakeUp Interrupt status is pending. (check fail)");
    else
        printf("\nPowerDown WakeUp Interrupt status is not pending. (check ok)");

    if(CLK->WKINTSTS & 0x1) /* Check Wake up status */
        printf("\nWake-up Interrupt Status is set. (check fail)");
    else
        printf("\nWake-up Interrupt Status is not set. (check ok)");

    printf("\n");
    printf("\n/***** PowerDown Check End *****/");
}

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

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN; // UART0 Clock Enable

    /* Select IP clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART0SEL_Msk;
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART0SEL_HIRC;// Clock source from HIRC

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD  */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk|SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |=  (SYS_GPB_MFPL_PB0MFP_UART0_RXD|SYS_GPB_MFPL_PB1MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();

}

/**
 *  @brief  Config UART0.
 *  @param  None
 *  @return None
 */
void UART0_Init()
{
    UART_Open(UART0, 115200); /* set UART baudrate is 115200 */
}

/**
  * @brief  Main routine.
  * @param  None.
  * @return None.
  */
int32_t main(void)
{
    /* Init System, IP clock and multi-function I/O */
    SYS_Init();
    /* Init UART0 for printf */
    UART0_Init();

    /* Prepare to enter power down mode, to demo the usage of PowerDown_Check(),
       this function does not actually put this system to  power down mode  */
    Enter_PowerDown();
    /* Check system state before enter power down mode */
    PowerDown_Check();

    while(1);
}



/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/



