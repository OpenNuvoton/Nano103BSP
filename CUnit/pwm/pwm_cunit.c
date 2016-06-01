/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                             */
/*                                                                                                         */
/* Copyright (c) 2014 by Nuvoton Technology Corporation                                                    */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   pwm_cunit.c                                                                                           */
/*            The test function of PWM for CUnit.                                                          */
/* Project:   NANO103                                                                                       */
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
#include "pwm_cunit.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"

extern unsigned int STATE0; 
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;

#if 0
#define PWM_CUNIT_TEST_1_TO_2
#define PWM_CUNIT_TEST_3_TO_5
#define PWM_CUNIT_TEST_6
#define PWM_CUNIT_TEST_9_TO_11
#define PWM_CUNIT_TEST_12_TO_14
#else
#define PWM_CUNIT_TEST_18
#define PWM_CUNIT_TEST_20_TO_21
#define PWM_CUNIT_TEST_22_TO_23
#define PWM_CUNIT_TEST_24_TO_25
#define PWM_CUNIT_TEST_26
#define PWM_CUNIT_TEST_27_to_28
#endif

/*---------------------------------------------------------------------------------------------------------*/
/*  Constant Definitions                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define PWM_MAX_CH 6
uint32_t EightBitsPatternTable[4] = {0xFF, 0x5A, 0xA5, 0x00}; /* 8 bits test pattern */
uint32_t SixteenBitsPatternTable[4] = {0xFFFF, 0x5A5A, 0xA5A5, 0x0000}; /* 16 bits test pattern */

#define PWM_CH0 0
#define PWM_CH1 1
#define PWM_CH2 2
#define PWM_CH3 3
#define PWM_CH4 4
#define PWM_CH5 5

/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void) 
{
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRC0EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRC0STB_Msk);

    /* Enable PWM module clock */
    CLK_EnableModuleClock(PWM0_MODULE);

    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    return 0; 
}

int suite_success_clean(void)
{
    /* Enable PWM module clock */
    CLK->APBCLK &= ~(CLK_APBCLK_PWM0CKEN_Msk);
    return 0;
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
    {"PWM MACRO", suite_success_init, suite_success_clean, NULL, NULL, PWM_MACRO},
    {"PWM API", suite_success_init, suite_success_clean, NULL, NULL, PWM_API},
    CU_SUITE_INFO_NULL
};

void Func_PWM_Start() 
{
    uint32_t u32TestCh = 0;
    
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    //test case 1: enable/disable one channel each time, then check each action
    for (u32TestCh = 0; u32TestCh < PWM_CHANNEL_NUM; u32TestCh++)
    {
        PWM_Start(PWM0, 1 << u32TestCh);
        CU_ASSERT_EQUAL(PWM0->CNTEN & (1UL << ((u32TestCh >> 1) << 1)), 1UL << ((u32TestCh >> 1) << 1));
      
        PWM_ForceStop(PWM0, 1 << u32TestCh);
        CU_ASSERT_EQUAL(PWM0->CNTEN & (1UL << ((u32TestCh >> 1) << 1)), 0 );
    }

    //test case 2: enable/disable one channel each time, then check at final action
    for (u32TestCh = 0; u32TestCh < PWM_CHANNEL_NUM; u32TestCh++)
    {
        PWM_Start(PWM0, 1 << u32TestCh);
    }
    CU_ASSERT_EQUAL(PWM0->CNTEN, 0x15);

    for (u32TestCh = 0; u32TestCh < PWM_CHANNEL_NUM; u32TestCh++)
    {
        PWM_ForceStop(PWM0, 1 << u32TestCh);
    }
    CU_ASSERT_EQUAL(PWM0->CNTEN, 0);

    //test case 3: enable/disable multiple channels each time
    PWM_Start(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1));
    CU_ASSERT_EQUAL(PWM0->CNTEN, 0x1);
    
    PWM_ForceStop(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1));
    CU_ASSERT_EQUAL(PWM0->CNTEN, 0);
    
    PWM_Start(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1) | (1 << PWM_CH2));
    CU_ASSERT_EQUAL(PWM0->CNTEN, 0x5);
    
    PWM_ForceStop(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1) | (1 << PWM_CH2));
    CU_ASSERT_EQUAL(PWM0->CNTEN, 0);
    
    PWM_Start(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1) | (1 << PWM_CH2) | (1 << PWM_CH3));
    CU_ASSERT_EQUAL(PWM0->CNTEN, 0x5);
    
    PWM_ForceStop(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1) | (1 << PWM_CH2) | (1 << PWM_CH3));
    CU_ASSERT_EQUAL(PWM0->CNTEN, 0);
}

void Func_PWM_Stop() 
{
    uint32_t u32TestCh = 0;
    
    Func_PWM_Start();
    
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    //test case 1: enable/disable one channel each time, then check each action
    for (u32TestCh = 0; u32TestCh < PWM_CHANNEL_NUM; u32TestCh++)
    {
        PWM_SET_CNR(PWM0, u32TestCh, 0x100);
              
        PWM_Stop(PWM0, 1 << u32TestCh);
        CU_ASSERT_EQUAL(PWM0->PERIOD[((u32TestCh >> 1) << 1)], 0);
    }

    //test case 2: enable/disable one channel each time, then check at final action=> ignore
    for (u32TestCh = 0; u32TestCh < PWM_MAX_CH; u32TestCh++)
    {
        PWM_SET_CNR(PWM0, u32TestCh, 0x100);
    }

    for (u32TestCh = 0; u32TestCh < PWM_MAX_CH; u32TestCh++)
    {
        PWM_Stop(PWM0, 1 << u32TestCh);
    }
    CU_ASSERT_EQUAL(PWM0->PERIOD[0], 0);
    CU_ASSERT_EQUAL(PWM0->PERIOD[1], 0);
    CU_ASSERT_EQUAL(PWM0->PERIOD[2], 0);

    //test case 3: enable/disable multiple channels each time
    for (u32TestCh = 0; u32TestCh < PWM_CHANNEL_NUM; u32TestCh++)
    {
        PWM_SET_CNR(PWM0, u32TestCh, 0x100);
    }
    
    PWM_Stop(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1) | (1 << PWM_CH2) | (1 << PWM_CH3));
    CU_ASSERT_EQUAL(PWM0->PERIOD[0], 0);
    CU_ASSERT_EQUAL(PWM0->PERIOD[1], 0);
}

