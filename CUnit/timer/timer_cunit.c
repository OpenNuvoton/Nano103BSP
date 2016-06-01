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
#include "timer_cunit.h"


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
    {"TIMER MACRO", suite_success_init, suite_success_clean, NULL, NULL, TIMER_MACRO},
    {"TIMER API", suite_success_init, suite_success_clean, NULL, NULL, TIMER_API},
    CU_SUITE_INFO_NULL
};

void PowerDownFunction(void)
{
    // Wait 'til UART FIFO empty to get a cleaner console out
    while(!(UART0->FIFOSTS & UART_FIFOSTS_TXEMPTY_Msk));
    // Enable sleep deep mode
    SCB->SCR = SCB_SCR_SLEEPDEEP_Msk;
    // Enable power down mode
    CLK->PWRCTL |= CLK_PWRCTL_PDEN_Msk;
    // Power down
    __WFI();
}

volatile uint8_t g_u8IsTMRWKF[4] = {0};
void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1) {
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
        while(TIMER_GetIntFlag(TIMER0) == 1);
    }

    if(TIMER_GetWakeupFlag(TIMER0) == 1) {
        /* Clear Timer0 wake-up flag */
        TIMER_ClearWakeupFlag(TIMER0);
        while(TIMER_GetWakeupFlag(TIMER0) == 1);

        g_u8IsTMRWKF[0] = 1;
    }
}

void TMR1_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER1) == 1) {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER1);
        while(TIMER_GetIntFlag(TIMER1) == 1);
    }

    if(TIMER_GetWakeupFlag(TIMER1) == 1) {
        /* Clear Timer1 wake-up flag */
        TIMER_ClearWakeupFlag(TIMER1);
        while(TIMER_GetWakeupFlag(TIMER1) == 1);

        g_u8IsTMRWKF[1] = 1;
    }
}

void TMR2_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER2) == 1) {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER2);
        while(TIMER_GetIntFlag(TIMER2) == 1);
    }

    if(TIMER_GetWakeupFlag(TIMER2) == 1) {
        /* Clear Timer1 wake-up flag */
        TIMER_ClearWakeupFlag(TIMER2);
        while(TIMER_GetWakeupFlag(TIMER2) == 1);

        g_u8IsTMRWKF[2] = 1;
    }
}

void TMR3_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER3) == 1) {
        /* Clear Timer1 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER3);
        while(TIMER_GetIntFlag(TIMER3) == 1);
    }

    if(TIMER_GetWakeupFlag(TIMER3) == 1) {
        /* Clear Timer1 wake-up flag */
        TIMER_ClearWakeupFlag(TIMER3);
        while(TIMER_GetWakeupFlag(TIMER3) == 1);

        g_u8IsTMRWKF[3] = 1;
    }
}

int32_t TIMER_InitClock(void)
{    
    volatile uint32_t u32Timeout = 0xf0000;
    
    //SYS_UnlockReg();
    
    /* Enable HXT */
    //CLK->PWRCTL |= CLK_PWRCTL_XTL12M;

    /* Waiting for clock ready */
    //while(u32Timeout-- > 0) {
    //    if((CLK->STATUS&CLK_STATUS_XTLSTB_Msk) == CLK_STATUS_XTLSTB_Msk) {
            
            CLK->APBCLK |= CLK_APBCLK_TMR0CKEN_Msk | CLK_APBCLK_TMR1CKEN_Msk | CLK_APBCLK_TMR2CKEN_Msk | CLK_APBCLK_TMR3CKEN_Msk;
            CLK->CLKSEL1 = CLK->CLKSEL1 & ~(CLK_CLKSEL1_TMR0SEL_Msk | CLK_CLKSEL1_TMR1SEL_Msk);
            CLK->CLKSEL2 = CLK->CLKSEL2 & ~(CLK_CLKSEL2_TMR2SEL_Msk | CLK_CLKSEL2_TMR3SEL_Msk);

            return 0;
    //    }
    //}
    
    //return -1;
}

int32_t IsTimerRegsCleared(TIMER_T *timer)
{
    if((timer->CTL != 0x0) || (timer->CMP != 0x0) || (timer->PRECNT != 0x0) || (timer->INTEN != 0x0) ||
        (timer->INTSTS != 0))
    {
        return -1;
    }
    
    return 0;
}

int32_t ClearTimerRegs(TIMER_T *timer)
{
    TIMER_Close(timer);
    
    timer->CMP = 0x0;
    timer->PRECNT = 0x0;
    timer->INTEN = 0x0;
    timer->INTSTS = timer->INTSTS;

    if(IsTimerRegsCleared(timer) < 0)
        return -1;
    
    return 0;    
}

