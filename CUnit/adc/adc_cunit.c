/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                             */
/*                                                                                                         */
/* Copyright (c) 2015 by Nuvoton Technology Corporation                                                    */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   adc_cunit.c                                                                                           */
/*            The test function of ADC for CUnit.                                                          */
/* Project:   NANO103                                                                                     */
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
#include "adc_cunit.h"


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
/*  Constant Definitions                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
#define ADC_MAX_CH 8
uint32_t AdcPatternTable[3] = {0x3FF, 0x3A5, 0x000}; /* 12 bits test pattern */

#define ADC_MAX_CMP_MATCH_COUNT 16

//#define MACRO_TEST
//#define MACRO2_TEST
#define API_TEST
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int suite_success_init(void) 
{
    /* Enable Internal RC 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRC0EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRC0STB_Msk);

    /* Enable ADC module clock */
    CLK_EnableModuleClock(ADC_MODULE);

    /* Select ADC module clock source to HIRC */
    CLK_SetModuleClock(ADC_MODULE,CLK_CLKSEL1_ADCSEL_HIRC,CLK_ADC_CLK_DIVIDER(5));

    /* Reset ADC */
    SYS_ResetModule(ADC_RST);

    return 0; 
}

int suite_success_clean(void)
{
    /* Enable ADC module clock */
    CLK->APBCLK &= ~CLK_APBCLK_ADCCKEN_Msk;
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

#ifdef MACRO_TEST
CU_SuiteInfo suites[] =
{
    {"ADC MACRO", suite_success_init, suite_success_clean, NULL, NULL, ADC_MACRO},
    //{"ADC API", suite_success_init, suite_success_clean, NULL, NULL, ADC_API},
    //{"ADC CONSTANT", suite_success_init, suite_success_clean, NULL, NULL, ADC_CONSTANT},
    CU_SUITE_INFO_NULL
};
#endif

#ifdef MACRO2_TEST
CU_SuiteInfo suites[] =
{
    {"ADC MACRO2", suite_success_init, suite_success_clean, NULL, NULL, ADC_MACRO2},
    //{"ADC API", suite_success_init, suite_success_clean, NULL, NULL, ADC_API},
    //{"ADC CONSTANT", suite_success_init, suite_success_clean, NULL, NULL, ADC_CONSTANT},
    CU_SUITE_INFO_NULL
};
#endif

#ifdef API_TEST
CU_SuiteInfo suites[] =
{
    //{"ADC MACRO", suite_success_init, suite_success_clean, NULL, NULL, ADC_MACRO},
    {"ADC API", suite_success_init, suite_success_clean, NULL, NULL, ADC_API},
    {"ADC CONSTANT", suite_success_init, suite_success_clean, NULL, NULL, ADC_CONSTANT},
    CU_SUITE_INFO_NULL
};
#endif

#ifdef MACRO_TEST
void MACRO_ADC_ENABLE_DISABLE_CMP0() 
{
    uint32_t u32TestCh = 0;
    uint32_t u32CMPDPatternIndex = 0, u32CMPDPatternCount = 0, u32CMPDPatternData = 0;
    uint32_t u32TestCMPMatchCount = 0;
    
    u32CMPDPatternCount = sizeof(AdcPatternTable)/sizeof(uint32_t);
    
    for (u32TestCh = 0; u32TestCh < ADC_MAX_CH; u32TestCh++)
    {
        for (u32CMPDPatternIndex = 0; u32CMPDPatternIndex < u32CMPDPatternCount; u32CMPDPatternIndex++)
        {
            for (u32TestCMPMatchCount = 1; u32TestCMPMatchCount <= ADC_MAX_CMP_MATCH_COUNT; u32TestCMPMatchCount++)
            {
                u32CMPDPatternData = AdcPatternTable[u32CMPDPatternIndex];

                ADC_ENABLE_CMP0(ADC, u32TestCh, ADC_CMP_LESS_THAN, u32CMPDPatternData, u32TestCMPMatchCount);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPCH_Msk), u32TestCh << 3);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPCOND_Msk), 0);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPCOND_Msk), ADC_CMP_LESS_THAN << ADC_CMP0_CMPCOND_Pos);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPDAT_Msk), AdcPatternTable[u32CMPDPatternIndex] << 16);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPMCNT_Msk), (u32TestCMPMatchCount - 1) << 8);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP1_ADCMPEN_Msk), 1);
                
                ADC_ENABLE_CMP0(ADC, u32TestCh, ADC_CMP_GREATER_OR_EQUAL_TO, u32CMPDPatternData, u32TestCMPMatchCount);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPCH_Msk), u32TestCh << 3);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPCOND_Msk), 1 << 2);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPCOND_Msk), ADC_CMP_GREATER_OR_EQUAL_TO);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPDAT_Msk), AdcPatternTable[u32CMPDPatternIndex] << 16);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPMCNT_Msk), (u32TestCMPMatchCount - 1) << 8);
                CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP1_ADCMPEN_Msk), 1);
                
                ADC_DISABLE_CMP0(ADC);
                CU_ASSERT_EQUAL(ADC->CMP0, 0);
            }
        }
    }
}

