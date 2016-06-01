/*---------------------------------------------------------------------------------------------------------*/
/* Winbond Electronics Corporation confidential                                                            */
/*                                                                                                         */
/* Copyright (c) 2007 by Winbond Electronics Corporation                                                   */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   timer_cunit_test.c                                                                                    */
/*            The test function of VIC for CUnit.                                                          */
/* Project:   DA8205                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Nano103.h"
#include "CUnit.h"
#include "Console.h"
#include "rtc_cunit.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

//#define D_msg   printf
#define D_msg(...)


extern unsigned int STATE0; 
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void) { return 0; }
int suite_success_clean(void) { return 0; }

void RTC_WaitAccessEnable(void)
{
	RTC->RWEN = RTC_WRITE_KEY;
    while(!(RTC->RWEN & RTC_RWEN_RWENF_Msk)) RTC->RWEN = RTC_WRITE_KEY;
	
	while(RTC->RWEN & RTC_RWEN_RTCBUSY_Msk);
}

/*---------------------------------------------------------------------------------------------------------*/
/* Function: function_name                                                                                 */
/*                                                                                                         */
/* Parameters:                                                                                             */
/*               param - [in/out], parameter_description                                                   */
/*               param - [in/out], parameter_description                                                   */
/*                                                                                                         */
/* Returns:      the_value                                                                                 */
/* Side effects: the_effect                                                                                */
/* Description:                                                                                            */
/*               description                                                                               */
/*---------------------------------------------------------------------------------------------------------*/

CU_SuiteInfo suites[] =
{
    {"RTC CONSTANT & MACRO", suite_success_init, suite_success_clean, NULL, NULL, RTC_CONSTANT_MACRO},
    {"RTC API", suite_success_init, suite_success_clean, NULL, NULL, RTC_API},
    CU_SUITE_INFO_NULL
};

void Timer0_Init(uint32_t u32ClkSrc)
{
    CLK->APBCLK |= CLK_APBCLK_TMR0CKEN_Msk;
    if(u32ClkSrc == CLK_CLKSEL1_TMR0SEL_LIRC) {        
        CLK->CLKSEL1 = (CLK->CLKSEL1&~CLK_CLKSEL1_TMR0SEL_Msk) | u32ClkSrc;
    } else {
        CLK->CLKSEL1 = (CLK->CLKSEL1&~CLK_CLKSEL1_TMR0SEL_Msk) | CLK_CLKSEL1_TMR0SEL_HXT;
    }
    
    TIMER0->CMP = 0xFFFFFF;
    if(u32ClkSrc == CLK_CLKSEL1_TMR0SEL_LIRC) {
        /* Enable LIRC */
        CLK->PWRCTL |= CLK_PWRCTL_LIRCEN_Msk;
        TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk;
    } else {
        TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk;
		TIMER0->PRECNT = (12 - 1);
    }    
}

void Timer0_ResetrCounter(void)
{
    TIMER0->CMP = 0xFFFFFF;
}

volatile uint32_t gu32TickINT = 0;
void RTC_IRQHandler(void)
{
    /* To check if RTC TICK interrupt occurred */
    if(RTC_GET_TICK_INT_FLAG() == 1) {
        /* Clear RTC TICK interrupt flag */
        RTC_CLEAR_TICK_INT_FLAG();
        gu32TickINT = 1;
    }

    /* To check if RTC snooper pin interrupt occurred */
    if(RTC_GET_TAMPER_FLAG() == 1) {
        /* Clear RTC snooper pin interrupt flag */
        RTC_CLEAR_TAMPER_FLAG();
    }
}

int32_t RTC_InitClock(void)
{    
    volatile uint32_t u32Timeout = SystemCoreClock/1000;
    
    Timer0_Init(CLK_CLKSEL1_TMR0SEL_HXT);
    
    /* Enable LXT */
    CLK->PWRCTL |= CLK_PWRCTL_LXTEN_Msk;

    /* Waiting for clock ready */
    while(u32Timeout-- > 0) {
        if((CLK->STATUS&CLK_STATUS_LXTSTB_Msk) == CLK_STATUS_LXTSTB_Msk) { 
            CLK->APBCLK |= CLK_APBCLK_RTCCKEN_Msk;
            return 0;
        }
    }    

    return -1;
}