const uint32_t au32OPModeSel[] = {
    TIMER_ONESHOT_MODE,
    TIMER_PERIODIC_MODE,
    TIMER_TOGGLE_MODE,
    TIMER_CONTINUOUS_MODE,
};
const uint32_t au32CapEdgeSel[] = {
    TIMER_CAPTURE_FALLING_EDGE,
    TIMER_CAPTURE_RISING_EDGE,
    TIMER_CAPTURE_FALLING_THEN_RISING_EDGE,
    TIMER_CAPTURE_RISING_THEN_FALLING_EDGE,
};
const uint32_t au32CntEdgeSel[] = {
    TIMER_COUNTER_FALLING_EDGE,
    TIMER_COUNTER_RISING_EDGE,
};
TIMER_T *TimerCh[] = {
    TIMER0, 
    TIMER1,
    TIMER2, 
    TIMER3,
};

void MACRO_TIMER_SET_CMP_VALUE(void)
{
//    int32_t i32Err = -1;
    volatile uint32_t i;

    if(TIMER_InitClock() != 0) {
        CU_FAIL("TIMER Init FAIL");  
        return ;
    }
    
    /* Check TIMER_SET_CMP_VALUE */
    for(i=0; i<4; i++) { 
        CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);  
        
        TIMER_SET_CMP_VALUE((TIMER_T *)TimerCh[i], 0x2);       
        CU_ASSERT_EQUAL(TimerCh[i]->CMP, 0x2);          
        
        TIMER_SET_CMP_VALUE((TIMER_T *)TimerCh[i], 0x5A5A5A);       
        CU_ASSERT_EQUAL(TimerCh[i]->CMP, 0x5A5A5A);          
        
        TIMER_SET_CMP_VALUE((TIMER_T *)TimerCh[i], 0xFFFFFF);       
        CU_ASSERT_EQUAL(TimerCh[i]->CMP, 0xFFFFFF);          
    }
    
}

void MACRO_TIMER_SET_PRESCALE_VALUE(void)
{
    volatile uint32_t i;

    if(TIMER_InitClock() != 0) {
        CU_FAIL("TIMER Init FAIL");  
        return ;
    }
    
    for(i=0; i<4; i++) { 
        CU_ASSERT_EQUAL(ClearTimerRegs((TIMER_T *)TimerCh[i]), 0);  
        
        TIMER_SET_PRESCALE_VALUE((TIMER_T *)TimerCh[i], 0x2);       
        CU_ASSERT_EQUAL(TimerCh[i]->PRECNT, 0x2);          
        
        TIMER_SET_PRESCALE_VALUE((TIMER_T *)TimerCh[i], 0x5A);       
        CU_ASSERT_EQUAL(TimerCh[i]->PRECNT, 0x5A);          
        
        TIMER_SET_PRESCALE_VALUE((TIMER_T *)TimerCh[i], 0xFF);       
        CU_ASSERT_EQUAL(TimerCh[i]->PRECNT, 0xFF);          
    }
}


void API_TIMER_StartAndStop(void)
{
    int32_t i32Err = -1;
    volatile uint32_t i;

    if(TIMER_InitClock() != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }
    
    /* Check TIMER_Start, TIMER_Stop and TIMER_IS_ACTIVE */
    for(i=0; i<4; i++) { 
        if(ClearTimerRegs((TIMER_T *)TimerCh[i]) < 0) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }    
        
        TimerCh[i]->CMP = 0x00FFFFFF;
        TimerCh[i]->CTL  = 0x00000010;
        
        if(TIMER_IS_ACTIVE((TIMER_T *)TimerCh[i]) == 1) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }
        
        TIMER_Start((TIMER_T *)TimerCh[i]);
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        if(TIMER_IS_ACTIVE((TIMER_T *)TimerCh[i]) == 0) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }
        if(TimerCh[i]->CTL != 0x00000091) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }
        
        TIMER_Stop((TIMER_T *)TimerCh[i]);
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        if(TIMER_IS_ACTIVE((TIMER_T *)TimerCh[i]) == 1) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        } 
        
        if(TimerCh[i]->CTL != 0x00000010) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }
    }
    
    i32Err = 0;
    
exit_test:  
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}

void API_TIMER_CaptureDebounce(void)
{
    int32_t i32Err = -1;
    volatile uint32_t i;

    if(TIMER_InitClock() != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }
    
    /* Check TIMER_EnableCaptureDebounce and TIMER_DisableCaptureDebounce */
    for(i=0; i<4; i++) { 
//        printf("TIMER_EnableCaptureDebounce (Ch-%d)\n", i);      
        if(ClearTimerRegs((TIMER_T *)TimerCh[i]) < 0) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }    
        
        TIMER_EnableCaptureDebounce((TIMER_T *)TimerCh[i]);       
        if(TimerCh[i]->CTL != 0x00400000) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }
        
        TIMER_DisableCaptureDebounce((TIMER_T *)TimerCh[i]);       
        if(TimerCh[i]->CTL != 0x0) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }
    }
    
    i32Err = 0;
    
exit_test:  
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}

