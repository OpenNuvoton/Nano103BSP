/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 3 $
 * $Date: 16/04/01 9:23a $
 * @brief    Demonstrate how to wake up system periodically with RTC interrupt.
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
#include "rtc.h"

#define __DEBUG_MSG

#ifdef __DEBUG_MSG
#define DEBUG_MSG   printf
#else
#define DEBUG_MSG(...)
#endif

/* Power down current of this sample code is around 2.2 uA */

/* External functions */
void planNextRTCInterrupt(S_RTC_TIME_DATA_T *sCurTime);

/* Global variables */
__IO int32_t   _Wakeup_Flag = 0;    /* 1 indicates system wake up from power down mode */
__IO uint32_t  _Pin_Setting[11];    /* store Px_H_MFP and Px_L_MFP */
__IO uint32_t  _PullUp_Setting[6];  /* store GPIOx_PUEN */

void Leave_PowerDown(void);

/**
  * @brief  Plane next RTC alarm interrupt.
  * @param  None.
  * @return None.
  */
void planNextRTCInterrupt(S_RTC_TIME_DATA_T *sCurTime)
{
    // plan next interrupt timing
    if(sCurTime->u32Minute < 59)
        sCurTime->u32Minute += 1;
    else
    {
        if(sCurTime->u32Hour < 23)
            sCurTime->u32Hour += 1;
        else    // next day
        {
            sCurTime->u32Hour = 0;

            // new year first day
            if(sCurTime->u32Month==12 && sCurTime->u32Day==31)
            {
                sCurTime->u32Year += 1;
                sCurTime->u32Month = 1;
                sCurTime->u32Day = 1;
            }
            else if(sCurTime->u32Month==1 ||
                    sCurTime->u32Month==3 ||
                    sCurTime->u32Month==5 ||
                    sCurTime->u32Month==7 ||
                    sCurTime->u32Month==8 ||
                    sCurTime->u32Month==10 ||
                    sCurTime->u32Month==12)   // 1,3,5,7,8,10,12 31-day month
            {
                if(sCurTime->u32Day < 31)
                    sCurTime->u32Day += 1;
                else
                {
                    sCurTime->u32Day = 1;
                    sCurTime->u32Month += 1;
                }
            }
            else if(sCurTime->u32Month==2)     // 2, 28 or 29-day month
            {
                if(RTC_IS_LEAP_YEAR())   // leap year
                {
                    if(sCurTime->u32Day < 29)
                        sCurTime->u32Day += 1;
                    else
                    {
                        sCurTime->u32Day = 1;
                        sCurTime->u32Month += 1;
                    }
                }
                else
                {
                    if(sCurTime->u32Day < 28)
                        sCurTime->u32Day += 1;
                    else
                    {
                        sCurTime->u32Day = 1;
                        sCurTime->u32Month += 1;
                    }
                }
            }
            else if(sCurTime->u32Month==4 ||
                    sCurTime->u32Month==6 ||
                    sCurTime->u32Month==9 ||
                    sCurTime->u32Month==11)   // 4,6,9,11 30-day
            {
                if(sCurTime->u32Day < 30)
                    sCurTime->u32Day += 1;
                else
                {
                    sCurTime->u32Day = 1;
                    sCurTime->u32Month += 1;
                }
            }
        }
    }
    sCurTime->u32Second = 0;

    RTC_SetAlarmDateAndTime(sCurTime); // Set alarm time
    RTC_EnableInt(RTC_INTEN_ALMIEN_Msk); // Enable alarm interrupt
    NVIC_EnableIRQ(RTC_IRQn); // Config NVIV

}

/**
  * @brief  PDWU IRQHandler.
  * @param  None.
  * @return None.
  */
void PDWU_IRQHandler()
{
    DEBUG_MSG("PDWU_IRQHandler running...\n");
    CLK->WKINTSTS = 1; /* clear interrupt status */
    Leave_PowerDown(); // Restore pin config and clock setting
    _Wakeup_Flag = 1;
}


/**
  * @brief  RTC IRQHandler.
  * @param  None.
  * @return None.
  */
void RTC_IRQHandler()
{
    S_RTC_TIME_DATA_T sCurTime;

    /* RTC Alarm interrupt */
    if ((RTC->INTEN & RTC_INTEN_ALMIEN_Msk) && (RTC->INTSTS & RTC_INTSTS_ALMIF_Msk))   // Check Alarm interrupt
    {
        DEBUG_MSG("RTC Alarm Interrupt.\n");
        RTC->INTSTS = RTC_INTSTS_ALMIF_Msk;

        RTC_GetDateAndTime(&sCurTime);
        DEBUG_MSG("Current Time:%d/%02d/%02d %02d:%02d:%02d\n",sCurTime.u32Year,sCurTime.u32Month,sCurTime.u32Day,sCurTime.u32Hour,sCurTime.u32Minute,sCurTime.u32Second);

        planNextRTCInterrupt(&sCurTime); // Plane next RTC alarm time
    }

}

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
    _Pin_Setting[9] = SYS->GPD_MFPH;
    _Pin_Setting[10] = SYS->GPF_MFPL;

    /* Save Pull-up setting */
    _PullUp_Setting[0] =  PA->PUEN;
    _PullUp_Setting[1] =  PB->PUEN;
    _PullUp_Setting[2] =  PC->PUEN;
    _PullUp_Setting[3] =  PD->PUEN;
    _PullUp_Setting[4] =  PE->PUEN;
    _PullUp_Setting[5] =  PF->PUEN;
}


/**
  * @brief  Restore original setting of multi-function pin selection.
  * @param  None.
  * @return None.
  */