void Func_PWM_EnableOutput() 
{
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    //test case 1: enable/disable one channel each time, then check each action
    PWM_EnableOutput(PWM0, 1 << PWM_CH0);
    CU_ASSERT_EQUAL(PWM0->POEN & (1 << 0), 1 << 0);
    PWM_DisableOutput(PWM0, 1 << PWM_CH0);
    CU_ASSERT_EQUAL(PWM0->POEN & (1 << 0), 0 << 0);

    PWM_EnableOutput(PWM0, 1 << PWM_CH1);
    CU_ASSERT_EQUAL(PWM0->POEN & (1 << 1), 1 << 1);
    PWM_DisableOutput(PWM0, 1 << PWM_CH1);
    CU_ASSERT_EQUAL(PWM0->POEN & (1 << 1), 0 << 1);

    PWM_EnableOutput(PWM0, 1 << PWM_CH2);
    CU_ASSERT_EQUAL(PWM0->POEN & (1 << 2), 1 << 2);
    PWM_DisableOutput(PWM0, 1 << PWM_CH2);
    CU_ASSERT_EQUAL(PWM0->POEN & (1 << 2), 0 << 2);
    
    PWM_EnableOutput(PWM0, 1 << PWM_CH3);
    CU_ASSERT_EQUAL(PWM0->POEN & (1 << 3), 1 << 3);
    PWM_DisableOutput(PWM0, 1 << PWM_CH3);
    CU_ASSERT_EQUAL(PWM0->POEN & (1 << 3), 0 << 3);

    //test case 2: enable/disable one channel each time, then check at final action
    PWM_EnableOutput(PWM0, 1 << PWM_CH0);
    PWM_EnableOutput(PWM0, 1 << PWM_CH1);
    PWM_EnableOutput(PWM0, 1 << PWM_CH2);
    PWM_EnableOutput(PWM0, 1 << PWM_CH3);
    CU_ASSERT_EQUAL(PWM0->POEN, 0xF);
    
    PWM_DisableOutput(PWM0, 1 << PWM_CH0);
    PWM_DisableOutput(PWM0, 1 << PWM_CH1);
    PWM_DisableOutput(PWM0, 1 << PWM_CH2);
    PWM_DisableOutput(PWM0, 1 << PWM_CH3);
    CU_ASSERT_EQUAL(PWM0->POEN, 0);

    //test case 3: enable/disable multiple channels each time
    PWM_EnableOutput(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1));
    CU_ASSERT_EQUAL(PWM0->POEN, 0x00000003);
    
    PWM_DisableOutput(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1));
    CU_ASSERT_EQUAL(PWM0->POEN, 0);
    
    PWM_EnableOutput(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1) | (1 << PWM_CH2));
    CU_ASSERT_EQUAL(PWM0->POEN, 0x00000007);
    
    PWM_DisableOutput(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1) | (1 << PWM_CH2));
    CU_ASSERT_EQUAL(PWM0->POEN, 0);
    
    PWM_EnableOutput(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1) | (1 << PWM_CH2) | (1 << PWM_CH3));
    CU_ASSERT_EQUAL(PWM0->POEN, 0x0000000F);
    
    PWM_DisableOutput(PWM0, (1 << PWM_CH0) | (1 << PWM_CH1) | (1 << PWM_CH2) | (1 << PWM_CH3));
    CU_ASSERT_EQUAL(PWM0->POEN, 0);
}

#ifdef PWM_CUNIT_TEST_18
void Func_PWM_ConfigOutputChannel() 
{
     uint32_t u32TestCh = 0;

    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    /* Enable Internal 10KHz clock */
    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);

    /* Waiting for Internal 10KHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

    /* Switch HCLK clock source to HIRC */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC,CLK_HCLK_CLK_DIVIDER(1));

    //----------------------------
    //PWM clock is HIRC: 12MHz
    //----------------------------
    /* Switch HCLK clock source to HIRC */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC,CLK_HCLK_CLK_DIVIDER(1));

    //duty cycle is 50%
    for (u32TestCh = PWM_CH0; u32TestCh <= PWM_CH5; u32TestCh++)
    {
        //(i%2) ? (100:1000)
        PWM_ConfigOutputChannel(PWM0, u32TestCh, (u32TestCh%2) ? 100:1000, 50);
        CU_ASSERT_EQUAL(PWM0->PERIOD[(u32TestCh >> 1) << 1] & 0xffff, (u32TestCh%2) ? 59999:11999);
        CU_ASSERT_EQUAL(PWM0->CMPDAT[u32TestCh] & 0xffff, (u32TestCh%2) ? 29999:5999);
    }

    //duty cycle is 0%
    for (u32TestCh = PWM_CH0; u32TestCh <= PWM_CH5; u32TestCh++)
    {
        PWM_ConfigOutputChannel(PWM0, u32TestCh, (u32TestCh%2) ? 100:1000, 0);
        CU_ASSERT_EQUAL(PWM0->PERIOD[(u32TestCh >> 1) << 1] & 0xffff, (u32TestCh%2) ? 59999:11999);
        CU_ASSERT_EQUAL(PWM0->CMPDAT[u32TestCh] & 0xffff, 0);
    }

    //duty cycle is 100%
    for (u32TestCh = PWM_CH0; u32TestCh <= PWM_CH5; u32TestCh++)
    {
        PWM_ConfigOutputChannel(PWM0, u32TestCh, (u32TestCh%2) ? 100:1000, 100);
        CU_ASSERT_EQUAL(PWM0->PERIOD[(u32TestCh >> 1) << 1] & 0xffff, (u32TestCh%2) ? 59999:11999);
        CU_ASSERT_EQUAL(PWM0->CMPDAT[u32TestCh] & 0xffff, (u32TestCh%2) ? 59999:11999);
    }

    //----------------------------
    //PWM clock is PLL: PLL_CLOCK = 32000000
    //----------------------------
    /* Switch HCLK clock source to PLL */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_PLL,CLK_HCLK_CLK_DIVIDER(1));

    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL1_PWM0SEL_PLL, 0);
    
    //duty cycle is 50%
    for (u32TestCh = PWM_CH0; u32TestCh <= PWM_CH5; u32TestCh++)
    {
        //(i%2) ? (100:1000)
        PWM_ConfigOutputChannel(PWM0, u32TestCh, (u32TestCh%2) ? 100:1000, 50);
        CU_ASSERT_EQUAL(PWM0->PERIOD[(u32TestCh >> 1) << 1] & 0xffff, (u32TestCh%2) ? 63999:31999);
        CU_ASSERT_EQUAL(PWM0->CMPDAT[u32TestCh] & 0xffff, (u32TestCh%2) ? 31999:15999);
    }

    //duty cycle is 0%
    for (u32TestCh = PWM_CH0; u32TestCh <= PWM_CH5; u32TestCh++)
    {
        PWM_ConfigOutputChannel(PWM0, u32TestCh, (u32TestCh%2) ? 100:1000, 0);
        CU_ASSERT_EQUAL(PWM0->PERIOD[(u32TestCh >> 1) << 1] & 0xffff, (u32TestCh%2) ? 63999:31999);
        CU_ASSERT_EQUAL(PWM0->CMPDAT[u32TestCh] & 0xffff, 0);
    }

    //duty cycle is 100%
    for (u32TestCh = PWM_CH0; u32TestCh <= PWM_CH5; u32TestCh++)
    {
        PWM_ConfigOutputChannel(PWM0, u32TestCh, (u32TestCh%2) ? 100:1000, 100);
        CU_ASSERT_EQUAL(PWM0->PERIOD[(u32TestCh >> 1) << 1] & 0xffff, (u32TestCh%2) ? 63999:31999);
        CU_ASSERT_EQUAL(PWM0->CMPDAT[u32TestCh] & 0xffff, (u32TestCh%2) ? 63999:31999);
    }
}
#endif

