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
#include "wwdt_cunit.h"


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
    {"WWDT API", suite_success_init, suite_success_clean, NULL, NULL, WWDT_API},
    CU_SUITE_INFO_NULL
};

void Timer0_Init(uint32_t u32ClkSrc)
{
    SYS_UnlockReg();

    CLK_EnableModuleClock(TMR0_MODULE);

    CLK->CLKSEL1 &= ~(0xF << CLK_CLKSEL1_TMR0SEL_Pos); // CLK_CLKSEL1_TMR0_S_HXT;
    TIMER0->CMP = 0xFFFFFF;
    TIMER0->PRECNT = 12 - 1;
    TIMER0->CTL = TIMER_PERIODIC_MODE | TIMER_CTL_CNTEN_Msk;
}

void Timer0_ResetrCounter(void)
{
    TIMER0->CMP = 0xFFFFFF;
}

volatile uint8_t gu8IntFlag;
volatile uint32_t gu32TDRTicks, gu32WWDTCnt;

void WDT_IRQHandler(void)
{
    if(WWDT_GET_INT_FLAG() == 1) {
        
        gu32TDRTicks = TIMER0->CNT;
        gu8IntFlag = 1;
        
        gu32WWDTCnt = WWDT_GET_COUNTER();
        /* Clear WWDT compare match interrupt flag */
        WWDT_CLEAR_INT_FLAG();
        WWDT_RELOAD_COUNTER();
        CU_ASSERT_EQUAL(WWDT_GET_INT_FLAG(), 0x0); 
    }
}

const uint32_t gu32PeriodSel[] = {
    WWDT_PRESCALER_1,
    WWDT_PRESCALER_2,
    WWDT_PRESCALER_4,
    WWDT_PRESCALER_8,
    WWDT_PRESCALER_16,
    WWDT_PRESCALER_32,
    WWDT_PRESCALER_64,
    WWDT_PRESCALER_128,
    WWDT_PRESCALER_192,
    WWDT_PRESCALER_256,
    WWDT_PRESCALER_384,
    WWDT_PRESCALER_512,
    WWDT_PRESCALER_768,
    WWDT_PRESCALER_1024,
    WWDT_PRESCALER_1536,
    WWDT_PRESCALER_2048,
};

#define TEST_MARKER         (0x20001FFC)
void API_WWDT_Open(void) 
{
    volatile uint32_t u32Timeout = 0;
    uint32_t u32MarkerCnt;
            
    
    if(0){
        int i = 0;
        Timer0_Init(CLK_CLKSEL1_TMR0SEL_HXT);
        while(1) {
            if(TIMER0->CNT > 1000000) {
                Timer0_ResetrCounter();
                printf("%d \n", i++);
            }
        }
    }
    
    
    CLK->APBCLK |= CLK_APBCLK_WDTCKEN_Msk;
    
    if(WWDT_GET_RESET_FLAG())
    {
        printf("[WWDT reset occurred]\n");
        WWDT_CLEAR_RESET_FLAG();
        CU_ASSERT_EQUAL(WWDT_GET_RESET_FLAG(), 0x0);
    } else {
        printf("[No WWDT reset]\n");
    }        
    UART_WAIT_TX_EMPTY(UART0);
    
    u32MarkerCnt = inpw(TEST_MARKER);
    if(u32MarkerCnt > 15)
    {
        u32MarkerCnt = 0;
    } else {
        u32MarkerCnt++;
    }
    outpw(TEST_MARKER, u32MarkerCnt);
    printf("[Current Test marker is %d. Please hit [R]un to check test results.]\n", u32MarkerCnt);
    UART_WAIT_TX_EMPTY(UART0);
    
    if(u32MarkerCnt == 7)
    {
        /* Enable WDT/WWDT NVIC */
        NVIC_EnableIRQ(WDT_IRQn);
        
        Timer0_Init(CLK_CLKSEL1_TMR0SEL_HXT);
        Timer0_ResetrCounter();
        
        /* If HCLK 32 MHz, prescaler 128 ... period time is around 262144 us */
        // (2048 * 128 * (64 - 0x20)) / 32000000 = 0.262144...
        WWDT_Open(WWDT_PRESCALER_128, 0x20, TRUE);
        CU_ASSERT_EQUAL(WWDT->CTL, 0x00200701); 
        gu8IntFlag = 0;

        while(gu8IntFlag == 0) {
            if(TIMER0->CNT > __HXT/12) {            
                /* Disable WWDT module clock */
                CLK->APBCLK &= ~CLK_APBCLK_WDTCKEN_Msk;
                CU_FAIL("API_WWDT_Open INT flag fail");
                return ;
            }         
            
            u32Timeout++;
        }
        
        if((gu32WWDTCnt == 0x20) || (gu32WWDTCnt == 0x1F)) {
            ;
        } else {
            CLK->APBCLK &= ~CLK_APBCLK_WDTCKEN_Msk;
            CU_FAIL("API_WWDT_Open Counter fail");
            return ;
        }
        
        if ((gu32TDRTicks > (288358)) || (gu32TDRTicks < (235929))) { //262144 * 0.9 & 1.1 as upper and lower bound
            CLK->APBCLK &= ~CLK_APBCLK_WDTCKEN_Msk;
            printf("-->%d\n", gu32TDRTicks);
            CU_FAIL("API_WWDT_Open period time fail");
            return ;
        }
    } else {
        WWDT_Open(gu32PeriodSel[u32MarkerCnt], 0x20, FALSE);
        WWDT->RLDCNT = WWDT_RELOAD_WORD;
    }    
}

CU_TestInfo WWDT_API[] =
{
    {"Check WWDT_Open API with Interrupt Status and Reload/Get Counter MACRO ", API_WWDT_Open}, 
    CU_TEST_INFO_NULL
};