const uint32_t au32TICKSel[] = {
    RTC_TICK_1_SEC,
    RTC_TICK_1_2_SEC,
    RTC_TICK_1_4_SEC,
    RTC_TICK_1_8_SEC,
    RTC_TICK_1_16_SEC,
    RTC_TICK_1_32_SEC,
    RTC_TICK_1_64_SEC,
    RTC_TICK_1_128_SEC,
};

const uint32_t au32WEEKDAYSel[] = {
    RTC_SUNDAY,
    RTC_MONDAY,
    RTC_TUESDAY,
    RTC_WEDNESDAY,
    RTC_THURSDAY,
    RTC_FRIDAY,
    RTC_SATURDAY,
};

const uint32_t au32SNPDetectSel[] = {
	RTC_SNOOPER_RISING,
    RTC_SNOOPER_FALLING,
};

const uint32_t au32SPRDataPattern[] = {
    0x55555555, 0xAAAAAAAA,
    0xFFFFFFFF, 0x00000000,
};

void CONSTANT_RTC(void)
{
    volatile uint32_t i = 0;
    volatile uint32_t u32Timeout;
    
    /* Init RTC clock */
    CU_ASSERT_EQUAL(RTC_InitClock(), 0);     

    /* Check RTC_INIT_KEY and RTC_WRITE_KEY Constant */
        u32Timeout = RTC_WAIT_COUNT/10000;
        RTC->INIT = RTC_INIT_KEY;
        while(RTC->INIT != RTC_INIT_INIT_ACTIVE_Msk) {
            if(u32Timeout-- == 0) {
                CU_FAIL("INIT FAIL");  
                return ;
            }
        }          
  
        u32Timeout = SystemCoreClock/1000;
        while((RTC->RWEN & RTC_RWEN_RWENF_Msk) != 0) {
            if(u32Timeout-- == 0) { 
                CU_FAIL("Default RWENF FAIL");  
                break;
            }    
        }            
        u32Timeout = SystemCoreClock/1000;
        RTC->RWEN = RTC_WRITE_KEY;
        while((RTC->RWEN & RTC_RWEN_RWENF_Msk) == 0x0) {
            if(u32Timeout-- == 0) {
                CU_FAIL("Enable WRENF FAIL");  
                break;
            }    
        }    

    /* Check RTC Tick Period Constant; RTC_SetTickPeriod()*/
        for(i=0; i<(sizeof(au32TICKSel)/sizeof(au32TICKSel[0])); i++) {
            D_msg("TICK Sel %d\n", i);
            RTC_SetTickPeriod(au32TICKSel[i]);
			while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
            RTC_WaitAccessEnable();
            CU_ASSERT_EQUAL(RTC->TICK, (i <<RTC_TICK_TICK_Pos));     
        }

    /* Check WEEKDAY Constant */
        for(i=0; i<(sizeof(au32WEEKDAYSel)/sizeof(au32WEEKDAYSel[0])); i++) {
            D_msg("WEEKDAY Sel %d\n", i);
            RTC_WaitAccessEnable();
            RTC->WEEKDAY = au32WEEKDAYSel[i] << RTC_WEEKDAY_WEEKDAY_Pos;
			while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
            RTC_WaitAccessEnable();
            CU_ASSERT_EQUAL(RTC->WEEKDAY, (i <<RTC_WEEKDAY_WEEKDAY_Pos));     
        }

    /* Check Snooper Detection Mode Constant;RTC_EnableSnooperDetection() */
        {
            //uint8_t u8DetectMode[] = {0, 2, 8, 0xA}; 
            for(i=0; i<(sizeof(au32SNPDetectSel)/sizeof(au32SNPDetectSel[0])); i++) {
                D_msg("SNP DetectMode Sel %d 0x%X\n", i, au32SNPDetectSel[i]);
                RTC_EnableTamperDetection(au32SNPDetectSel[i]);
                //while((RTC->SPRCTL&RTC_SPRCTL_SPRRWRDY_Msk) != RTC_SPRCTL_SPRRWRDY_Msk);
				while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
				RTC_WaitAccessEnable();
                CU_ASSERT_EQUAL((RTC->SPRCTL&RTC_SPRCTL_SNPTYPE0_Msk), (au32SNPDetectSel[i] << RTC_SPRCTL_SNPTYPE0_Pos));  
				CU_ASSERT_EQUAL((RTC->SPRCTL&RTC_SPRCTL_SNPDEN_Msk), (1 << RTC_SPRCTL_SNPDEN_Pos)); 				
            }
        }
                    
    /* Check RTC_DisableSnooperDetection() */
        RTC_DisableTamperDetection();
        //while(!(RTC->SPRCTL&RTC_SPRCTL_SPRRWRDY_Msk)) {};
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
		RTC_WaitAccessEnable();
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
        CU_ASSERT_EQUAL((RTC->SPRCTL&RTC_SPRCTL_SNPDEN_Msk), 0);     
}

