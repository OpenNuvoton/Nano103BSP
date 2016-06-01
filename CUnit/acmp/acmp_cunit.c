/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/* Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.                                        */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   acmp_cunit.c                                                                                          */
/*            The test function of ACMP for CUnit.                                                         */
/* Project:   M0519                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------*/
/* Includes of system headers                                                                              */
/*---------------------------------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Nano103.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Includes of local headers                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
#include "CUnit.h"
#include "Console.h"
#include "acmp_cunit.h"

extern unsigned int STATE0; 
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int ACMP_Tests_Init(void) { return 0; }
int ACMP_Tests_Clean(void) { return 0; }

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
    {"ACMP MACRO", ACMP_Tests_Init, ACMP_Tests_Clean, NULL, NULL, ACMP_MacroTests},
    {"ACMP API", ACMP_Tests_Init, ACMP_Tests_Clean, NULL, NULL, ACMP_ApiTests},
    CU_SUITE_INFO_NULL
};


void MACRO_ACMP_SET_NEG_SRC()
{
    int i;
    uint32_t lvl[] = {ACMP_VNEG_4_OVER_24_VDD, 
                      ACMP_VNEG_5_OVER_24_VDD,
                      ACMP_VNEG_6_OVER_24_VDD,
                      ACMP_VNEG_7_OVER_24_VDD,
                      ACMP_VNEG_8_OVER_24_VDD,
                      ACMP_VNEG_9_OVER_24_VDD,
                      ACMP_VNEG_10_OVER_24_VDD,
                      ACMP_VNEG_11_OVER_24_VDD,
                      ACMP_VNEG_12_OVER_24_VDD,
                      ACMP_VNEG_13_OVER_24_VDD,
                      ACMP_VNEG_14_OVER_24_VDD,
                      ACMP_VNEG_15_OVER_24_VDD,
                      ACMP_VNEG_16_OVER_24_VDD,
                      ACMP_VNEG_17_OVER_24_VDD,
                      ACMP_VNEG_18_OVER_24_VDD,
                      ACMP_VNEG_19_OVER_24_VDD}; 
    
    CU_ASSERT_FALSE(ACMP->CTL0 & 0x10);
    ACMP_SET_NEG_SRC(ACMP, 0, ACMP_VNEG_PIN);
    CU_ASSERT((ACMP->CTL0 & 0x30)==0x00);
    ACMP_SET_NEG_SRC(ACMP, 0, ACMP_VNEG_IREF);
    CU_ASSERT((ACMP->CTL0 & 0x30)==0x20);
    CU_ASSERT((ACMP->VREF & 0xFF)==0x00);
                      
    for(i = 0; i < sizeof(lvl)/sizeof(uint32_t); i++) {
        ACMP_SET_NEG_SRC(ACMP, 0, lvl[i]);
        CU_ASSERT((ACMP->CTL0 & 0x30)==0x10);
        CU_ASSERT((ACMP->VREF & 0x10)==0x10);
        CU_ASSERT((ACMP->VREF & 0xF)==i);
    }
                      
    /* Reset ACMP */
    SYS->IPRST2 |= SYS_IPRST2_ACMP0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_ACMP0RST_Msk;
}

void MACRO_ACMP_ENABLE_DISABLE_HYSTERESIS()
{
    CU_ASSERT_FALSE(ACMP->CTL0 & 0x4);
    ACMP_ENABLE_HYSTERESIS(ACMP, 0);
    CU_ASSERT((ACMP->CTL0 & 0x4)==0x4);
    ACMP_DISABLE_HYSTERESIS(ACMP, 0);
    CU_ASSERT((ACMP->CTL0 & 0x4)==0x0);
    
    /* Reset ACMP */
    SYS->IPRST2 |= SYS_IPRST2_ACMP0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_ACMP0RST_Msk;
}

void MACRO_ACMP_ENABLE_DISABLE_INT()
{
    CU_ASSERT_FALSE(ACMP->CTL0 & 0x2);
    ACMP_ENABLE_INT(ACMP, 0);
    CU_ASSERT((ACMP->CTL0 & 0x2)==0x2);
    ACMP_DISABLE_INT(ACMP, 0);
    CU_ASSERT((ACMP->CTL0 & 0x2)==0x0);
    
    /* Reset ACMP */
    SYS->IPRST2 |= SYS_IPRST2_ACMP0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_ACMP0RST_Msk;
}

void MACRO_ACMP_ENABLE_DISABLE_ACMP()
{
    CU_ASSERT_FALSE(ACMP->CTL0 & 0x1);
    ACMP_ENABLE(ACMP, 0);
    CU_ASSERT((ACMP->CTL0 & 0x1)==0x1);
    ACMP_DISABLE(ACMP, 0);
    CU_ASSERT((ACMP->CTL0 & 0x1)==0x0);
    
    /* Reset ACMP */
    SYS->IPRST2 |= SYS_IPRST2_ACMP0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_ACMP0RST_Msk;
}