void API_TIMER_EventCounterDebounce(void)
{
    int32_t i32Err = -1;
    volatile uint32_t i;

    if(TIMER_InitClock() != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }
    
    /* Check TIMER_EnableEventCounterDebounce and TIMER_DisableEventCounterDebounce */
    for(i=0; i<4; i++) { 
//        printf("TIMER_EnableEventCounterDebounce (Ch-%d)\n", i);       
        if(ClearTimerRegs((TIMER_T *)TimerCh[i]) < 0) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }    
        
        TIMER_EnableEventCounterDebounce((TIMER_T *)TimerCh[i]);       
        if(TimerCh[i]->CTL != 0x00004000) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }
        
        TIMER_DisableEventCounterDebounce((TIMER_T *)TimerCh[i]);       
        if(TimerCh[i]->CTL != 0x0) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }
    }       
    
    
    i32Err = 0;
    
exit_test:  
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}

void API_TIMER_OpenAndInterrupt(void)
{
    int32_t i32Err = -1;
    volatile uint32_t i, j, u32Timeout = 0;

    if(TIMER_InitClock() != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }
            
    /* Check TIMER_Open, TIMER_EnableInt and TIMER_DisableInt*/
    for(i=0; i<4; i++) {     
        for(j=0; j<sizeof(au32OPModeSel)/4; j++) { 
//            printf("OPModeSel %d (Ch-%d)\n", j, i);        
            if(ClearTimerRegs((TIMER_T *)TimerCh[i]) < 0) {
                CU_ASSERT_EQUAL(i32Err, 0);     
                goto exit_test;
            }               
            
            TIMER_Open((TIMER_T *)TimerCh[i], au32OPModeSel[j], 0);            
            if(TimerCh[i]->CTL != (j<<4)) {
                CU_ASSERT_EQUAL(i32Err, 0);     
                goto exit_test;
            }
            
            TIMER_EnableInt((TIMER_T *)TimerCh[i]);
            if(TimerCh[i]->INTEN != 1) {
                CU_ASSERT_EQUAL(i32Err, 0);     
                goto exit_test;
            }
            
            TIMER_DisableInt((TIMER_T *)TimerCh[i]);
            if(TimerCh[i]->INTEN != 0) {
                CU_ASSERT_EQUAL(i32Err, 0);     
                goto exit_test;
            }
            
            TIMER_Close((TIMER_T *)TimerCh[i]);
            if(TimerCh[i]->CTL != 0x0) {
                CU_ASSERT_EQUAL(i32Err, 0);     
                goto exit_test;
            }                           
        }
    }
    
    i32Err = 0;
    
exit_test:  
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}

void API_TIMER_CaptureAndInterrupt(void)
{
    int32_t i32Err = -1;
    volatile uint32_t i, j, u32Timeout = 0;

    if(TIMER_InitClock() != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }
            
    /* Check Capture Edge Macro - TIMER_EnableCapture, TIMER_DisableCapture, TIMER_EnableCaptureInt, TIMER_DisableCaptureInt */
        /* With TIMER_CAPTURE_FREE_COUNTING_MODE */
        for(i=0; i<4; i++) {     
            for(j=0; j<sizeof(au32CapEdgeSel)/4; j++) { 
//                printf("CapEdgeSel %d (Ch-%d)\n", j, i);        
                if(ClearTimerRegs((TIMER_T *)TimerCh[i]) < 0) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }  
                
                TIMER_EnableCapture((TIMER_T *)TimerCh[i], TIMER_CAPTURE_FREE_COUNTING_MODE, au32CapEdgeSel[j]);
                TIMER_EnableCaptureInt((TIMER_T *)TimerCh[i]);                
                if(TimerCh[i]->CTL != (0x10000 |(j<<18))) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }
                if(TimerCh[i]->INTEN != 0x2) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }
                
                TIMER_DisableCapture((TIMER_T *)TimerCh[i]);
                if(TimerCh[i]->CTL != j<<18) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }
                
                TIMER_DisableCaptureInt((TIMER_T *)TimerCh[i]);
                if(TimerCh[i]->INTEN != 0) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }
            }
        }

        for(i=0; i<4; i++) {     
            for(j=0; j<sizeof(au32CapEdgeSel)/4; j++) { 

                if(ClearTimerRegs((TIMER_T *)TimerCh[i]) < 0) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }  
                
                TIMER_EnableCapture((TIMER_T *)TimerCh[i], TIMER_CAPTURE_TRIGGER_COUNTING_MODE, au32CapEdgeSel[j]);
                TIMER_EnableCaptureInt((TIMER_T *)TimerCh[i]);                
                if(TimerCh[i]->CTL != (0x10000 | (0x100000 |(j<<18)))) {
                    printf("%x\n", TimerCh[i]->CTL);
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }
                if(TimerCh[i]->INTEN != 0x2) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }
                TIMER_DisableCapture((TIMER_T *)TimerCh[i]);
                if(TimerCh[i]->CTL != (0x100000 | (j<<18))) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }
                
                TIMER_DisableCaptureInt((TIMER_T *)TimerCh[i]);
                if(TimerCh[i]->INTEN != 0) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }
            }
        }
        
        /* With TIMER_CAPTURE_COUNTER_RESET_MODE */
        for(i=0; i<4; i++) {     
            for(j=0; j<sizeof(au32CapEdgeSel)/4; j++) { 
                if(ClearTimerRegs((TIMER_T *)TimerCh[i]) < 0) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }    
                
                TIMER_EnableCapture((TIMER_T *)TimerCh[i], TIMER_CAPTURE_COUNTER_RESET_MODE, au32CapEdgeSel[j]);                
                if(TimerCh[i]->CTL != (0x30000 |(j<<18))) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }
                
                TIMER_DisableCapture((TIMER_T *)TimerCh[i]);
                if(TimerCh[i]->CTL != (0x20000 |(j<<18))) {
                    CU_ASSERT_EQUAL(i32Err, 0);     
                    goto exit_test;
                }
            }
        }
    
    i32Err = 0;
    
