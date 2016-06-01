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
#include "Nano103.h"
#include "CUnit.h"
#include "Console.h"
#include "wdt_cunit.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

extern unsigned int STATE0; 
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void) { return 0; }
int suite_success_clean(void) { return 0; }

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
    {"WDT MACRO", suite_success_init, suite_success_clean, NULL, NULL, WDT_MACRO},
    {"WDT API", suite_success_init, suite_success_clean, NULL, NULL, WDT_API},
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
        TIMER0->PRECNT  = 0;
        TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk;
    } else {
        TIMER0->PRECNT = 12 - 1;
        TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk;
    }    
}

void Timer0_ResetCounter(void)
{
    TIMER0->CMP = 0xFFFFFF;
}

volatile uint8_t gu8IntFlag, gu8WakeupFlag;
volatile uint32_t gu32TDRTicks;
volatile uint32_t gu32RegWDTCTL, gu32RegWDTISR;
void WDT_IRQHandler(void)
{
    gu32RegWDTCTL = WDT->CTL;
    gu32RegWDTISR = WDT->STATUS;
    if(WDT_GET_TIMEOUT_INT_FLAG() == 1) {
        
        gu32TDRTicks = TIMER0->CNT;

        gu8IntFlag = 1;
        
        /* Clear WDT time-out interrupt flag */
        WDT_CLEAR_TIMEOUT_INT_FLAG();
        
        WDT_RESET_COUNTER();
        
        CU_ASSERT((WDT->STATUS & WDT_STATUS_WDTIF_Msk) == 0);     
    }
    
    if(WDT_GET_TIMEOUT_WAKEUP_FLAG() == 1) {
        
         gu8WakeupFlag = 1;

        /* Clear WDT time-out wake-up flag */
        WDT_CLEAR_TIMEOUT_WAKEUP_FLAG();       
    }
}

int32_t WDT_InitClock(void)
{
    volatile uint32_t u32Timeout = SystemCoreClock/1000;
    
    /* Enable LIRC */
    CLK->PWRCTL |= CLK_PWRCTL_LIRCEN_Msk;

    /* Waiting for clock ready */
    while(u32Timeout-- > 0) {
        if((CLK->STATUS & CLK_STATUS_LIRCSTB_Msk) == CLK_STATUS_LIRCSTB_Msk) {
            CLK->APBCLK |= CLK_APBCLK_WDT_EN;
            
            if(WDT_GET_RESET_FLAG())
                return 0;
            
            WDT->CTL |= WDT->CTL;
            WDT->CTL = 0;
            return 0;
        }
    }
    
    return -1;
}

const uint32_t au32TOUTSel[] = {
    WDT_TIMEOUT_2POW4,
    WDT_TIMEOUT_2POW6,
    WDT_TIMEOUT_2POW8,
    WDT_TIMEOUT_2POW10,
    WDT_TIMEOUT_2POW12,
    WDT_TIMEOUT_2POW14,
    WDT_TIMEOUT_2POW16,
    WDT_TIMEOUT_2POW18,
};

const uint32_t au32RDSel[] = {
    WDT_RESET_DELAY_1026CLK,
    WDT_RESET_DELAY_130CLK,
    WDT_RESET_DELAY_18CLK,
    WDT_RESET_DELAY_3CLK,
};

void CONSTANT_WDT_SETTING(void) 
{
    volatile uint32_t i, u32Timeout = 0;
    volatile uint32_t u32RegWDTCTL, u32RegWDTALT;
    
    if(WDT_InitClock() != 0) {
        CU_FAIL("WDT Init FAIL");  
        return ;
    }
       
    /* Select time-out interval; enable interrupt; enable reset; enable wake-up */
    for(i=0; i<sizeof(au32TOUTSel)/4; i++) { 
//        printf("TOUTSel %d\n", i);        
        WDT_Open(au32TOUTSel[i], NULL, TRUE, TRUE);
        WDT_EnableInt();
        
        CU_ASSERT_EQUAL(WDT->CTL, ((i<<4)|0xE));     
        if(WDT->CTL != ((i<<4)|0xE)) {
            WDT_Close();
            return ;
        }
    }
    WDT_Close();
    /* Select reset delay period; disable interrupt; disable reset; disable wake-up */
    for(i=0; i<sizeof(au32RDSel)/4; i++) {

        WDT_Open(WDT_TIMEOUT_2POW14, au32RDSel[i], FALSE, FALSE);

        CU_ASSERT_EQUAL(WDT->CTL, (i << 8) | 0x58);   
    }
    
    WDT_Close();
}

void API_WDT_Interrupt(void) 
{
    volatile uint32_t i, u32Timeout = 0;
    volatile uint32_t u32RegWDTCTL, u32RegWDTALT;
    
    if(WDT_InitClock() != 0) {
        CU_FAIL("WDT Init FAIL");  
        return ;
    }
     
    /* Check WDT_EnableInt */
    //WDT->CTL = 0x700;
    WDT_EnableInt();
    CU_ASSERT_EQUAL(WDT->INTEN, 0x1);     
    if(WDT->INTEN != 0x1) {
        WDT_Close();
        return ;
    }
    
    /* Check WDT_DisableInt */
    WDT_DisableInt();
    CU_ASSERT_EQUAL(WDT->INTEN, 0);     
    if(WDT->INTEN != 0) {
        WDT_Close();
        return ;
    }
            
    WDT_Close();
}