void MACRO_ADC_ENABLE_DISABLE_CMP1() 
{
    uint32_t u32TestCh = 0;
    uint32_t u32CMPDPatternIndex = 0, u32CMPDPatternCount = 0, u32CMPDPatternData = 0;
    uint32_t u32TestCMPMatchCount = 0;
    
    u32CMPDPatternCount = sizeof(AdcPatternTable)/sizeof(uint32_t);
    
    for (u32TestCh = 0; u32TestCh < ADC_MAX_CH; u32TestCh++)
    {
        for (u32CMPDPatternIndex = 0; u32CMPDPatternIndex < u32CMPDPatternCount; u32CMPDPatternIndex++)
        {
            for (u32TestCMPMatchCount = 1; u32TestCMPMatchCount <= ADC_MAX_CMP_MATCH_COUNT; u32TestCMPMatchCount++)
            {
                u32CMPDPatternData = AdcPatternTable[u32CMPDPatternIndex];

                ADC_ENABLE_CMP1(ADC, u32TestCh, ADC_CMP_LESS_THAN, u32CMPDPatternData, u32TestCMPMatchCount);
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPCH_Msk), u32TestCh << 3);
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPCOND_Msk), 0);
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPCOND_Msk), ADC_CMP_LESS_THAN << ADC_CMP1_CMPCOND_Pos);
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPDAT_Msk), AdcPatternTable[u32CMPDPatternIndex] << 16);
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPMCNT_Msk), (u32TestCMPMatchCount - 1) << 8);
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_ADCMPEN_Msk), 1);
                
                ADC_ENABLE_CMP1(ADC, u32TestCh, ADC_CMP_GREATER_OR_EQUAL_TO, u32CMPDPatternData, u32TestCMPMatchCount);
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPCH_Msk), u32TestCh << 3);
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPCOND_Msk), 1 << 2); 
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPCOND_Msk), ADC_CMP_GREATER_OR_EQUAL_TO);
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPDAT_Msk), AdcPatternTable[u32CMPDPatternIndex] << 16);
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPMCNT_Msk), (u32TestCMPMatchCount - 1) << 8);
                CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_ADCMPEN_Msk), 1);
                
                ADC_DISABLE_CMP1(ADC);
                CU_ASSERT_EQUAL(ADC->CMP1, 0);
            }
        }
    }
}

void MACRO_ADC_SET_INPUT_CHANNEL() 
{
    uint32_t u32TestCh = 0;
    
    for (u32TestCh = 0; u32TestCh < ADC_MAX_CH; u32TestCh++)
    {
        ADC_SET_INPUT_CHANNEL(ADC, 1 << u32TestCh);
        CU_ASSERT_EQUAL((ADC->CHEN & (1 << u32TestCh)), 1 << u32TestCh);
    }
}

void MACRO_ADC_START_STOP_CONV() 
{
    uint32_t u32TestCh = 0;
    
    ADC_POWER_ON(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_ADCEN_Msk), 1);
    
    ADC_Open(ADC, NULL, NULL, 1 << u32TestCh);
    
    ADC_START_CONV(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_SWTRG_Msk), 1 << 11);

    ADC_STOP_CONV(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_SWTRG_Msk), 0 << 11);
}