void RestorePinSetting()
{
    /* Restore Pin selection setting */
    SYS->GPA_MFPL = _Pin_Setting[0];
    SYS->GPA_MFPH = _Pin_Setting[1];
    SYS->GPB_MFPL = _Pin_Setting[2];
    SYS->GPB_MFPH = _Pin_Setting[3];
    SYS->GPC_MFPL = _Pin_Setting[4];
    SYS->GPC_MFPH = _Pin_Setting[5];
    SYS->GPD_MFPL = _Pin_Setting[6];
    SYS->GPD_MFPH = _Pin_Setting[7];
    SYS->GPE_MFPL = _Pin_Setting[8];
    SYS->GPD_MFPH = _Pin_Setting[9];
    SYS->GPF_MFPL = _Pin_Setting[10];

    /* Restore Pull-up setting */
    PA->PUEN = _PullUp_Setting[0];
    PB->PUEN = _PullUp_Setting[1];
    PC->PUEN = _PullUp_Setting[2];
    PD->PUEN = _PullUp_Setting[3];
    PE->PUEN = _PullUp_Setting[4];
    PF->PUEN = _PullUp_Setting[5];
}

/**
  * @brief  This function will not be called when system wakeup.
  * @param  None.
  * @return None.
  */
void Leave_PowerDown()
{
    RestorePinSetting(); /* Restore pin setting */

    /* Enable clock */
    CLK->PWRCTL |= CLK_PWRCTL_LXTEN_Msk; /* enable LXT - 32Khz */
    CLK->PWRCTL |= CLK_PWRCTL_LIRCEN_Msk; /* enable LIRC - 10Khz */

    SYS_LockReg();
}

/**
  * @brief  Save multi-function pin setting and then go to power down.
  * @param  None.
  * @return None.
  */
void Enter_PowerDown()
{
    /* Back up original setting */
    SavePinSetting();

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
    SYS->GPD_MFPH = 0;
    SYS->GPF_MFPL = 0x77007700; // PF6:X32O, PF7:X32I, PF2:XT1_OUT, PF3:XT1_IN

    /* Enable GPIO pull up */
    PA->PUEN = 0xFFFF;
    PB->PUEN = 0xFFFF;
    PC->PUEN = 0xFFFF;
    PD->PUEN = 0xFFFF;
    PE->PUEN = 0xFFFF;
    PF->PUEN = 0x0033;      /* exclude GPF2 and GPF3 which are HXT OUT/IN */ /* exclude GPF6 and GPF7 which are LXT OUT/IN */

    SYS_UnlockReg();

    /* Disable clock */
    //CLK->PWRCTL &= ~CLK_PWRCTL_LXTEN_Msk; /* disable LXT - 32Khz */
    CLK->PWRCTL &= ~CLK_PWRCTL_LIRCEN_Msk;  /* disable LIRC - 10KHz */
    CLK->PWRCTL |= CLK_PWRCTL_PDWKIEN_Msk; /* Enable wake up interrupt source */
    NVIC_EnableIRQ(PDWU_IRQn);              /* Enable IRQ request for PDWU interrupt */

    CLK_PowerDown(); // Enter Power Down Mode
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

    /* Enable External XTAL (4~24 MHz) */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk; // XTAL12M (HXT) Enabled

    /* Enable External LXT (32 KHz) */
    CLK->PWRCTL |= CLK_PWRCTL_LXTEN_Msk;

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to XTAL */
    CLK->CLKSEL0 = (CLK->CLKSEL0 &~ CLK_CLKSEL0_HCLKSEL_Msk) | (CLK_CLKSEL0_HCLKSEL_HXT);

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_UART0CKEN_Msk; // UART0 Clock Enable
    CLK->APBCLK |= CLK_APBCLK_RTCCKEN_Msk; // RTC Clock Enable

    /* Select IP clock source */
    CLK->CLKSEL1 &= ~CLK_CLKSEL1_UART0SEL_Msk;
    CLK->CLKSEL1 |= (0x0 << CLK_CLKSEL1_UART0SEL_Pos);// Clock source from external 12 MHz crystal clock

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
    S_RTC_TIME_DATA_T sCurTime;

    SYS_Init();
    UART0_Init();

    /* Time Setting */
    sCurTime.u32Year       = 2016; // Set Year
    sCurTime.u32Month      = 3;   // Set Month
    sCurTime.u32Day        = 1;   // Set Day
    sCurTime.u32Hour       = 0;    // Set Hour
    sCurTime.u32Minute     = 0;    // Set Minute
    sCurTime.u32Second     = 0;    // Set Second
    sCurTime.u32DayOfWeek  = RTC_TUESDAY;  // Set Day of week
    sCurTime.u32TimeScale  = RTC_CLOCK_24; // Set Time Scale

    RTC_Open(&sCurTime);  //user maybe need Waiting for RTC settings stable

    RTC_GetDateAndTime(&sCurTime); /* Read current RTC time */
    DEBUG_MSG("Current Time:%d/%02d/%02d %02d:%02d:%02d\n",sCurTime.u32Year,sCurTime.u32Month,sCurTime.u32Day,sCurTime.u32Hour,sCurTime.u32Minute,sCurTime.u32Second);

    /* Enable RTC alarm for 1 minute to update RTC time */
    planNextRTCInterrupt(&sCurTime);

    while(1)
    {
        DEBUG_MSG("Going to Power Down...\n");
        while(!(UART0->FIFOSTS & UART_FIFOSTS_TXENDF_Msk)) ;  /* waits for message send out */

        Enter_PowerDown();  /* Enter power down mode */

        DEBUG_MSG("Program resume...\n");

        if (_Wakeup_Flag == 1)
        {
            _Wakeup_Flag = 0;

            DEBUG_MSG("*** WAKE UP ***\n");
        }
    };


}


/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/