#ifdef PWM_CUNIT_TEST_12_TO_14
void Func_PWM_EnableDeadZone() 
{
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    //test case 1: enable/disable one channel each time, then check each action
    PWM_EnableDeadZone(PWM0, PWM_CH0, 0xFF);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTEN_Msk, 1 << PWM_DTCTL0_1_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTCNT_Msk, 0xFF);
    PWM_DisableDeadZone(PWM0, PWM_CH0);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTEN_Msk, 0 << PWM_DTCTL0_1_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTCNT_Msk, 0xFF);

    PWM_EnableDeadZone(PWM0, PWM_CH1, 0);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTEN_Msk, 1 << PWM_DTCTL0_1_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTCNT_Msk, 0);
    PWM_DisableDeadZone(PWM0, PWM_CH1);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTEN_Msk, 0 << PWM_DTCTL0_1_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTCNT_Msk, 0);

    PWM_EnableDeadZone(PWM0, PWM_CH2, 0xFF);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTEN_Msk, 1 << PWM_DTCTL2_3_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTCNT_Msk, 0xFF);
    PWM_DisableDeadZone(PWM0, PWM_CH2);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTEN_Msk, 0 << PWM_DTCTL2_3_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTCNT_Msk, 0xFF);

    PWM_EnableDeadZone(PWM0, PWM_CH3, 0);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTEN_Msk, 1 << PWM_DTCTL2_3_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTCNT_Msk, 0);
    PWM_DisableDeadZone(PWM0, PWM_CH3);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTEN_Msk, 0 << PWM_DTCTL2_3_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTCNT_Msk, 0);
    
    //test case 2: enable/disable one channel each time, then check at final action
    PWM_EnableDeadZone(PWM0, PWM_CH0, 0xA5);
    PWM_EnableDeadZone(PWM0, PWM_CH1, 0xA5);
    PWM_EnableDeadZone(PWM0, PWM_CH2, 0xA5);
    PWM_EnableDeadZone(PWM0, PWM_CH3, 0xA5);
    PWM_EnableDeadZone(PWM0, PWM_CH4, 0xA5);
    PWM_EnableDeadZone(PWM0, PWM_CH5, 0xA5);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTEN_Msk, 1 << PWM_DTCTL0_1_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTEN_Msk, 1 << PWM_DTCTL2_3_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL4_5_DTEN_Msk, 1 << PWM_DTCTL4_5_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTCNT_Msk, 0xA5);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTCNT_Msk, 0xA5);
    CU_ASSERT_EQUAL(PWM0->DTCTL4_5 & PWM_DTCTL4_5_DTCNT_Msk, 0xA5);
    
    PWM_DisableDeadZone(PWM0, PWM_CH0);
    PWM_DisableDeadZone(PWM0, PWM_CH1);
    PWM_DisableDeadZone(PWM0, PWM_CH2);
    PWM_DisableDeadZone(PWM0, PWM_CH3);
    PWM_DisableDeadZone(PWM0, PWM_CH4);
    PWM_DisableDeadZone(PWM0, PWM_CH5);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTEN_Msk, 0 << PWM_DTCTL0_1_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTEN_Msk, 0 << PWM_DTCTL2_3_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL4_5_DTEN_Msk, 0 << PWM_DTCTL4_5_DTEN_Pos);
    CU_ASSERT_EQUAL(PWM0->DTCTL0_1 & PWM_DTCTL0_1_DTCNT_Msk, 0xA5);
    CU_ASSERT_EQUAL(PWM0->DTCTL2_3 & PWM_DTCTL2_3_DTCNT_Msk, 0xA5);
    CU_ASSERT_EQUAL(PWM0->DTCTL4_5 & PWM_DTCTL4_5_DTCNT_Msk, 0xA5);
}
#endif