void MACRO_RTC(void)
{
    volatile uint32_t i = 0, j = 0;
    volatile uint32_t u32Value, u32Timeout;
    S_RTC_TIME_DATA_T sWriteRTC, sReadRTC;
    
    /* Init RTC clock */
    CU_ASSERT_EQUAL(RTC_InitClock(), 0);     

    u32Timeout = SystemCoreClock/1000;
    RTC->INIT = RTC_INIT_KEY;
    while(RTC->INIT != RTC_INIT_INIT_ACTIVE_Msk) {
        if(u32Timeout-- == 0) {
            CU_FAIL("INIT FAIL");  
            return ;
        }
    }    
        
    /* Check RTC_IS_LEAP_YEAR Macro; RTC_SetDate() */
        RTC_SetDate(2016, 1, 24, RTC_SUNDAY);
        RTC_WaitAccessEnable();
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
        CU_ASSERT_EQUAL(RTC->LEAPYEAR, 1);     
        RTC_SetDate(2015, 1, 24, RTC_SATURDAY);
        RTC_WaitAccessEnable();
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
        CU_ASSERT_EQUAL(RTC->LEAPYEAR, 0);

    /* Check RTC_GET_TICK_INT_FLAG and RTC_CLEAR_TICK_INT_FLAG Macro; RTC_SetTickPeriod()*/        
        u32Timeout = SystemCoreClock/100;
        RTC_SetTickPeriod(RTC_TICK_1_128_SEC);
        while((RTC->RWEN & RTC_RWEN_RWENF_Msk) == RTC_RWEN_RWENF_Msk); /* Wait RWENF clear to 0 */
        while(RTC_GET_TICK_INT_FLAG() == 0) {
            if(u32Timeout-- == 0) {
                CU_FAIL("RTC_GET_TICK_INT_FLAG FAIL");
                return ;
            }                
                
        }
        RTC_CLEAR_TICK_INT_FLAG();
        while(RTC_GET_TICK_INT_FLAG() == 0);
        RTC_CLEAR_TICK_INT_FLAG();
        Timer0_ResetrCounter();
        
        CU_ASSERT_EQUAL(RTC_GET_TICK_INT_FLAG(), 0); 
        u32Timeout = SystemCoreClock/100;
        while(RTC_GET_TICK_INT_FLAG() == 0) {
            if(u32Timeout-- == 0) {
                CU_FAIL("RTC_GET_TICK_INT_FLAG FAIL");
                return ;
            }                
                
        }
        u32Value = TIMER0->CNT;
        D_msg("CNT: %d\n", u32Value);
        if((u32Value > (7812+50)) || (u32Value < (7812-50))) {
            CU_FAIL("TICK Period FAIL");
            return ;
        }
        RTC_WaitAccessEnable();
        RTC->TICK = RTC_TICK_1_SEC << RTC_TICK_TICK_Pos;
   
    /* Check RTC_GET_ALARM_INT_FLAG and RTC_CLEAR_ALARM_INT_FLAG Macro; RTC_SetAlarmDateAndTime(), RTC_GetAlarmDateAndTime() */
        u32Timeout = SystemCoreClock/2;
        RTC_CLEAR_ALARM_INT_FLAG();
        RTC->CALM = 0x00140301;
        RTC->TALM = 0x00020102;
        sWriteRTC.u32Year       = 2014;
        sWriteRTC.u32Month      = 3;
        sWriteRTC.u32Day        = 1;
        sWriteRTC.u32Hour       = 2;
        sWriteRTC.u32Minute     = 1;
        sWriteRTC.u32Second     = 2;
        sWriteRTC.u32TimeScale  = RTC_CLOCK_24;
        RTC_SetAlarmDateAndTime(&sWriteRTC);                
        RTC_WaitAccessEnable();
        //Timer0_ResetrCounter();
        RTC->CAL  = 0x00140301;
        RTC->TIME = 0x00020101;
		
		while(RTC->TIME != 0x00020101);
		Timer0_ResetrCounter();
		
        while(RTC_GET_ALARM_INT_FLAG() == 0) {
            if(u32Timeout-- == 0) {
                CU_FAIL("RTC_GET_ALARM_INT_FLAG FAIL");
                return ;
            }                
                
        }
		u32Value = TIMER0->CNT;
        RTC_CLEAR_ALARM_INT_FLAG();
        CU_ASSERT_EQUAL(RTC_GET_ALARM_INT_FLAG(), 0); 
        D_msg("CNT: %d\n", u32Value);
        if((u32Value > (1000000+10000)) || (u32Value < (1000000-10000))) {
            CU_FAIL("ALARM Period FAIL");
            return ;
        }
        RTC_WaitAccessEnable();
        CU_ASSERT_EQUAL(RTC->CALM, 0x00140301);
        CU_ASSERT_EQUAL(RTC->TALM, 0x00020102);
        RTC_GetAlarmDateAndTime(&sReadRTC);
        CU_ASSERT_EQUAL(sReadRTC.u32Year,   2014);
        CU_ASSERT_EQUAL(sReadRTC.u32Month,  3);
        CU_ASSERT_EQUAL(sReadRTC.u32Day,    1);
        CU_ASSERT_EQUAL(sReadRTC.u32Hour,   2);
        CU_ASSERT_EQUAL(sReadRTC.u32Minute, 1);
        CU_ASSERT_EQUAL(sReadRTC.u32Second, 2);
        CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_24);
        D_msg("%d/%02d/%02d %02d:%02d:%02d\n",
                sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);              
        
    /* Check RTC_WRITE_SPARE_REGISTER and RTC_READ_SPARE_REGISTER Macro; RTC_EnableSpareAccess(), RTC_DisableSpareRegister() */
        RTC_EnableSpareAccess();
        CU_ASSERT_EQUAL(RTC->SPRCTL, (RTC->SPRCTL|RTC_SPRCTL_SPRRWEN_Msk));     
        for(i=0; i<(sizeof(au32SPRDataPattern)/sizeof(au32SPRDataPattern[0])); i++) { 
            for(j=0; j<5; j++) {
                RTC_WaitAccessEnable();
                RTC_WRITE_SPARE_REGISTER(j, au32SPRDataPattern[i]);
				//while((RTC->SPRCTL&RTC_SPRCTL_SPRRDY_Msk) == 0); /* Wait RTC_SPRCTL_SPRRDY_Msk change to 1 */
                CU_ASSERT_EQUAL(RTC_READ_SPARE_REGISTER(j), au32SPRDataPattern[i]);     
            }
        }
        		
}