exit_test:  
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}

void API_TIMER_EventCounter(void) 
{
    int32_t i32Err = -1;
    volatile uint32_t i, j, u32Timeout = 0;

    if(TIMER_InitClock() != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }
        
    /* Check Counter Edge Macro - TIMER_EnableEventCounter, TIMER_DisableEventCounter */
    for(i=0; i<4; i++) {     
        for(j=0; j<sizeof(au32CntEdgeSel)/4; j++) { 
//            printf("CntEdgeSel %d (Ch-%d)\n", j, i);        
            if(ClearTimerRegs((TIMER_T *)TimerCh[i]) < 0) {
                CU_ASSERT_EQUAL(i32Err, 0);     
                goto exit_test;
            }    
            
            TIMER_EnableEventCounter((TIMER_T *)TimerCh[i], au32CntEdgeSel[j]);            
            if(TimerCh[i]->CTL != ((j<<13) | (0x1000))) {
                CU_ASSERT_EQUAL(i32Err, 0);     
                goto exit_test;
            }
            
            TIMER_DisableEventCounter((TIMER_T *)TimerCh[i]);
            if(TimerCh[i]->CTL != j<<13) {
                CU_ASSERT_EQUAL(i32Err, 0);     
                goto exit_test;
            }
        }
    }
    
    i32Err = 0;
    
exit_test:  
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}

void API_TIMER_Wakeup(void)
{
    int32_t i32Err = -1;
    volatile uint32_t i, u32TDR;

    SYS->BODCTL &= ~SYS_BODCTL_LVREN_Msk;  // Ver. A bug, cannot enable LVR/POR whiel testing power down/wake up function.
    
    if(TIMER_InitClock() != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }
    
    /* Check TIMER_EnableWakeup and TIMER_DisableWakeup */
    for(i=0; i<4; i++) { 
//        printf("TIMER_EnableWakeup (Ch-%d)\n", i);       
        if(ClearTimerRegs((TIMER_T *)TimerCh[i]) < 0) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }    
        
        TIMER_EnableWakeup((TIMER_T *)TimerCh[i]);       
        if(TimerCh[i]->CTL != 0x4) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }
        
        TIMER_DisableWakeup((TIMER_T *)TimerCh[i]);       
        if(TimerCh[i]->CTL != 0x0) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }
    }
    
    /* Enable LIRC */
    //CLK->PWRCTL |= CLK_PWRCTL_LIRCEN_Msk;
    //while(!(CLK->STATUS & CLK_STATUS_LIRCSTB_Msk));
    CLK->CLKSEL1 &= ~(CLK_CLKSEL1_TMR0SEL_Msk | CLK_CLKSEL1_TMR1SEL_Msk);
    CLK->CLKSEL1 |= (CLK_CLKSEL1_TMR0SEL_LIRC | CLK_CLKSEL1_TMR1SEL_LIRC);
    CLK->CLKSEL2 &= ~(CLK_CLKSEL2_TMR2SEL_Msk | CLK_CLKSEL2_TMR3SEL_Msk);
    CLK->CLKSEL2 |= (CLK_CLKSEL2_TMR2SEL_LIRC | CLK_CLKSEL2_TMR3SEL_LIRC);
    
    for(i=0; i<4; i++) {
        if(ClearTimerRegs((TIMER_T *)TimerCh[0]) < 0) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }    
    }

    NVIC_EnableIRQ(TMR0_IRQn);
    NVIC_EnableIRQ(TMR1_IRQn);
    NVIC_EnableIRQ(TMR2_IRQn);
    NVIC_EnableIRQ(TMR3_IRQn);
    
    /* Check TIMER_GetIntFlag, TIMER_ClearIntFlag, TIMER_GetWakeupFlag, TIMER_ClearWakeupFlag and TIMER_GetCounter */
    for(i=0; i<4; i++) {
        TimerCh[i]->CMP = 100;
        TimerCh[i]->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_WKEN_Msk;
        TimerCh[i]->INTEN = TIMER_INTEN_CNTIEN_Msk;

        g_u8IsTMRWKF[i] = 0;
        printf("%d %02x, %02x, %02x\n", i, TimerCh[i]->CMP, TimerCh[i]->CTL, TimerCh[i]->INTEN);
        {int volatile ii; for(ii = 0; ii < 10000; ii++);}  // prevent modify CTL too soon since timer clock is slow now
        TIMER_Start(TimerCh[i]);
        printf("%d %02x, %02x, %02x %02x\n", i, TimerCh[i]->CMP, TimerCh[i]->CTL, TimerCh[i]->INTEN, TimerCh[i]->CNT);
        PowerDownFunction();

        while(g_u8IsTMRWKF[i] == 0);
        
        u32TDR = TIMER_GetCounter(TimerCh[i]);
        if(u32TDR > 120) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }
        
        TIMER_Stop(TimerCh[i]);
    }   
    
    i32Err = 0;
    