#ifdef PWM_CUNIT_TEST_20_TO_21
void Func_PWM_EnablePeriodInt() 
{
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    //test case : enable/disable one channel each time, then check at final action
    PWM_EnablePeriodInt(PWM0, PWM_CH0, 0);
    PWM_EnablePeriodInt(PWM0, PWM_CH1, 0);
    PWM_EnablePeriodInt(PWM0, PWM_CH2, 0);
    PWM_EnablePeriodInt(PWM0, PWM_CH3, 0);
    PWM_EnablePeriodInt(PWM0, PWM_CH4, 0);
    PWM_EnablePeriodInt(PWM0, PWM_CH5, 0);
    CU_ASSERT_EQUAL(PWM0->INTEN0, 0x1500);
    
    PWM_DisablePeriodInt(PWM0, PWM_CH0);
    PWM_DisablePeriodInt(PWM0, PWM_CH1);
    PWM_DisablePeriodInt(PWM0, PWM_CH2);
    PWM_DisablePeriodInt(PWM0, PWM_CH3);
    PWM_DisablePeriodInt(PWM0, PWM_CH4);
    PWM_DisablePeriodInt(PWM0, PWM_CH5);
    CU_ASSERT_EQUAL(PWM0->INTEN0, 0);
}
#endif

#ifdef PWM_CUNIT_TEST_22_TO_23
void Func_PWM_ConfigCaptureChannel() 
{
     uint32_t u32TestCh = 0;

    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    /* Enable Internal 10KHz clock */
    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);

    /* Waiting for Internal 10KHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk);

    /* Switch HCLK clock source to HXT */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_HCLK_CLK_DIVIDER(1));

    //----------------------------
    //PWM clock is HCLK HXT: 12MHz
    //----------------------------
    CLK_SetModuleClock(PWM0_MODULE, CLK_CLKSEL1_PWM0SEL_PCLK0, 0);

    //duty cycle is 50%
    for (u32TestCh = PWM_CH0; u32TestCh <= PWM_CH3; u32TestCh++)
    {
        //(i%2) ? (100:1000)
        PWM_ConfigCaptureChannel(PWM0, u32TestCh, (u32TestCh%2) ? 100:1000, 50);
        CU_ASSERT_EQUAL(PWM0->PERIOD[(u32TestCh >> 1) << 1] & 0xffff, (u32TestCh%2) ? 65535:65535);
    }

}

void Func_PWM_EnableCapture()
{
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    //test case : enable/disable capture, then check at action
    PWM_EnableCapture(PWM0, PWM_CH_0_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPCTL & PWM_CH_0_MASK, PWM_CH_0_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPINEN & PWM_CH_0_MASK, PWM_CH_0_MASK);
    PWM_EnableCapture(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPCTL & (PWM_CH_0_MASK | PWM_CH_1_MASK), PWM_CH_0_MASK | PWM_CH_1_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPINEN & (PWM_CH_0_MASK | PWM_CH_1_MASK), PWM_CH_0_MASK | PWM_CH_1_MASK);
    PWM_EnableCapture(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPCTL & (PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK), PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPINEN & (PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK), PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK);
    PWM_EnableCapture(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPCTL & (PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK), PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPINEN & (PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK), PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);
    
    PWM_DisableCapture(PWM0, PWM_CH_0_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPCTL & PWM_CH_0_MASK, 0);
    CU_ASSERT_EQUAL(PWM0->CAPINEN & PWM_CH_0_MASK, 0);
    PWM_DisableCapture(PWM0, PWM_CH_1_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPCTL & PWM_CH_1_MASK, 0);
    CU_ASSERT_EQUAL(PWM0->CAPINEN & PWM_CH_1_MASK, 0);
    PWM_DisableCapture(PWM0, PWM_CH_2_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPCTL & PWM_CH_2_MASK, 0);
    CU_ASSERT_EQUAL(PWM0->CAPINEN & PWM_CH_2_MASK, 0);
    PWM_DisableCapture(PWM0, PWM_CH_3_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPCTL & PWM_CH_3_MASK, 0);
    CU_ASSERT_EQUAL(PWM0->CAPINEN & PWM_CH_3_MASK, 0);
    
    //test case : enable all capture, then disable all capture , finally check CTL and CAPCTL register 
    PWM_EnableCapture(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPCTL & (PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK), PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPINEN & (PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK), PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);
    PWM_DisableCapture(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);
    CU_ASSERT_EQUAL(PWM0->CAPCTL & (PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK), 0);
    CU_ASSERT_EQUAL(PWM0->CAPINEN & (PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK), 0);
}
#endif

#ifdef PWM_CUNIT_TEST_24_TO_25
void Func_PWM_EnableCaptureInt()
{
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    PWM_EnableCaptureInt (PWM0,  PWM_CH0, PWM_CAPTURE_INT_RISING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (1 << PWM_CH0), 1 << PWM_CH0);
    PWM_EnableCaptureInt (PWM0,  PWM_CH1, PWM_CAPTURE_INT_RISING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (1 << PWM_CH1), 1 << PWM_CH1);
    PWM_EnableCaptureInt (PWM0,  PWM_CH2, PWM_CAPTURE_INT_RISING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (1 << PWM_CH2), 1 << PWM_CH2);
    PWM_EnableCaptureInt (PWM0,  PWM_CH3, PWM_CAPTURE_INT_RISING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (1 << PWM_CH3), 1 << PWM_CH3);
    PWM_EnableCaptureInt (PWM0,  PWM_CH4, PWM_CAPTURE_INT_RISING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (1 << PWM_CH4), 1 << PWM_CH4);
    PWM_EnableCaptureInt (PWM0,  PWM_CH5, PWM_CAPTURE_INT_RISING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (1 << PWM_CH5), 1 << PWM_CH5);

  
    PWM_EnableCaptureInt (PWM0,  PWM_CH0, PWM_CAPTURE_INT_FALLING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (0x100 << PWM_CH0), 0x100 << PWM_CH0);
    PWM_EnableCaptureInt (PWM0,  PWM_CH1, PWM_CAPTURE_INT_FALLING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (0x100 << PWM_CH1), 0x100 << PWM_CH1);
    PWM_EnableCaptureInt (PWM0,  PWM_CH2, PWM_CAPTURE_INT_FALLING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (0x100 << PWM_CH2), 0x100 << PWM_CH2);
    PWM_EnableCaptureInt (PWM0,  PWM_CH3, PWM_CAPTURE_INT_FALLING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (0x100 << PWM_CH3), 0x100 << PWM_CH3);
    PWM_EnableCaptureInt (PWM0,  PWM_CH4, PWM_CAPTURE_INT_FALLING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (0x100 << PWM_CH4), 0x100 << PWM_CH4);
    PWM_EnableCaptureInt (PWM0,  PWM_CH5, PWM_CAPTURE_INT_FALLING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIEN & (0x100 << PWM_CH5), 0x100 << PWM_CH5);
}