void API_WDT_Open(void) 
{
    volatile uint32_t u32Timeout = 0;
    
    if(WDT_InitClock() != 0) {
        CU_FAIL("WDT Init FAIL");  
        return ;
    }
    
    /* Enable WDT NVIC */
    NVIC_EnableIRQ(WDT_IRQn);

    Timer0_Init(CLK_CLKSEL1_TMR0SEL_HXT);
    Timer0_ResetCounter();
    
    /* WDT time-out period is around 25.6 ms */
    WDT_Open(WDT_TIMEOUT_2POW8, NULL, TRUE, TRUE);
    WDT_EnableInt();

    gu8IntFlag = gu8WakeupFlag = 0;
    while(gu8IntFlag == 0) {
        if((TIMER0->CNT > __HXT/12) || (u32Timeout > SystemCoreClock/20)) {
            CU_FAIL("API_WDT_Open fail 01");
            WDT_Close();
            return ;
        }         
        
        u32Timeout++;
    }
    
    if((gu32TDRTicks > (25600+12000)) || (gu32TDRTicks < (25600-12000)) || (WDT->CTL != 0x2E) || (gu32RegWDTISR != 0x1)) {
            CU_FAIL("API_WDT_Open fail 02");
            WDT_Close();
    }
        
    WDT_Close();
}

void API_WDT_Wakeup(void) 
{
    volatile uint32_t u32Timeout = 0;
    
    if(WDT_InitClock() != 0) {
        CU_FAIL("WDT Init FAIL");  
        return ;
    }
    
    /* Enable WDT NVIC */
    NVIC_EnableIRQ(WDT_IRQn);

    Timer0_Init(CLK_CLKSEL1_TMR0SEL_LIRC);
    Timer0_ResetCounter();
    
    /* WDT time-out period is around 25.6 ms */
    WDT_Open(WDT_TIMEOUT_2POW8, NULL, TRUE, TRUE);
    WDT_EnableInt();

    gu8IntFlag = gu8WakeupFlag = 0;
    
    /* Enter to power-down */
    {
        UART_WAIT_TX_EMPTY(UART0);
        
        SCB->SCR = 4;

        /* To program PWRCON register, it needs to disable register protection first. */
        CLK->PWRCTL |= CLK_PWRCTL_PDEN_Msk | CLK_PWRCTL_PDWKIEN_Msk | CLK_PWRCTL_PDWKDLY_Msk;

        __WFI();
    }
    
    while((gu8IntFlag == 0) || (gu8WakeupFlag == 0)) {
        if(u32Timeout > SystemCoreClock/20) {
            CU_FAIL("API_WDT_Wakeup fail 01");
            WDT_Close();
            return ;
        }         
        
        u32Timeout++;
    }

    gu32TDRTicks = gu32TDRTicks*100; // us
    //if((gu32TDRTicks > (25600+12000)) || (gu32TDRTicks < (25600-12000)) || (WDT->CTL != 0x2E) || (gu32RegWDTISR != 0x5)) {
    if((WDT->CTL != 0x2E) || (gu32RegWDTISR != 0x5)) {
        CU_FAIL("API_WDT_Wakeup fail 02");
        WDT_Close();
        return ;
    }
    
    WDT_Close();
}

void API_WDT_ResetTest(void) 
{    
    if(WDT_InitClock() != 0) {
        CU_FAIL("WDT Init FAIL");  
        return ;
    }
                       
    if(WDT_GET_RESET_FLAG()) {
        printf("[WDT time-out reset occurred (0x%X)]", WDT->CTL);        
        UART_WAIT_TX_EMPTY(UART0);
        WDT_CLEAR_RESET_FLAG();
        CU_ASSERT_EQUAL(WDT_GET_RESET_FLAG(), 0x0);          
    } else {     
        printf("\n\n*** Please execute [R]un again to check test results... ***\n");        
        UART_WAIT_TX_EMPTY(UART0);
        
        /* WDT time-out period is around 25.6 ms */
        WDT_Open(WDT_TIMEOUT_2POW4, NULL, TRUE, TRUE);
        while(SYS->PDID);
    }               
}

CU_TestInfo WDT_MACRO[] =
{
    {"Check WDT Time-out Reset Test ",                          API_WDT_ResetTest}, 
    {"Check WDT CONSTANT (with WDT_Open and WDT_Close API) ",   CONSTANT_WDT_SETTING}, 
    CU_TEST_INFO_NULL
};

CU_TestInfo WDT_API[] =
{
    {"Check WDT Enable/Disable Interrupt API ",                             API_WDT_Interrupt}, 
    {"Check WDT_Open API (with Interrupt Status and Counter Reset MACRO) ", API_WDT_Open}, 
    {"Check WDT Wake-up Behavior (with Wake-up Interrupt Status MACRO) ",   API_WDT_Wakeup}, 
    CU_TEST_INFO_NULL
};