void MACRO_ADC_IS_DATA_OVERRUN() 
{
    uint32_t u32TestCh = 0;
 
    ADC_POWER_ON(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_ADCEN_Msk), 1);
    
    for (u32TestCh = 0; u32TestCh < ADC_MAX_CH; u32TestCh++)
    {
        ADC_Open(ADC, NULL, ADC_OPERATION_MODE_SINGLE, 1 << u32TestCh);
        
        ADC_START_CONV(ADC);
      
        while(ADC_IS_BUSY(ADC));
    }
    
    for (u32TestCh = 0; u32TestCh < ADC_MAX_CH; u32TestCh++)
    {
        ADC_Open(ADC, NULL, ADC_OPERATION_MODE_SINGLE, 1 << u32TestCh);
        ADC_START_CONV(ADC);
        while(ADC_IS_BUSY(ADC));
        CU_ASSERT_EQUAL(ADC->DAT[u32TestCh] & ADC_DAT0_OV_Msk, (0x1ul << ADC_DAT0_OV_Pos));
    }
    
    ADC_STOP_CONV(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_SWTRG_Msk), 0 << 11);
    
    ADC_POWER_DOWN(ADC);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_ADCEN_Msk, 0);

}

void MACRO_ADC_IS_DATA_VALID() 
{
    uint32_t u32TestCh = 0;
    
    ADC_POWER_ON(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_ADCEN_Msk), 1);
    
    for (u32TestCh = 0; u32TestCh < ADC_MAX_CH; u32TestCh++)
    {
        ADC_Open(ADC, NULL, NULL, 1 << u32TestCh);
        
        ADC_START_CONV(ADC);

        while(ADC_IS_BUSY(ADC));
      
        CU_ASSERT_EQUAL(ADC->DAT[u32TestCh] & ADC_DAT0_VALID_Msk, (0x1ul << ADC_DAT0_VALID_Pos));
    }
    
    ADC_STOP_CONV(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_SWTRG_Msk), 0 << 11);
    
    ADC_POWER_DOWN(ADC);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_ADCEN_Msk, 0);
}

void MACRO_ADC_GET_CLR_INT_FLAG() 
{
    uint32_t u32TestCh = 0;
    
    ADC_POWER_ON(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_ADCEN_Msk), 1);

		ADC_ENABLE_CMP0(ADC, u32TestCh, (1<<ADC_CMP0_CMPCOND_Pos), AdcPatternTable[2], 1);
		CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPCH_Msk), u32TestCh << 3);
		CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPCOND_Msk), 1 << 2);
		CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPDAT_Msk), AdcPatternTable[2] << 16);
		CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_CMPMCNT_Msk), (1 - 1) << 8);
		CU_ASSERT_EQUAL((ADC->CMP0 & ADC_CMP0_ADCMPEN_Msk), 1);

		ADC_ENABLE_CMP1(ADC, u32TestCh, (1<<ADC_CMP1_CMPCOND_Pos) , AdcPatternTable[2], 1);
		CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPCH_Msk), u32TestCh << 3);
		CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPCOND_Msk), 1 << 2);
		CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPDAT_Msk), AdcPatternTable[2] << 16);
		CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_CMPMCNT_Msk), (1 - 1) << 8);
		CU_ASSERT_EQUAL((ADC->CMP1 & ADC_CMP1_ADCMPEN_Msk), 1);
		
		ADC_Open(ADC, NULL, NULL, 1 << u32TestCh);
		
		ADC_START_CONV(ADC);

		while((ADC->STATUS & ADC_STATUS_ADIF_Msk) == 0);
		while((ADC->STATUS & ADC_STATUS_ADCMPF0_Msk) == 0);
		while((ADC->STATUS & ADC_STATUS_ADCMPF1_Msk) == 0);

		ADC_STOP_CONV(ADC);
		CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_SWTRG_Msk), 0 << 11);

		CU_ASSERT_EQUAL(ADC_GET_INT_FLAG(ADC, ADC_ADF_INT) & ADC_STATUS_ADIF_Msk, 0x1);
		CU_ASSERT_EQUAL(ADC_GET_INT_FLAG(ADC, ADC_CMP0_INT) & ADC_STATUS_ADCMPF0_Msk, 0x2);
		CU_ASSERT_EQUAL(ADC_GET_INT_FLAG(ADC, ADC_CMP1_INT) & ADC_STATUS_ADCMPF1_Msk, 0x4);
		
		ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
		CU_ASSERT_EQUAL(ADC->STATUS & ADC_STATUS_ADIF_Msk, 0);
		
		ADC_CLR_INT_FLAG(ADC, ADC_CMP0_INT);
		CU_ASSERT_EQUAL(ADC->STATUS & ADC_STATUS_ADCMPF0_Msk, 0);

		ADC_CLR_INT_FLAG(ADC, ADC_CMP1_INT);
		CU_ASSERT_EQUAL(ADC->STATUS & ADC_STATUS_ADCMPF1_Msk, 0);

}
#endif