void Func_PWM_GetCaptureIntFlag()
{
    uint32_t i;
    
    // Use self test mode for capture test

    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);
  
    //PWM0_Ch0: PA12
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA12MFP_Msk));
    GPIO_SetMode(PA, BIT12, GPIO_PMD_OUTPUT);

    //PWM0_Ch1: PA13
    SYS->GPA_MFPH = (SYS->GPA_MFPH & (~SYS_GPA_MFPH_PA13MFP_Msk));
    GPIO_SetMode(PA, BIT13, GPIO_PMD_OUTPUT);

    //PWM0_Ch2: PA0
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA0MFP_Msk));
    GPIO_SetMode(PA, BIT0, GPIO_PMD_OUTPUT);
    
    //PWM0_Ch3: PA6
    SYS->GPA_MFPL = (SYS->GPA_MFPL & (~SYS_GPA_MFPL_PA6MFP_Msk));
    GPIO_SetMode(PA, BIT6, GPIO_PMD_OUTPUT);
    
    //PWM0_Ch4: PB11
    SYS->GPB_MFPH = (SYS->GPB_MFPH & (~SYS_GPB_MFPH_PB11MFP_Msk));
    GPIO_SetMode(PB, BIT11, GPIO_PMD_OUTPUT);

    //PWM0_Ch5: PE5
    SYS->GPE_MFPL = (SYS->GPE_MFPL & (~SYS_GPE_MFPL_PE5MFP_Msk));
    GPIO_SetMode(PE, BIT5, GPIO_PMD_OUTPUT);
    
    //Self test mode
    if (1) {
        uint32_t volatile *ptr;
        ptr = (uint32_t volatile *)0x50000014;
        *ptr |= 1;
    }

    //Ch0: PA12, Ch1: PA13, Ch2: PA0, Ch3: PA6, Ch4: PB11, Ch5: PE5
    PA->MODE = (PA->MODE & ~((0x3 << 24) | (0x3 < 26) | (0x3) | (0x3 < 12))) | (1 << 24) | (1 << 26) | (1) | (1 << 12);
    PB->MODE = (PB->MODE & ~(0x3 << 22)) | (1 << 22);
    PE->MODE = (PE->MODE & ~(0x3 << 10)) | (1 << 10);
    PA12 = 1;
    PA13 = 1;
    PA0 = 1;
    PA6 = 1;
    PB11 = 1;
    PE5 = 1;

    PWM_ConfigCaptureChannel(PWM0, 0, 5, 0);
    PWM_ConfigCaptureChannel(PWM0, 1, 5, 0);
    PWM_ConfigCaptureChannel(PWM0, 2, 5, 0);
    PWM_ConfigCaptureChannel(PWM0, 3, 5, 0);
    PWM_ConfigCaptureChannel(PWM0, 4, 5, 0);
    PWM_ConfigCaptureChannel(PWM0, 5, 5, 0);

    PWM0->CAPCTL |= PWM_CAPCTL_FCRLDENn_Msk | PWM_CAPCTL_RCRLDENn_Msk;

    PWM_EnableCapture(PWM0, 0x3F);
    PWM_Start(PWM0, 0x3F);
    
    PA12 = 0;
    PA13 = 0;
    PA0 = 0;
    PA6 = 0;
    PB11 = 0;
    PE5 = 0;
    for(i = 0; i < 1000; i++);
    PA12 = 1;
    PA13 = 1;
    PA0 = 1;
    PA6 = 1;
    PB11 = 1;
    PE5 = 1;
    for(i = 0; i < 1000; i++);

    i = PWM_GetCaptureIntFlag(PWM0, PWM_CH0);
    CU_ASSERT_EQUAL(i, 3);
    CU_ASSERT_EQUAL(PWM0->CAPIF, 0x3f3f);
    PWM_ClearCaptureIntFlag (PWM0, PWM_CH0, PWM_CAPTURE_INT_RISING_LATCH | PWM_CAPTURE_INT_FALLING_LATCH);
    PWM_ClearCaptureIntFlag (PWM0, PWM_CH1, PWM_CAPTURE_INT_RISING_LATCH | PWM_CAPTURE_INT_FALLING_LATCH);
    PWM_ClearCaptureIntFlag (PWM0, PWM_CH2, PWM_CAPTURE_INT_RISING_LATCH | PWM_CAPTURE_INT_FALLING_LATCH);
    PWM_ClearCaptureIntFlag (PWM0, PWM_CH3, PWM_CAPTURE_INT_RISING_LATCH | PWM_CAPTURE_INT_FALLING_LATCH);
    PWM_ClearCaptureIntFlag (PWM0, PWM_CH4, PWM_CAPTURE_INT_RISING_LATCH | PWM_CAPTURE_INT_FALLING_LATCH);
    PWM_ClearCaptureIntFlag (PWM0, PWM_CH5, PWM_CAPTURE_INT_RISING_LATCH | PWM_CAPTURE_INT_FALLING_LATCH);
    CU_ASSERT_EQUAL(PWM0->CAPIF, 0);
    
    //Disable self test mode
    if (1) {
        uint32_t volatile *ptr;
        ptr = (uint32_t volatile *)0x50000014;
        *ptr &= ~1;
    }
    
    PA->MODE &= ~((0x3 << 24) | (0x3 < 26) | (0x3) | (0x3 < 12));
    PB->MODE &= ~(0x3 << 22);
    PE->MODE &= ~(0x3 << 10);
    SYS->IPRST1 |= SYS_IPRST2_PWM0RST_Msk;
    SYS->IPRST1 &= ~SYS_IPRST2_PWM0RST_Msk;

}
#endif

#ifdef PWM_CUNIT_TEST_26
void Func_PWM_EnablePDMA()
{
    //Not support PDMA
}
#endif