void API_RTC_DataTime_Func(void) 
{
    volatile uint32_t i = 0, j = 0;
    volatile uint32_t u32Value, u32Timeout;
    S_RTC_TIME_DATA_T sWriteRTC, sReadRTC;
    
    /* Init RTC clock */
    CU_ASSERT_EQUAL(RTC_InitClock(), 0);     

    u32Timeout = SystemCoreClock/1000;
    RTC->INIT = RTC_INIT_KEY;
    while(RTC->INIT != RTC_INIT_INIT_ACTIVE_Msk) {
        if(u32Timeout-- == 0) {
            CU_FAIL("INIT FAIL");  
            return ;
        }
    }   

    /* Date/Time 1: 24-Hour */
    /* Check RTC_Open(); Open RTC and start counting */
        sWriteRTC.u32Year       = 2014;
        sWriteRTC.u32Month      = 5;
        sWriteRTC.u32Day        = 15;
        sWriteRTC.u32DayOfWeek  = RTC_THURSDAY;
        sWriteRTC.u32Hour       = 13;
        sWriteRTC.u32Minute     = 14;
        sWriteRTC.u32Second     = 5;
        sWriteRTC.u32TimeScale  = RTC_CLOCK_24;
        RTC_Open(&sWriteRTC);
        //RTC_WaitAccessEnable();  
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
        /* Check RTC_GetDateAndTime(); Read current RTC date/time */
        RTC_GetDateAndTime(&sReadRTC);
        CU_ASSERT_EQUAL(sReadRTC.u32Year,   2014);
        CU_ASSERT_EQUAL(sReadRTC.u32Month,  5);
        CU_ASSERT_EQUAL(sReadRTC.u32Day,    15);
        CU_ASSERT_EQUAL(sReadRTC.u32Hour,   13);
        CU_ASSERT_EQUAL(sReadRTC.u32Minute, 14);
        CU_ASSERT_EQUAL(sReadRTC.u32Second, 5);
        CU_ASSERT_EQUAL(sReadRTC.u32DayOfWeek, RTC_THURSDAY);
        CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_24);  
        /* Check RTC_GetDayOfWeek(), RTC_IS_LEAP_YEAR() */
        CU_ASSERT_EQUAL(RTC_GetDayOfWeek(), RTC_THURSDAY);
        CU_ASSERT_EQUAL(RTC_IS_LEAP_YEAR(), 0);        
        D_msg("%d/%02d/%02d %02d:%02d:%02d\n",
                sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);
        
    /* Date/Time 2: 12-Hour, PM */
    /* Check RTC_Open(); Open RTC and start counting */
        sWriteRTC.u32Year       = 2016;
        sWriteRTC.u32Month      = 1;
        sWriteRTC.u32Day        = 24;
        sWriteRTC.u32DayOfWeek  = RTC_SUNDAY;
        sWriteRTC.u32Hour       = 14;
        sWriteRTC.u32Minute     = 24;
        sWriteRTC.u32Second     = 0;
        sWriteRTC.u32TimeScale  = RTC_CLOCK_12;
        sWriteRTC.u32AmPm       = RTC_PM;
        RTC_Open(&sWriteRTC);
        //RTC_WaitAccessEnable();
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);		
        /* Check RTC_GetDateAndTime(); Read current RTC date/time */
        RTC_GetDateAndTime(&sReadRTC);
        CU_ASSERT_EQUAL(sReadRTC.u32Year,   2016);
        CU_ASSERT_EQUAL(sReadRTC.u32Month,  1);
        CU_ASSERT_EQUAL(sReadRTC.u32Day,    24);
        CU_ASSERT_EQUAL(sReadRTC.u32Hour,   14);
        CU_ASSERT_EQUAL(sReadRTC.u32Minute, 24);
        CU_ASSERT_EQUAL(sReadRTC.u32Second, 0);
        CU_ASSERT_EQUAL(sReadRTC.u32DayOfWeek, RTC_SUNDAY);
        CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_12);    
        /* Check RTC_GetDayOfWeek(), RTC_IS_LEAP_YEAR() */
        CU_ASSERT_EQUAL(RTC_GetDayOfWeek(), RTC_SUNDAY);
        CU_ASSERT_EQUAL(RTC_IS_LEAP_YEAR(), 1);        
        CU_ASSERT_EQUAL((RTC->TIME&BIT21), BIT21);        
        D_msg("%d/%02d/%02d %02d:%02d:%02d\n",
                sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);              

    /* Date/Time 3: 12-Hour, AM */
    /* Check RTC_Open(); Open RTC and start counting */
        sWriteRTC.u32Year       = 2016;
        sWriteRTC.u32Month      = 2;
        sWriteRTC.u32Day        = 28;
        sWriteRTC.u32DayOfWeek  = RTC_SUNDAY;
        sWriteRTC.u32Hour       = 3;
        sWriteRTC.u32Minute     = 30;
        sWriteRTC.u32Second     = 10;
        sWriteRTC.u32TimeScale  = RTC_CLOCK_12;
        sWriteRTC.u32AmPm       = RTC_AM;
        RTC_Open(&sWriteRTC);
        //RTC_WaitAccessEnable();  
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);		
        /* Check RTC_GetDateAndTime(); Read current RTC date/time */
        RTC_GetDateAndTime(&sReadRTC);
        CU_ASSERT_EQUAL(sReadRTC.u32Year,   2016);
        CU_ASSERT_EQUAL(sReadRTC.u32Month,  2);
        CU_ASSERT_EQUAL(sReadRTC.u32Day,    28);
        CU_ASSERT_EQUAL(sReadRTC.u32Hour,   3);
        CU_ASSERT_EQUAL(sReadRTC.u32Minute, 30);
        CU_ASSERT_EQUAL(sReadRTC.u32Second, 10);
        CU_ASSERT_EQUAL(sReadRTC.u32DayOfWeek, RTC_SUNDAY);
        CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_12);    
        /* Check RTC_GetDayOfWeek(), RTC_IS_LEAP_YEAR() */
        CU_ASSERT_EQUAL(RTC_GetDayOfWeek(), RTC_SUNDAY);
        CU_ASSERT_EQUAL(RTC_IS_LEAP_YEAR(), 1);        
        CU_ASSERT_EQUAL((RTC->TIME&BIT21), 0);        
        D_msg("%d/%02d/%02d %02d:%02d:%02d\n",
                sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);              

    /* Check RTC_SetAlarmDate(), RTC_SetAlarmTime() */
        RTC_SetAlarmDate(2014, 3, 29);
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
        RTC_SetAlarmTime(5, 6, 7, RTC_CLOCK_24, RTC_AM);
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
        RTC_GetAlarmDateAndTime(&sReadRTC);
        CU_ASSERT_EQUAL(sReadRTC.u32Year,   2014);
        CU_ASSERT_EQUAL(sReadRTC.u32Month,  3);
        CU_ASSERT_EQUAL(sReadRTC.u32Day,    29);
        CU_ASSERT_EQUAL(sReadRTC.u32Hour,   5);
        CU_ASSERT_EQUAL(sReadRTC.u32Minute, 6);
        CU_ASSERT_EQUAL(sReadRTC.u32Second, 7);
        CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_24);
        D_msg("%d/%02d/%02d %02d:%02d:%02d\n",
                sReadRTC.u32Year, sReadRTC.u32Month, sReadRTC.u32Day, sReadRTC.u32Hour, sReadRTC.u32Minute, sReadRTC.u32Second);              

    /* Check RTC_EnableInt(), RTC_DisableInt() */
        RTC_EnableInt(RTC_INTEN_ALMIEN_Msk);
        CU_ASSERT_EQUAL(RTC->INTEN, RTC_INTEN_ALMIEN_Msk);
        RTC_DisableInt(RTC_INTEN_ALMIEN_Msk);
        CU_ASSERT_EQUAL(RTC->INTEN, 0);
        RTC_EnableInt(RTC_INTEN_TICKIEN_Msk);
        CU_ASSERT_EQUAL(RTC->INTEN, RTC_INTEN_TICKIEN_Msk);
        RTC_DisableInt(RTC_INTEN_TICKIEN_Msk);
        CU_ASSERT_EQUAL(RTC->INTEN, 0);
        RTC_EnableInt(RTC_INTEN_SNPDIEN_Msk);
        CU_ASSERT_EQUAL(RTC->INTEN, RTC_INTEN_SNPDIEN_Msk);
        RTC_DisableInt(RTC_INTEN_SNPDIEN_Msk);
        CU_ASSERT_EQUAL(RTC->INTEN, 0);

    /* Check RTC_SetTime() */
        RTC_SetTime(13, 5, 6, RTC_CLOCK_24, RTC_PM);
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
        RTC_WaitAccessEnable();   
        RTC_GetDateAndTime(&sReadRTC);
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
        CU_ASSERT_EQUAL(sReadRTC.u32Hour,   13);
        CU_ASSERT_EQUAL(sReadRTC.u32Minute, 5);
        CU_ASSERT_EQUAL(sReadRTC.u32Second, 6);
        CU_ASSERT_EQUAL(sReadRTC.u32TimeScale, RTC_CLOCK_24);
    
    /* Check RTC_32KCalibration() */
        RTC_WaitAccessEnable();   
        RTC_32KCalibration(327650000);
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
        RTC_WaitAccessEnable();   
        CU_ASSERT_EQUAL(RTC->FREQADJ, 0x2000C0);
        RTC_WaitAccessEnable();   
        RTC_32KCalibration(327700000);
		while(RTC->RWEN & RTC_RWEN_RWENF_Msk);
        RTC_WaitAccessEnable();   
        CU_ASSERT_EQUAL(RTC->FREQADJ, 0x1FFF80);
	
    /* Check RTC Interrupt behavior */
        NVIC_EnableIRQ(RTC_IRQn);
        RTC_WaitAccessEnable();   
        RTC_EnableInt(RTC_INTEN_TICKIEN_Msk);
        RTC->TICK = RTC_TICK_1_128_SEC;
        u32Timeout = SystemCoreClock/1000;
        while(gu32TickINT == 0) {
            if(u32Timeout-- == 0) {
                CU_FAIL("TICK INT FAIL");  
                RTC_Close();
                return ;
            }
        }   
        RTC_DisableInt(RTC_INTEN_TICKIEN_Msk);
        /* Disable RTC NVIC */
        NVIC_DisableIRQ(RTC_IRQn);
   	
    RTC_Close();
}

CU_TestInfo RTC_CONSTANT_MACRO[] =
{
    //{"Check RTC CONSTANT ", CONSTANT_RTC}, 
    //{"Check RTC MACRO ",    MACRO_RTC}, 
    CU_TEST_INFO_NULL
};

CU_TestInfo RTC_API[] =
{
    {"Check RTC Data, Time APIs ",   API_RTC_DataTime_Func}, 
    CU_TEST_INFO_NULL
};