#ifdef MACRO2_TEST
void MACRO_ADC_GET_CONVERSION_DATA()
{
    uint32_t u32TestCh = 0;
    
    ADC_POWER_ON(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_ADCEN_Msk), 1);

    for (u32TestCh = 0; u32TestCh < ADC_MAX_CH; u32TestCh++)
    {
        ADC_Open(ADC, NULL, NULL, 1 << u32TestCh);
        ADC_START_CONV(ADC);
        while(ADC_IS_BUSY(ADC) == 0);

        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
        CU_ASSERT_EQUAL(ADC->STATUS & ADC_STATUS_ADIF_Msk, 0);

        CU_ASSERT_EQUAL(ADC_GET_CONVERSION_DATA(ADC, u32TestCh), ADC->DAT[u32TestCh] & ADC_DAT0_RESULT_Msk);
    }
    
    ADC_STOP_CONV(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_SWTRG_Msk), 0 << 11);
}

void MACRO_ADC_IS_BUSY() 
{
    uint32_t u32TestCh = 0;
    
        /* Set HCLK source form HXT and HCLK source divide 1  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_HCLK_CLK_DIVIDER(1));

    /* Enable external 12MHz HXT, 32KHz LXT and HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_HIRC0STB_Msk | CLK_STATUS_HIRC1STB_Msk | CLK_STATUS_MIRCSTB_Msk);

    CLK_SetModuleClock(ADC_MODULE,CLK_CLKSEL1_ADCSEL_HIRC,CLK_ADC_CLK_DIVIDER(5));

    /* Reset ADC */
    SYS_ResetModule(ADC_RST);

    ADC_POWER_ON(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_ADCEN_Msk), 1);
    
    for (u32TestCh = 0; u32TestCh < ADC_MAX_CH; u32TestCh++)
    {
        ADC_Open(ADC, NULL, NULL, 1 << u32TestCh);
        
        ADC_START_CONV(ADC);
       
        while(ADC_IS_BUSY(ADC) == 1);
    }
    CU_ASSERT_EQUAL(ADC->STATUS & ADC_STATUS_BUSY_Msk, 0);
    
    ADC_STOP_CONV(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_SWTRG_Msk), 0 << 11);
    
    ADC_POWER_DOWN(ADC);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_ADCEN_Msk, 0);
}

void MACRO_ADC_SET_RESOLUTION()
{
    ADC_SET_RESOLUTION(ADC, ADC_RESSEL_6_BIT);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_RESSEL_Msk), ADC_RESSEL_6_BIT);
    ADC_SET_RESOLUTION(ADC, ADC_RESSEL_8_BIT);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_RESSEL_Msk), ADC_RESSEL_8_BIT);
    ADC_SET_RESOLUTION(ADC, ADC_RESSEL_10_BIT);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_RESSEL_Msk), ADC_RESSEL_10_BIT);
    ADC_SET_RESOLUTION(ADC, ADC_RESSEL_12_BIT);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_RESSEL_Msk), ADC_RESSEL_12_BIT);
}