#ifdef PWM_CUNIT_TEST_27_to_28
void Func_PWM_EnableADCTrigger()
{
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    PWM_EnableADCTrigger (PWM0, PWM_CH0, PWM_TRIGGER_ADC_EVEN_ZERO_POINT);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGEN0_Msk, PWM_ADCTS0_TRGEN0_Msk);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGSEL0_Msk, (PWM_TRIGGER_ADC_EVEN_ZERO_POINT) << PWM_ADCTS0_TRGSEL0_Pos);
    PWM_EnableADCTrigger (PWM0, PWM_CH1, PWM_TRIGGER_ADC_EVEN_PERIOD_POINT);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGEN1_Msk, PWM_ADCTS0_TRGEN1_Msk);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGSEL1_Msk, (PWM_TRIGGER_ADC_EVEN_PERIOD_POINT) << PWM_ADCTS0_TRGSEL1_Pos);
    PWM_EnableADCTrigger (PWM0, PWM_CH2, PWM_TRIGGER_ADC_EVEN_ZERO_OR_PERIOD_POINT);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGEN2_Msk, PWM_ADCTS0_TRGEN2_Msk);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGSEL2_Msk, (PWM_TRIGGER_ADC_EVEN_ZERO_OR_PERIOD_POINT) << PWM_ADCTS0_TRGSEL2_Pos);
    PWM_EnableADCTrigger (PWM0, PWM_CH3, PWM_TRIGGER_ADC_EVEN_COMPARE_UP_COUNT_POINT);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGEN3_Msk, PWM_ADCTS0_TRGEN3_Msk);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGSEL3_Msk, (PWM_TRIGGER_ADC_EVEN_COMPARE_UP_COUNT_POINT) << PWM_ADCTS0_TRGSEL3_Pos);
    PWM_EnableADCTrigger (PWM0, PWM_CH4, PWM_TRIGGER_ADC_EVEN_COMPARE_DOWN_COUNT_POINT);
    CU_ASSERT_EQUAL(PWM0->ADCTS1 & PWM_ADCTS1_TRGEN4_Msk, PWM_ADCTS1_TRGEN4_Msk);
    CU_ASSERT_EQUAL(PWM0->ADCTS1 & PWM_ADCTS1_TRGSEL4_Msk, (PWM_TRIGGER_ADC_EVEN_COMPARE_DOWN_COUNT_POINT) << PWM_ADCTS1_TRGSEL4_Pos);
    PWM_EnableADCTrigger (PWM0, PWM_CH5, PWM_TRIGGER_ADC_ODD_COMPARE_UP_COUNT_POINT);
    CU_ASSERT_EQUAL(PWM0->ADCTS1 & PWM_ADCTS1_TRGEN5_Msk, PWM_ADCTS1_TRGEN5_Msk);
    CU_ASSERT_EQUAL(PWM0->ADCTS1 & PWM_ADCTS1_TRGSEL5_Msk, (PWM_TRIGGER_ADC_ODD_COMPARE_UP_COUNT_POINT) << PWM_ADCTS1_TRGSEL5_Pos);
    PWM_EnableADCTrigger (PWM0, PWM_CH0, PWM_TRIGGER_ADC_ODD_COMPARE_DOWN_COUNT_POINT);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGEN0_Msk, PWM_ADCTS0_TRGEN0_Msk);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGSEL0_Msk, (PWM_TRIGGER_ADC_ODD_COMPARE_DOWN_COUNT_POINT) << PWM_ADCTS0_TRGSEL0_Pos);

    PWM_DisableADCTrigger (PWM0, PWM_CH0);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGEN0_Msk, 0);
    PWM_DisableADCTrigger (PWM0, PWM_CH1);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGEN1_Msk, 0);
    PWM_DisableADCTrigger (PWM0, PWM_CH2);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGEN2_Msk, 0);
    PWM_DisableADCTrigger (PWM0, PWM_CH3);
    CU_ASSERT_EQUAL(PWM0->ADCTS0 & PWM_ADCTS0_TRGEN3_Msk, 0);
    PWM_DisableADCTrigger (PWM0, PWM_CH4);
    CU_ASSERT_EQUAL(PWM0->ADCTS1 & PWM_ADCTS1_TRGEN4_Msk, 0);
    PWM_DisableADCTrigger (PWM0, PWM_CH5);
    CU_ASSERT_EQUAL(PWM0->ADCTS1 & PWM_ADCTS1_TRGEN5_Msk, 0);

}

void Func_PWM_GetADCTriggerFlag()
{
    uint32_t flag;
  
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);
  
    flag = PWM_GetADCTriggerFlag (PWM0, PWM_CH0);
    CU_ASSERT_EQUAL(flag, 0);
    PWM_ClearADCTriggerFlag (PWM0, PWM_CH0, 0);
    CU_ASSERT_EQUAL(PWM0->STATUS & ((1 << PWM_STATUS_ADCTRGn_Pos) << PWM_CH0), 0);
}
#endif

#ifdef PWM_CUNIT_TEST_1_TO_2
void Func_PWM_ENABLE_OUTPUT_INVERTER() 
{
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    //test case 1: enable one channel each time, then check each action
    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH0);
    CU_ASSERT_EQUAL(PWM0->POLCTL & (1 << 0), 1 << 0);

    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH1);
    CU_ASSERT_EQUAL(PWM0->POLCTL & (1 << 1), 1 << 1);

    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH2);
    CU_ASSERT_EQUAL(PWM0->POLCTL & (1 << 2), 1 << 2);

    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH3);
    CU_ASSERT_EQUAL(PWM0->POLCTL & (1 << 3), 1 << 3);

    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH4);
    CU_ASSERT_EQUAL(PWM0->POLCTL & (1 << 4), 1 << 4);

    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH5);
    CU_ASSERT_EQUAL(PWM0->POLCTL & (1 << 5), 1 << 5);
    
    //test case 2: enable one channel each time, then check at final action
    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH0);
    CU_ASSERT_EQUAL(PWM0->POLCTL, 1 << PWM_CH0);
    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH1);
    CU_ASSERT_EQUAL(PWM0->POLCTL, 1 << PWM_CH1);
    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH2);
    CU_ASSERT_EQUAL(PWM0->POLCTL, 1 << PWM_CH2);
    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH3);
    CU_ASSERT_EQUAL(PWM0->POLCTL, 1 << PWM_CH3);
    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH4);
    CU_ASSERT_EQUAL(PWM0->POLCTL, 1 << PWM_CH4);
    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 1 << PWM_CH5);
    CU_ASSERT_EQUAL(PWM0->POLCTL, 1 << PWM_CH5);
    
    //test case 3: enable/disable multiple channels each time
    PWM_ENABLE_OUTPUT_INVERTER(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);
    CU_ASSERT_EQUAL(PWM0->POLCTL, 0xf);

    PWM_ENABLE_OUTPUT_INVERTER(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK);
    CU_ASSERT_EQUAL(PWM0->POLCTL, 0x7);

    PWM_ENABLE_OUTPUT_INVERTER(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK);
    CU_ASSERT_EQUAL(PWM0->POLCTL, 0x3);

    PWM_ENABLE_OUTPUT_INVERTER(PWM0, PWM_CH_0_MASK);
    CU_ASSERT_EQUAL(PWM0->POLCTL, 0x1);

    PWM_ENABLE_OUTPUT_INVERTER(PWM0, 0);
    CU_ASSERT_EQUAL(PWM0->POLCTL, 0);
}