void MACRO_ACMP_GET_OUTPUT()
{
    ACMP_SET_NEG_SRC(ACMP, 0, ACMP_VNEG_IREF);
    /* Set PA.0 (ACMP0_P) as output mode */
    PA->MODE = 0x01;
    /* Enable ACMP */
    ACMP_ENABLE(ACMP, 0);
    /* Set PA.0 (ACMP0_P) to low level */
    PA0 = 0;
    /* 2 ms stable time */
    CLK_SysTickDelay(2000);
    CU_ASSERT(ACMP_GET_OUTPUT(ACMP, 0)==0);
    /* Set PA.0 (ACMP0_P) to high level */
    PA0 = 1;
    __NOP();
//    CU_ASSERT(ACMP_GET_OUTPUT(ACMP, 0)==1);
    
    /* Reset ACMP */
    SYS->IPRST2 |= SYS_IPRST2_ACMP0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_ACMP0RST_Msk;
}

void MACRO_ACMP_GET_CLR_INT_FLAG()
{
    ACMP_SET_NEG_SRC(ACMP, 0, ACMP_VNEG_IREF);
    /* Set PA.0 (ACMP0_P) as output mode */
    PA->MODE = 0x01;
    /* Enable ACMP */
    ACMP_ENABLE(ACMP, 0);
    /* Set PA.0 (ACMP0_P) to low level */
    PA0 = 0;
    /* 2 ms stable time */
    CLK_SysTickDelay(2000);
    /* Set PA.0 (ACMP0_P) to high level */
    PA0 = 1;
    __NOP();
//    CU_ASSERT(ACMP_GET_INT_FLAG(ACMP, 0)==1);
//    CU_ASSERT((ACMP->STATUS & 0x1)==0x1);
    ACMP_CLR_INT_FLAG(ACMP, 0);
    CU_ASSERT((ACMP->STATUS & 0x1)==0x0);
    
    /* Reset ACMP */
    SYS->IPRST2 |= SYS_IPRST2_ACMP0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_ACMP0RST_Msk;
}

void MACRO_ACMP_CRV_CTRL()
{
    int i;
    uint32_t lvl[] = {ACMP_VNEG_4_OVER_24_VDD, 
                      ACMP_VNEG_5_OVER_24_VDD,
                      ACMP_VNEG_6_OVER_24_VDD,
                      ACMP_VNEG_7_OVER_24_VDD,
                      ACMP_VNEG_8_OVER_24_VDD,
                      ACMP_VNEG_9_OVER_24_VDD,
                      ACMP_VNEG_10_OVER_24_VDD,
                      ACMP_VNEG_11_OVER_24_VDD,
                      ACMP_VNEG_12_OVER_24_VDD,
                      ACMP_VNEG_13_OVER_24_VDD,
                      ACMP_VNEG_14_OVER_24_VDD,
                      ACMP_VNEG_15_OVER_24_VDD,
                      ACMP_VNEG_16_OVER_24_VDD,
                      ACMP_VNEG_17_OVER_24_VDD,
                      ACMP_VNEG_18_OVER_24_VDD,
                      ACMP_VNEG_19_OVER_24_VDD}; 
    
    CU_ASSERT(ACMP->VREF==0);
    ACMP_ENABLE_CRV(ACMP);
    CU_ASSERT(ACMP->VREF==0x10);

    for(i = 0; i < sizeof(lvl)/sizeof(uint32_t); i++) {
        ACMP_CRV_SEL(lvl[i]);
        CU_ASSERT((ACMP->VREF & 0x30)==0x10);
        CU_ASSERT((ACMP->VREF & ACMP_VREF_CRVCTL_Msk)==i);
    }
    
    ACMP_DISABLE_CRV(ACMP);
    CU_ASSERT(ACMP->VREF==0xF);
    
    
    /* Reset ACMP */
    SYS->IPRST2 |= SYS_IPRST2_ACMP0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_ACMP0RST_Msk;
}


void API_ACMP_Open_Close()
{
    ACMP_Open(ACMP, 0, ACMP_VNEG_PIN, ACMP_HYSTERESIS_ENABLE);
    CU_ASSERT(ACMP->CTL0==0x5);
    ACMP_Open(ACMP, 0, ACMP_VNEG_IREF, ACMP_HYSTERESIS_DISABLE);
    CU_ASSERT(ACMP->CTL0==0x21);
    ACMP_Close(ACMP, 0);
    CU_ASSERT((ACMP->CTL0&ACMP_CTL0_ACMPEN_Msk)==0);
    
    /* Reset ACMP */
    SYS->IPRST2 |= SYS_IPRST2_ACMP0RST_Msk;
    SYS->IPRST2 &= ~SYS_IPRST2_ACMP0RST_Msk;
}



CU_TestInfo ACMP_MacroTests[] =
{

    {"Set ACMP negative input source", MACRO_ACMP_SET_NEG_SRC},
    {"Enable/Disable ACMP hysteresis function", MACRO_ACMP_ENABLE_DISABLE_HYSTERESIS},
    {"Enable/Disable ACMP interrupt function", MACRO_ACMP_ENABLE_DISABLE_INT},
    {"Enable/Disable ACMP", MACRO_ACMP_ENABLE_DISABLE_ACMP},
    {"Get ACMP output value", MACRO_ACMP_GET_OUTPUT},
    {"Get/Clear ACMP interrupt flag", MACRO_ACMP_GET_CLR_INT_FLAG},
    {"CRV control test", MACRO_ACMP_CRV_CTRL},
    CU_TEST_INFO_NULL
};

CU_TestInfo ACMP_ApiTests[] =
{

    {"ACMP open and close test", API_ACMP_Open_Close},
 
    CU_TEST_INFO_NULL
};