void MACRO_ADC_SET_REF_VOLTAGE()
{
    ADC_SET_REF_VOLTAGE(ADC, ADC_REFSEL_POWER);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_REFSEL_Msk), ADC_REFSEL_POWER);
    ADC_SET_REF_VOLTAGE(ADC, ADC_REFSEL_INT_VREF);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_REFSEL_Msk), ADC_REFSEL_INT_VREF);
    ADC_SET_REF_VOLTAGE(ADC, ADC_REFSEL_VREF);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_REFSEL_Msk), ADC_REFSEL_VREF);
}

void MACRO_ADC_SET_POWERDOWN_MODE()
{
    ADC_SET_POWERDOWN_MODE(ADC, 0, 1);
    CU_ASSERT_EQUAL((ADC->PWD & (ADC_PWD_PWDMOD_Msk | ADC_PWD_PWDCALEN_Msk)), (0 << ADC_PWD_PWDMOD_Pos) | (1 << ADC_PWD_PWDCALEN_Pos));
    ADC_SET_POWERDOWN_MODE(ADC, 2, 1);
    CU_ASSERT_EQUAL((ADC->PWD & (ADC_PWD_PWDMOD_Msk | ADC_PWD_PWDCALEN_Msk)), (2 << ADC_PWD_PWDMOD_Pos) | (1 << ADC_PWD_PWDCALEN_Pos));
    ADC_SET_POWERDOWN_MODE(ADC, 2, 0);
    CU_ASSERT_EQUAL((ADC->PWD & (ADC_PWD_PWDMOD_Msk | ADC_PWD_PWDCALEN_Msk)), (2 << ADC_PWD_PWDMOD_Pos) | (0 << ADC_PWD_PWDCALEN_Pos));
}

void MACRO_ADC_ENABLE_PDMA()
{
    ADC_ENABLE_PDMA(ADC);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_PTEN_Msk, ADC_CTL_PTEN_Msk);
    ADC_DISABLE_PDMA(ADC);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_PTEN_Msk, 0);
}
#endif

#ifdef API_TEST
void API_ADC_Close() 
{
    ADC_POWER_ON(ADC);
    CU_ASSERT_EQUAL((ADC->CTL & ADC_CTL_ADCEN_Msk), 1);

    ADC_Close(ADC);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_ADCEN_Msk, 0);
}

void API_ADC_EnableHWTrigger() 
{
    ADC_EnableHWTrigger(ADC, (ADC_TRIGGER_BY_EXT_PIN << ADC_CTL_HWTRGEN_Pos), ADC_FALLING_EDGE_TRIGGER);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGSEL_Msk, 0 << 4);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGCOND_Msk, 2 << 6);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGCOND_Msk, 2 << ADC_CTL_HWTRGCOND_Pos);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGEN_Msk, 1 << 8);
    
    ADC_EnableHWTrigger(ADC, (ADC_TRIGGER_BY_EXT_PIN << ADC_CTL_HWTRGEN_Pos), ADC_RISING_EDGE_TRIGGER);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGSEL_Msk, 0 << 4);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGCOND_Msk, 3 << 6);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGCOND_Msk, 3 << ADC_CTL_HWTRGCOND_Pos);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGEN_Msk, 1 << 8);
}

void API_ADC_DisableHWTrigger() 
{
    ADC_EnableHWTrigger(ADC, ADC_TRIGGER_BY_EXT_PIN, ADC_FALLING_EDGE_TRIGGER);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGSEL_Msk, 0 << 4);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGCOND_Msk, 2 << 6);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGEN_Msk, 1 << 8);

    ADC_DisableHWTrigger(ADC);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGSEL_Msk, 0 << 4);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGCOND_Msk, 0 << 6);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGEN_Msk, 0 << 8);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_HWTRGEN_Msk, (0UL<<ADC_CTL_HWTRGEN_Pos));
}

void API_ADC_EnableTimerTrigger() 
{
    ADC_EnableTimerTrigger(ADC, 0, 2);
    CU_ASSERT_EQUAL(ADC->CTL & (2 << ADC_CTL_TMPDMACNT_Pos), (2 << ADC_CTL_TMPDMACNT_Pos));
    CU_ASSERT_EQUAL(ADC->CTL & (0 << ADC_CTL_TMSEL_Pos), (0 << ADC_CTL_TMSEL_Pos));
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_TMTRGMOD_Msk, ADC_CTL_TMTRGMOD_Msk);
  
    ADC_EnableTimerTrigger(ADC, 3, 2);
    CU_ASSERT_EQUAL(ADC->CTL & (2 << ADC_CTL_TMPDMACNT_Pos), (2 << ADC_CTL_TMPDMACNT_Pos));
    CU_ASSERT_EQUAL(ADC->CTL & (3 << ADC_CTL_TMSEL_Pos), (3 << ADC_CTL_TMSEL_Pos));
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_TMTRGMOD_Msk, ADC_CTL_TMTRGMOD_Msk);
}