exit_test:  
    NVIC_DisableIRQ(TMR0_IRQn);
    NVIC_DisableIRQ(TMR1_IRQn);
    NVIC_DisableIRQ(TMR2_IRQn);
    NVIC_DisableIRQ(TMR3_IRQn);
    TIMER_Stop(TIMER0);
    TIMER_Stop(TIMER1);
    TIMER_Stop(TIMER2);
    TIMER_Stop(TIMER3);
    
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}

void API_TIMER_Delay(void)
{
    int32_t i32Err = -1;
    volatile uint32_t i, u32TDR[3];

    if(TIMER_InitClock() != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }
    // previous test using LIRC as timer source, give it some time to switch back to HXT
    {int volatile ii; for(ii = 0; ii < 10000; ii++);}
    SYS->IPRST2 = SYS_IPRST2_TMR0RST_Msk | SYS_IPRST2_TMR1RST_Msk | SYS_IPRST2_TMR2RST_Msk | SYS_IPRST2_TMR3RST_Msk;
    SYS->IPRST2 = 0;
    
    /* Check TIMER_Delay */
    for(i=0; i < 4; i++) { 

        if((i == 0) || (i == 1) ) {
            TIMER2->CMP = 0xFFFFFF;
            TIMER2->PRECNT = 12 - 1;
            TIMER2->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk;
            
            TIMER2->CMP = 0xFFFFFF;
            TIMER_Delay((TIMER_T *)TimerCh[i], 1000); 
            u32TDR[0] = TIMER_GetCounter(TIMER2);
            
            TIMER2->CMP = 0xFFFFFF;
            TIMER_Delay(TimerCh[i], 500000);       
            u32TDR[1] = TIMER_GetCounter(TIMER2);
            
            TIMER2->CMP = 0xFFFFFF;
            TIMER_Delay(TimerCh[i], 1000000);       
            u32TDR[2] = TIMER_GetCounter(TIMER2);     
        } else {        
            TIMER0->CMP = 0xFFFFFF;
            TIMER0->PRECNT = 12 - 1;
            TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk;

            TIMER0->CMP = 0xFFFFFF;
            TIMER_Delay((TIMER_T *)TimerCh[i], 1000); 
            u32TDR[0] = TIMER_GetCounter(TIMER0);
            
            TIMER0->CMP = 0xFFFFFF;
            TIMER_Delay(TimerCh[i], 500000);       
            u32TDR[1] = TIMER_GetCounter(TIMER0);
            
            TIMER0->CMP = 0xFFFFFF;
            TIMER_Delay(TimerCh[i], 1000000);       
            u32TDR[2] = TIMER_GetCounter(TIMER0);     
        }
        // delay NOP in TIMER_Delay make this u32TDR[x] larger than expected. however this delay is necessary while 
        // system is using PLL and timer use LXT/LIRC
        if(((u32TDR[0] > 2500) || (u32TDR[0] <= 1000)) ||
            ((u32TDR[1] > 501500) || (u32TDR[1] <= 500000)) ||
            ((u32TDR[2] > 1001500) || (u32TDR[2] <= 1000000))) {
                CU_ASSERT_EQUAL(i32Err, 0);     
                goto exit_test;
            }
    }
    
    i32Err = 0;
    
exit_test:  
    TIMER_Stop(TIMER0);
    TIMER_Stop(TIMER1);
    TIMER_Stop(TIMER2);
    TIMER_Stop(TIMER3);
    
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}