void Func_PWM_SET_PRESCALER() 
{
    uint32_t u32TestCh = 0;
    uint32_t u32PrescalerPatternIdx = 0, u32PrescalerPatternCount = 0, u32PrescalerPatternData = 0;

    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);
    
    u32PrescalerPatternCount = sizeof(EightBitsPatternTable)/sizeof(uint32_t);
    
    for (u32TestCh = 0; u32TestCh < PWM_CHANNEL_NUM; u32TestCh++)
    {
        for (u32PrescalerPatternIdx = 0; u32PrescalerPatternIdx < u32PrescalerPatternCount; u32PrescalerPatternIdx++)
        {
            u32PrescalerPatternData = EightBitsPatternTable[u32PrescalerPatternIdx];

					  switch(u32TestCh)
					  {
								case 0:
								case 1:
								{
										PWM_SET_PRESCALER(PWM0, u32TestCh, u32PrescalerPatternData);
										CU_ASSERT_EQUAL(PWM0->CLKPSC0_1 & PWM_CLKPSC0_1_CLKPSC_Msk, u32PrescalerPatternData << 0);
								}
								break;

								case 2:
								case 3:
								{
										PWM_SET_PRESCALER(PWM0, u32TestCh, u32PrescalerPatternData);
										CU_ASSERT_EQUAL(PWM0->CLKPSC2_3 & PWM_CLKPSC2_3_CLKPSC_Msk, u32PrescalerPatternData << 0);
								}
								break;
                
                case 4:
								case 5:
								{
										PWM_SET_PRESCALER(PWM0, u32TestCh, u32PrescalerPatternData);
										CU_ASSERT_EQUAL(PWM0->CLKPSC4_5 & PWM_CLKPSC4_5_CLKPSC_Msk, u32PrescalerPatternData << 0);
								}
								break;
								
						}
        }
    }
}
#endif

#ifdef PWM_CUNIT_TEST_3_TO_5
void Func_PWM_SET_DIVIDER() 
{
    //Not support clock divider
}

void Func_PWM_SET_CMR() 
{
    uint32_t u32TestCh = 0;
    uint32_t u32PrescalerPatternIdx = 0, u32PrescalerPatternCount = 0, u32PrescalerPatternData = 0;

    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);
    
    u32PrescalerPatternCount = sizeof(SixteenBitsPatternTable)/sizeof(uint32_t);
    
    for (u32TestCh = 0; u32TestCh < PWM_CHANNEL_NUM; u32TestCh++)
    {
        for (u32PrescalerPatternIdx = 0; u32PrescalerPatternIdx < u32PrescalerPatternCount; u32PrescalerPatternIdx++)
        {
            u32PrescalerPatternData = SixteenBitsPatternTable[u32PrescalerPatternIdx];

            PWM_SET_CMR(PWM0, u32TestCh, u32PrescalerPatternData);
            CU_ASSERT_EQUAL(PWM0->CMPDAT[u32TestCh] & PWM_CMPDAT0_CMPDAT_Msk, (u32PrescalerPatternData << PWM_CMPDAT0_CMPDAT_Pos));
            
            if (u32TestCh % 2 == 0) {
                PWM_SET_CNR(PWM0, u32TestCh, u32PrescalerPatternData);
                CU_ASSERT_EQUAL(PWM0->PERIOD[u32TestCh] & PWM_PERIOD0_PERIOD_Msk, u32PrescalerPatternData);
            }
        }
    }   
}

void Func_PWM_GET_CAPTURE_RISING_DATA()
{
    uint32_t data;
  
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    data = PWM_GET_CAPTURE_RISING_DATA(PWM0, PWM_CH0);
    CU_ASSERT_EQUAL(data,0);
    data = PWM_GET_CAPTURE_FALLING_DATA(PWM0, PWM_CH0);
    CU_ASSERT_EQUAL(data,0);
}
#endif