void API_ADC_DisableTimerTrigger() 
{
    ADC_EnableTimerTrigger(ADC, 0, 2);
    ADC_DisableTimerTrigger(ADC);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_TMTRGMOD_Msk, 0);
}

void API_ADC_SetExtraSampleTime()
{
    ADC_SetExtraSampleTime(ADC, 1, 5);
    CU_ASSERT_EQUAL(ADC->EXTSMPT0 & (5 << (1 * 4)), (5 << (1 * 4)));
    ADC_SetExtraSampleTime(ADC, 16, 5);
    CU_ASSERT_EQUAL(ADC->EXTSMPT1 & ADC_EXTSMPT1_EXTSMPT_INTCH_Msk, (5 << ADC_EXTSMPT1_EXTSMPT_INTCH_Pos));
}

void API_ADC_EnableInt() 
{
    ADC_EnableInt(ADC, ADC_ADF_INT);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_ADCIEN_Msk, 1 << 1);
    
    ADC_EnableInt(ADC, ADC_CMP0_INT);
    CU_ASSERT_EQUAL(ADC->CMP0 & ADC_CMP0_ADCMPIE_Msk, 1 << 1);

    ADC_EnableInt(ADC, ADC_CMP1_INT);
    CU_ASSERT_EQUAL(ADC->CMP1 & ADC_CMP1_ADCMPIE_Msk, 1 << 1);
    CU_ASSERT_EQUAL(ADC->CMP1 & ADC_CMP1_ADCMPIE_Msk, ADC_CMP1_ADCMPIE_Msk);
}

void API_ADC_DisableInt() 
{
    ADC_EnableInt(ADC, ADC_ADF_INT);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_ADCIEN_Msk, 1 << 1);
    
    ADC_EnableInt(ADC, ADC_CMP0_INT);
    CU_ASSERT_EQUAL(ADC->CMP0 & ADC_CMP0_ADCMPIE_Msk, 1 << 1);

    ADC_EnableInt(ADC, ADC_CMP1_INT);
    CU_ASSERT_EQUAL(ADC->CMP1 & ADC_CMP1_ADCMPIE_Msk, 1 << 1);
    
    ADC_EnableInt(ADC, ADC_ADF_INT);
    CU_ASSERT_EQUAL(ADC->CTL & ADC_CTL_ADCIEN_Msk, 1 << 1);
    
    ADC_DisableInt(ADC, ADC_CMP0_INT);
    CU_ASSERT_EQUAL(ADC->CMP0 & ADC_CMP1_ADCMPIE_Msk, 0 << 1);

    ADC_DisableInt(ADC, ADC_CMP1_INT);
    CU_ASSERT_EQUAL(ADC->CMP1 & ADC_CMP1_ADCMPIE_Msk, 0 << 1);
}

void Func_ADC_ADCMPR_CMPD()
{
    uint32_t u32CMPDPatternIndex = 0, u32CMPDPatternCount = 0, u32CMPDPatternData = 0;
    
    u32CMPDPatternCount = sizeof(AdcPatternTable)/sizeof(uint32_t);
    
    for (u32CMPDPatternIndex = 0; u32CMPDPatternIndex < u32CMPDPatternCount; u32CMPDPatternIndex++)
    {
        u32CMPDPatternData = AdcPatternTable[u32CMPDPatternIndex];

        ADC->CMP0 = (u32CMPDPatternData) << ADC_CMP0_CMPDAT_Pos;
        CU_ASSERT_EQUAL(ADC->CMP0, u32CMPDPatternData << 16);
    }
}