void API_TIMER_GetModuleClock(void)
{
    int32_t i32Err = -1;
    volatile uint32_t i, j, u32Timeout = 0;

    if(TIMER_InitClock() != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }
            
    /* Check TIMER_GetModuleClock */
    CLK->CLKSEL1 &= ~(CLK_CLKSEL1_TMR0SEL_Msk | CLK_CLKSEL1_TMR1SEL_Msk);
    CLK->CLKSEL1 |= (CLK_CLKSEL1_TMR0SEL_HXT | CLK_CLKSEL1_TMR1SEL_HXT);
    CLK->CLKSEL2 &= ~(CLK_CLKSEL2_TMR2SEL_Msk | CLK_CLKSEL2_TMR3SEL_Msk);
    CLK->CLKSEL2 |= (CLK_CLKSEL2_TMR2SEL_HXT | CLK_CLKSEL2_TMR3SEL_HXT);
    if((TIMER_GetModuleClock(TIMER0) != __HXT) || (TIMER_GetModuleClock(TIMER1) != __HXT) ||
        (TIMER_GetModuleClock(TIMER2) != __HXT) || (TIMER_GetModuleClock(TIMER3) != __HXT)) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }               
               
    CLK->CLKSEL1 &= ~(CLK_CLKSEL1_TMR0SEL_Msk | CLK_CLKSEL1_TMR1SEL_Msk);
    CLK->CLKSEL1 |= (CLK_CLKSEL1_TMR0SEL_LIRC | CLK_CLKSEL1_TMR1SEL_LIRC);
    CLK->CLKSEL2 &= ~(CLK_CLKSEL2_TMR2SEL_Msk | CLK_CLKSEL2_TMR3SEL_Msk);
    CLK->CLKSEL2 |= (CLK_CLKSEL2_TMR2SEL_LIRC | CLK_CLKSEL2_TMR3SEL_LIRC);
    if((TIMER_GetModuleClock(TIMER0) != __LIRC) || (TIMER_GetModuleClock(TIMER1) != __LIRC) ||
        (TIMER_GetModuleClock(TIMER2) != __LIRC) || (TIMER_GetModuleClock(TIMER3) != __LIRC)) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }               
    
    CLK->CLKSEL1 &= ~(CLK_CLKSEL1_TMR0SEL_Msk | CLK_CLKSEL1_TMR1SEL_Msk);
    CLK->CLKSEL1 |= (CLK_CLKSEL1_TMR0SEL_HIRC | CLK_CLKSEL1_TMR1SEL_HIRC);
    CLK->CLKSEL2 &= ~(CLK_CLKSEL2_TMR2SEL_Msk | CLK_CLKSEL2_TMR3SEL_Msk);
    CLK->CLKSEL2 |= (CLK_CLKSEL2_TMR2SEL_HIRC | CLK_CLKSEL2_TMR3SEL_HIRC);
    if((TIMER_GetModuleClock(TIMER0) != __HIRC) || (TIMER_GetModuleClock(TIMER1) != __HIRC) ||
        (TIMER_GetModuleClock(TIMER2) != __HIRC) || (TIMER_GetModuleClock(TIMER3) != __HIRC)) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }

		CLK->CLKSEL1 &= ~(CLK_CLKSEL1_TMR0SEL_Msk | CLK_CLKSEL1_TMR1SEL_Msk);
    CLK->CLKSEL1 |= (CLK_CLKSEL1_TMR0SEL_MIRC | CLK_CLKSEL1_TMR1SEL_MIRC);
    CLK->CLKSEL2 &= ~(CLK_CLKSEL2_TMR2SEL_Msk | CLK_CLKSEL2_TMR3SEL_Msk);
    CLK->CLKSEL2 |= (CLK_CLKSEL2_TMR2SEL_MIRC | CLK_CLKSEL2_TMR3SEL_MIRC);
    if((TIMER_GetModuleClock(TIMER0) != __MIRC) || (TIMER_GetModuleClock(TIMER1) != __MIRC) ||
        (TIMER_GetModuleClock(TIMER2) != __MIRC) || (TIMER_GetModuleClock(TIMER3) != __MIRC)) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }		
    
    CLK->CLKSEL1 &= ~(CLK_CLKSEL1_TMR0SEL_Msk | CLK_CLKSEL1_TMR1SEL_Msk);
    CLK->CLKSEL1 |= (CLK_CLKSEL1_TMR0SEL_LXT | CLK_CLKSEL1_TMR1SEL_LXT);
    CLK->CLKSEL2 &= ~(CLK_CLKSEL2_TMR2SEL_Msk | CLK_CLKSEL2_TMR3SEL_Msk);
    CLK->CLKSEL2 |= (CLK_CLKSEL2_TMR2SEL_LXT | CLK_CLKSEL2_TMR3SEL_LXT);
    if((TIMER_GetModuleClock(TIMER0) != __LXT) || (TIMER_GetModuleClock(TIMER1) != __LXT) ||
        (TIMER_GetModuleClock(TIMER2) != __LXT) || (TIMER_GetModuleClock(TIMER3) != __LXT)) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }               
    
    i32Err = 0;
    