#ifdef PWM_CUNIT_TEST_6
void Func_PWM_SET_ALIGNED_TYPE()
{
    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    // check edge align
    PWM_SET_ALIGNED_TYPE(PWM0, PWM_CH_0_MASK, PWM_EDGE_ALIGNED);
    CU_ASSERT_EQUAL(PWM0->CTL1 & PWM_CTL1_CNTTYPE0_Msk,1);
    PWM_SET_ALIGNED_TYPE(PWM0, PWM_CH_2_MASK, PWM_EDGE_ALIGNED);
    CU_ASSERT_EQUAL(PWM0->CTL1 & PWM_CTL1_CNTTYPE2_Msk, 1 << PWM_CTL1_CNTTYPE2_Pos);
    PWM_SET_ALIGNED_TYPE(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK, PWM_EDGE_ALIGNED);
    CU_ASSERT_EQUAL(PWM0->CTL1 & (PWM_CTL1_CNTTYPE0_Msk | PWM_CTL1_CNTTYPE2_Msk), 0x11);
    PWM_SET_ALIGNED_TYPE(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK | PWM_CH_4_MASK | PWM_CH_5_MASK, PWM_EDGE_ALIGNED);
    CU_ASSERT_EQUAL(PWM0->CTL1 & (PWM_CTL1_CNTTYPE0_Msk | PWM_CTL1_CNTTYPE2_Msk | PWM_CTL1_CNTTYPE4_Msk), 0x111);
  
    // check center align
    PWM_SET_ALIGNED_TYPE(PWM0, PWM_CH_0_MASK, PWM_CENTER_ALIGNED);
    CU_ASSERT_EQUAL(PWM0->CTL1 & PWM_CTL1_CNTTYPE0_Msk,2);
    PWM_SET_ALIGNED_TYPE(PWM0, PWM_CH_2_MASK, PWM_CENTER_ALIGNED);
    CU_ASSERT_EQUAL(PWM0->CTL1 & PWM_CTL1_CNTTYPE2_Msk, 2 << PWM_CTL1_CNTTYPE2_Pos);
    PWM_SET_ALIGNED_TYPE(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK, PWM_CENTER_ALIGNED);
    CU_ASSERT_EQUAL(PWM0->CTL1 & (PWM_CTL1_CNTTYPE0_Msk | PWM_CTL1_CNTTYPE2_Msk), 0x22);
    PWM_SET_ALIGNED_TYPE(PWM0, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK | PWM_CH_4_MASK | PWM_CH_5_MASK, PWM_CENTER_ALIGNED);
    CU_ASSERT_EQUAL(PWM0->CTL1 & (PWM_CTL1_CNTTYPE0_Msk | PWM_CTL1_CNTTYPE2_Msk | PWM_CTL1_CNTTYPE4_Msk), 0x222);
}
#endif

#ifdef PWM_CUNIT_TEST_20_TO_21
void Func_PWM_GetPeriodIntFlag() 
{
    uint32_t u32TestCh = 0;

    /* Reset PWM */
    SYS_ResetModule(PWM0_RST);

    for (u32TestCh = 0; u32TestCh < PWM_CHANNEL_NUM; u32TestCh++)
    {
        PWM_SET_CNR(PWM0, u32TestCh, 10);
        PWM_SET_CMR(PWM0, u32TestCh, 5);
        PWM_SET_PRESCALER(PWM0, u32TestCh, 2);
        PWM_EnablePeriodInt(PWM0, u32TestCh, 0);
        PWM_Start(PWM0, 1 << u32TestCh);        

        //Wait 1 Period interrupt flags
        while(PWM_GetPeriodIntFlag(PWM0, u32TestCh) == 0);
        //CU_ASSERT_EQUAL(PWM_GetPeriodIntFlag(PWM0, u32TestCh), 1);

        PWM_DisablePeriodInt(PWM0, u32TestCh);
        PWM_ForceStop(PWM0, 1 << u32TestCh);
        
        PWM_ClearPeriodIntFlag(PWM0, u32TestCh);
        CU_ASSERT_EQUAL(PWM0->INTSTS0 & (PWM_INTSTS0_PIF0_Msk << ((u32TestCh >> 1) << 1)), 0);
    }
}
#endif

CU_TestInfo PWM_MACRO[] =
{
#ifdef PWM_CUNIT_TEST_1_TO_2
    {"1.PWM_ENABLE_OUTPUT_INVERTER()", Func_PWM_ENABLE_OUTPUT_INVERTER},
    {"2.PWM_SET_PRESCALER()", Func_PWM_SET_PRESCALER},
#endif

#ifdef PWM_CUNIT_TEST_3_TO_5
    {"3.PWM_SET_DIVIDER()", Func_PWM_SET_DIVIDER},
    {"4.PWM_SET_CMR()/PWM_SET_CNR()", Func_PWM_SET_CMR},
    {"5.PWM_GET_CAPTURE_RISING_DATA()/PWM_GET_CAPTURE_FALLING_DATA()", Func_PWM_GET_CAPTURE_RISING_DATA},
#endif

#ifdef PWM_CUNIT_TEST_6
    {"6.PWM_SET_ALIGNED_TYPE()", Func_PWM_SET_ALIGNED_TYPE},
#endif

    CU_TEST_INFO_NULL
};

CU_TestInfo PWM_API[] =
{
#ifdef PWM_CUNIT_TEST_9_TO_11
    {"9. PWM_Start()/PWM_ForceStop()/PWM_Stop()", Func_PWM_Stop},
    {"11.PWM_EnableOutput()/PWM_DisableOutput()", Func_PWM_EnableOutput},
#endif

#ifdef PWM_CUNIT_TEST_12_TO_14
    {"12.PWM_EnableDeadZone()/PWM_DisableDeadZone()", Func_PWM_EnableDeadZone},
#endif

#ifdef PWM_CUNIT_TEST_18
    {"18.PWM_ConfigOutputChannel()", Func_PWM_ConfigOutputChannel},
#endif

#ifdef PWM_CUNIT_TEST_20_TO_21
    {"20.PWM_EnableZeroPointInt()", Func_PWM_EnablePeriodInt},
    {"21.PWM_GetZeroPointIntFlag()", Func_PWM_GetPeriodIntFlag},
#endif

#ifdef PWM_CUNIT_TEST_22_TO_23
    {"22.PWM_ConfigCaptureChannel()", Func_PWM_ConfigCaptureChannel},
    {"23.PWM_EnableCapture()/PWM_DisableCapture()", Func_PWM_EnableCapture},
#endif

#ifdef PWM_CUNIT_TEST_24_TO_25
    {"24.PWM_EnableCaptureInt()/PWM_DisableCaptureInt()", Func_PWM_EnableCaptureInt},
    {"25.PWM_GetCaptureIntFlag()/PWM_ClearCaptureIntFlag()", Func_PWM_GetCaptureIntFlag},
#endif
    
#ifdef PWM_CUNIT_TEST_26
    {"26.PWM_EnablePDMA()/PWM_DisablePDMA()", Func_PWM_EnablePDMA},
#endif

#ifdef PWM_CUNIT_TEST_27_to_28
    {"27.PWM_EnableADCTrigger()/PWM_DisableADCTrigger()/", Func_PWM_EnableADCTrigger},
    {"28.PWM_GetADCTriggerFlag()/PWM_ClearADCTriggerFlag()/", Func_PWM_GetADCTriggerFlag},
#endif    

    CU_TEST_INFO_NULL
};