void Func_ADC_ADCMPR_CMPMATCNT()
{
    uint32_t u32TestCMPMatchCount = 0;
    
    for (u32TestCMPMatchCount = 1; u32TestCMPMatchCount <= ADC_MAX_CMP_MATCH_COUNT; u32TestCMPMatchCount++)
    {
        ADC->CMP0 = (((u32TestCMPMatchCount)-1) << ADC_CMP0_CMPMCNT_Pos);
        CU_ASSERT_EQUAL(ADC->CMP0, (u32TestCMPMatchCount - 1) << 8);
    }
}

void Func_ADC_ADCMPR_CMPCH() 
{
    uint32_t u32TestCh = 0;
    
    for (u32TestCh = 0; u32TestCh < ADC_MAX_CH; u32TestCh++)
    {
        ADC->CMP0 = ((u32TestCh) << ADC_CMP0_CMPCH_Pos);
        CU_ASSERT_EQUAL(ADC->CMP0, u32TestCh << 3);
    }
}
#endif

#ifdef MACRO_TEST
CU_TestInfo ADC_MACRO[] =
{   
    {"Test ADC_ENABLE_CMP0()/ADC_DISABLE_CMP0()", MACRO_ADC_ENABLE_DISABLE_CMP0},
    {"Test ADC_ENABLE_CMP1()/ADC_DISABLE_CMP1()", MACRO_ADC_ENABLE_DISABLE_CMP1},
    {"Test ADC_SET_INPUT_CHANNEL()", MACRO_ADC_SET_INPUT_CHANNEL},
    {"Test ADC_START_CONV()/ADC_STOP_CONV()", MACRO_ADC_START_STOP_CONV},    
    {"Test ADC_IS_DATA_OVERRUN()", MACRO_ADC_IS_DATA_OVERRUN},   
    {"Test ADC_IS_DATA_VALID():", MACRO_ADC_IS_DATA_VALID},
    {"Test ADC_GET_INT_FLAG()/ADC_CLR_INT_FLAG()", MACRO_ADC_GET_CLR_INT_FLAG},
    CU_TEST_INFO_NULL
};
#endif

#ifdef MACRO2_TEST
CU_TestInfo ADC_MACRO2[] =
{   
    {"Test ADC_SET_RESOLUTION()", MACRO_ADC_SET_RESOLUTION},
    {"Test ADC_SET_REF_VOLTAGE()", MACRO_ADC_SET_REF_VOLTAGE},
    {"Test ADC_SET_POWERDOWN_MODE()", MACRO_ADC_SET_POWERDOWN_MODE},
    {"Test ADC_ENABLE_PDMA()", MACRO_ADC_ENABLE_PDMA},
    {"Test ADC_GET_CONVERSION_DATA()", MACRO_ADC_GET_CONVERSION_DATA},
    {"Test ADC_IS_BUSY()", MACRO_ADC_IS_BUSY},
CU_TEST_INFO_NULL
};
#endif

#ifdef API_TEST
CU_TestInfo ADC_API[] =
{
    {"Test ADC_Close()", API_ADC_Close},
    {"Test ADC_EnableHWTrigger()", API_ADC_EnableHWTrigger},
    {"Test ADC_DisableHWTrigger()", API_ADC_DisableHWTrigger},
    {"Test ADC_EnableTimerTrigger()", API_ADC_EnableTimerTrigger},
    {"Test ADC_DisableTimerTrigger()", API_ADC_DisableTimerTrigger},
    {"Test ADC_SetExtraSampleTime()", API_ADC_SetExtraSampleTime},
    {"Test ADC_EnableInt()", API_ADC_EnableInt},
    {"Test ADC_DisableInt()", API_ADC_DisableInt},
    CU_TEST_INFO_NULL
};

CU_TestInfo ADC_CONSTANT[] =
{
    {"Test ADC_ADCMPR_CMPD(x)", Func_ADC_ADCMPR_CMPD},
    {"Test ADC_ADCMPR_CMPMATCNT(x)", Func_ADC_ADCMPR_CMPMATCNT},
    {"Test ADC_ADCMPR_CMPCH(x)", Func_ADC_ADCMPR_CMPCH},
    CU_TEST_INFO_NULL
};
#endif