exit_test:  
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}


void API_TIMER_TriggerSetting(void)
{
    int32_t i, i32Err = -1;

    
    /* Check TIMER_Delay */
    for(i=0; i<4; i++) { 
        if(ClearTimerRegs((TIMER_T *)TimerCh[i]) < 0) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }    
        
        TIMER_SetTriggerSource((TIMER_T *)TimerCh[i], TIMER_CAPTURE_TRIGGER);
        if(TimerCh[i]->CTL != (1 << 11)) {
            CU_ASSERT_EQUAL(i32Err, 0);    
            goto exit_test;
        }
        
        TIMER_SetTriggerSource((TIMER_T *)TimerCh[i], TIMER_TIMEOUT_TRIGGER);
        if(TimerCh[i]->CTL != 0) {
            CU_ASSERT_EQUAL(i32Err, 0);    
            goto exit_test;
        }
				
				TIMER_SetTriggerTarget((TIMER_T *)TimerCh[i], TIMER_CTL_TRGPWM_Msk);
        if(TimerCh[i]->CTL != 0x10000000) {
            CU_ASSERT_EQUAL(i32Err, 0);    
            goto exit_test;
        }
        
        TIMER_SetTriggerTarget((TIMER_T *)TimerCh[i], TIMER_CTL_TRGPDMA_Msk);
        if(TimerCh[i]->CTL != 0x400) {
            CU_ASSERT_EQUAL(i32Err, 0);    
            goto exit_test;
        }

        TIMER_SetTriggerTarget((TIMER_T *)TimerCh[i], TIMER_CTL_TRGADC_Msk);
        if(TimerCh[i]->CTL != 0x100) {
            CU_ASSERT_EQUAL(i32Err, 0);    
            goto exit_test;
        }
    }
    i32Err = 0;
    
exit_test:  
    TIMER_Stop(TIMER0);
    TIMER_Stop(TIMER1);
    TIMER_Stop(TIMER2);
    TIMER_Stop(TIMER3);
    
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}



void API_TIMER_FreqCounter(void)
{
    int32_t i, i32Err = -1;

    
    /* Check TIMER_Delay */
    for(i=0; i<4; i+=2) { 
        if(ClearTimerRegs((TIMER_T *)TimerCh[i]) < 0) {
            CU_ASSERT_EQUAL(i32Err, 0);     
            goto exit_test;
        }    
    }
   
    TIMER_EnableFreqCounter(TIMER0, 0, 0, TRUE);
    if(TIMER0->CTL != 0x01001081) {   // [7] Timer is Active
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    if(TIMER1->INTEN != 0x2) {
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    if(TIMER1->CMP != 0xFFFFFF) {
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    TIMER_EnableFreqCounter(TIMER0, 0, 0, FALSE);
    if(TIMER0->CTL != 0x01001081) {   // [7] Timer is Active
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    if(TIMER1->INTEN != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    if(TIMER1->CMP != 0xFFFFFF) {
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    TIMER_DisableFreqCounter(TIMER0);
    if(TIMER0->CTL != 0x1081) {   // [7] Timer is Active
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }

    TIMER_EnableFreqCounter(TIMER2, 0, 0, TRUE);
    if(TIMER2->CTL != 0x01001081) {   // [7] Timer is Active
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    if(TIMER3->INTEN != 0x2) {
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    if(TIMER3->CMP != 0xFFFFFF) {
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    TIMER_EnableFreqCounter(TIMER2, 0, 0, FALSE);
    if(TIMER2->CTL != 0x01001081) {   // [7] Timer is Active
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    if(TIMER3->INTEN != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    if(TIMER3->CMP != 0xFFFFFF) {
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    TIMER_DisableFreqCounter(TIMER2);
    if(TIMER2->CTL != 0x1081) {   // [7] Timer is Active
        CU_ASSERT_EQUAL(i32Err, 0);    
        goto exit_test;
    }
    
    i32Err = 0;
    
exit_test:  
    TIMER_Stop(TIMER0);
    TIMER_Stop(TIMER1);
    TIMER_Stop(TIMER2);
    TIMER_Stop(TIMER3);
    
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}

void API_TIMER_Reset(void)
{
    int32_t i32Err = -1;
    volatile uint32_t i, t1;

    if(TIMER_InitClock() != 0) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }
    CLK->CLKSEL1 &= ~(CLK_CLKSEL1_TMR0SEL_Msk | CLK_CLKSEL1_TMR1SEL_Msk);
    CLK->CLKSEL1 |= (CLK_CLKSEL1_TMR0SEL_LXT | CLK_CLKSEL1_TMR1SEL_LXT);
    CLK->CLKSEL2 &= ~(CLK_CLKSEL2_TMR2SEL_Msk | CLK_CLKSEL2_TMR3SEL_Msk);
    CLK->CLKSEL2 |= (CLK_CLKSEL2_TMR2SEL_LXT | CLK_CLKSEL2_TMR3SEL_LXT);
    if((TIMER_GetModuleClock(TIMER0) != __LXT) || (TIMER_GetModuleClock(TIMER1) != __LXT) ||
        (TIMER_GetModuleClock(TIMER2) != __LXT) || (TIMER_GetModuleClock(TIMER3) != __LXT)) {
        CU_ASSERT_EQUAL(i32Err, 0);     
        goto exit_test;
    }             
        
    for(i=0; i < 4; i++) { 
        TIMER_Open((TIMER_T *)TimerCh[i], TIMER_PERIODIC_MODE, 1);
        TIMER_Start((TIMER_T *)TimerCh[i]);
        while(TIMER_GetCounter((TIMER_T *)TimerCh[i]) == 0);
        TIMER_ResetCounter((TIMER_T *)TimerCh[i]);
        while(((TIMER_T *)TimerCh[i])->CNT & 0x80000000);
        t1 = TIMER_GetCounter((TIMER_T *)TimerCh[i]);
        if(t1 != 0) {
            CU_ASSERT_EQUAL(i32Err, 0);
            goto exit_test;
        }

    }
    
    i32Err = 0;
    
exit_test:  
    TIMER_Stop(TIMER0);
    TIMER_Stop(TIMER1);
    TIMER_Stop(TIMER2);
    TIMER_Stop(TIMER3);
    
    if(i32Err == 0)
        CU_ASSERT_EQUAL(i32Err, 0);     
}


void API_TIMER_CaptureStatus(void)
{
    unsigned int volatile ii;
    *(unsigned int volatile *)(0x50000014) = 1;  // enter test mode
    
    TIMER_InitClock();
    
    /* Check TIMER_GetCaptureIntFlag, TIMER_ClearCaptureIntFlag and TIMER_GetCaptureData */
    /* Timer0 */
    //SYS->GPA_MFPH = (SYS->GPA_MFPH & ~SYS_GPA_MFPH_PA12MFP_Msk) | SYS_GPA_MFPH_PA12MFP_TM0_EXT;
    PA->MODE |= (1 << 24); // output
    TIMER0->CMP = 0xFFFFFF;
    TIMER0->CTL = TIMER_CONTINUOUS_MODE | TIMER_CTL_CNTEN_Msk | TIMER_CTL_CAPEN_Msk;

    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlag(TIMER0), 0);
    PA12 = 1;
    for(ii = 0; ii < 1000; ii++);
    PA12 = 0;
    for(ii = 0; ii < 1000; ii++);
    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlag(TIMER0), 1);
    CU_ASSERT_EQUAL(TIMER_GetCaptureData(TIMER0), TIMER0->CAP);
    TIMER_ClearCaptureIntFlag(TIMER0);
    CU_ASSERT_EQUAL(TIMER_GetCaptureIntFlag(TIMER0), 0);


    TIMER_Stop(TIMER0);
    *(unsigned int volatile *)(0x50000014) = 0; 
}


CU_TestInfo TIMER_MACRO[] =
{
    {"Check TIMER_SET_CMP_VALUE ",      MACRO_TIMER_SET_CMP_VALUE}, 
    {"Check TIMER_SET_PRESCALE_VALUE ", MACRO_TIMER_SET_PRESCALE_VALUE}, 
    {"Check TIMER_IS_ACTIVE ",          API_TIMER_StartAndStop}, 
    CU_TEST_INFO_NULL
};

CU_TestInfo TIMER_API[] =
{
#if 1   // code too large, need to seperate to 2 test cases
//    {"Check TIMER Start/Stop ",                                  API_TIMER_StartAndStop}, 
//    {"Check Enable/Disable Capture Debounce API ",               API_TIMER_CaptureDebounce}, 
//    {"Check Enable/Disable Event Counter Debounce API ",         API_TIMER_EventCounterDebounce}, 
//    {"Check TIMER Open/Close and Enable/Disable Interrupt API ", API_TIMER_OpenAndInterrupt}, 
//    {"Check Enable/Disable Capture Function and Interrupt API ", API_TIMER_CaptureAndInterrupt}, 
//    {"Check Enable/Disable Event Counter Function API ",         API_TIMER_EventCounter}, 
//    {"Check Interrupt, Wake-up Function and Status API ",        API_TIMER_Wakeup}, 
//    {"Check TIMER Delay API ",                                   API_TIMER_Delay}, 
//    {"Check Get Module Clock API ",                              API_TIMER_GetModuleClock}, 
//    {"Check Trigger Setting",                                    API_TIMER_TriggerSetting}, 
    {"Check Freq Counter",                                       API_TIMER_FreqCounter},    
//    {"Check Timer Reset",                                        API_TIMER_Reset},        
//    {"Check Capture Interrupt Status API ",                      API_TIMER_CaptureStatus}, 
#endif
    CU_TEST_INFO_NULL
};
