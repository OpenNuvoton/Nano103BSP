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
#include "clk_cunit.h"


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
    {"Test IP: CLK", suite_success_init, suite_success_clean,NULL, NULL, CLK_TestCases},
    CU_SUITE_INFO_NULL
};


/*---------------------------------------------------------------------------------------------------------*/
/* Global Interface Variables Declarations                                                                 */
/*---------------------------------------------------------------------------------------------------------*/



void WDT_IRQHandler(void)
{
    /* Clear WDT time-out interrupt and wake-up flag */
    WDT_CLEAR_TIMEOUT_INT_FLAG();
    WDT_CLEAR_TIMEOUT_WAKEUP_FLAG();
    printf(" WDT");
}

void TestFunc_CLK_EnableCKO() 
{
    uint8_t TestParamA;    //A CLKO cloclk source
    uint8_t TestParamB;    //B CLKO frequency divider
    uint8_t TestParamC=0;    //B CLKO frequency divider
 
    /* Test loop */    
    for( TestParamA=0; TestParamA<8; TestParamA++ )
    {
        //if( TestParamA==1 ) continue;   //skip reserved case
        
        for( TestParamB=0; TestParamB<16; TestParamB++ )
        {
            TestParamC=!TestParamC;
            /* Enable CLKO, clock source = TestParamA, FSEL = TestParamB */
            CLK_EnableCKO( (TestParamA<<CLK_CLKSEL2_CLKOSEL_Pos), TestParamB, TestParamC);
            //CLK_EnableCKO( (TestParamA<<CLK_CLKSEL2_FRQDIV0_S_Pos), TestParamB);
            CU_ASSERT( (CLK->APBCLK & CLK_APBCLK_CLKOCKEN_Msk) == CLK_APBCLK_CLKOCKEN_Msk );   
            CU_ASSERT( (CLK->CLKSEL2 & CLK_CLKSEL2_CLKOSEL_Msk ) == (TestParamA<<CLK_CLKSEL2_CLKOSEL_Pos) );    
            CU_ASSERT( (CLK->CLKOCTL & CLK_CLKOCTL_DIV1EN_Msk) == TestParamC<<CLK_CLKOCTL_DIV1EN_Pos );
            CU_ASSERT( (CLK->CLKOCTL & CLK_CLKOCTL_FREQSEL_Msk ) == TestParamB ); 

            /* Disable CKO clock */
            CLK_DisableCKO();    
            CU_ASSERT( (CLK->APBCLK & CLK_APBCLK_CLKOCKEN_Msk )== 0 );
        }
    }   
}


void TestFunc_CLK_GetHXTFreq() 
{
    //disable HXT test
    SYS_UnlockReg();
    CLK->PWRCTL |= CLK_PWRCTL_HIRC0EN_Msk;
    while( (CLK->STATUS & CLK_STATUS_HIRC0STB_Msk) != CLK_STATUS_HIRC0STB_Msk );
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HIRC;
    CLK->PWRCTL &= ~CLK_PWRCTL_HXTEN_Msk;
    CU_ASSERT( CLK_GetHXTFreq()==0 );   
    
    //ensable HXT test
    CLK->PWRCTL = (CLK->PWRCTL) | CLK_PWRCTL_HXTEN_Msk;
    while( (CLK->STATUS & CLK_STATUS_HXTSTB_Msk) != CLK_STATUS_HXTSTB_Msk );
    CU_ASSERT( CLK_GetHXTFreq()==12000000 );
    SYS_LockReg();

    //disable HXT test
    SYS_UnlockReg();
    CLK->PWRCTL |= CLK_PWRCTL_HIRC0EN_Msk;
    while( (CLK->STATUS & CLK_STATUS_HIRC0STB_Msk) != CLK_STATUS_HIRC0STB_Msk );
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HIRC;
    CLK->PWRCTL &= ~CLK_PWRCTL_LXTEN_Msk;
    CU_ASSERT( CLK_GetLXTFreq()==0 );   
    
    //ensable LXT test
    CLK->PWRCTL = (CLK->PWRCTL) | CLK_PWRCTL_LXTEN_Msk;
    while( (CLK->STATUS & CLK_STATUS_LXTSTB_Msk) != CLK_STATUS_LXTSTB_Msk );
    CU_ASSERT( CLK_GetLXTFreq()==__LXT );
    SYS_LockReg();
}

const uint32_t au32HclkClkSrcSel[] = {
    CLK_CLKSEL0_HCLKSEL_HXT, __HXT,
    CLK_CLKSEL0_HCLKSEL_PLL, 32000000,
    CLK_CLKSEL0_HCLKSEL_LIRC, __LIRC,
    CLK_CLKSEL0_HCLKSEL_HIRC, __HIRC,
    CLK_CLKSEL0_HCLKSEL_LXT,__LXT,
	  CLK_CLKSEL0_HCLKSEL_MIRC,__MIRC,
};

void TestFunc_CLK_GetHCLKFreq() 
{
    uint8_t u8HclkClkSrcSelIdx;    //HCLK cloclk source
    uint8_t u8TestClkDiv;    //clock divider
    SYS_UnlockReg();
    /* Enable all clock */
    CLK->PWRCTL = CLK->PWRCTL | ( CLK_PWRCTL_LIRCEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HXTEN_Msk  | CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk );
	
    //CLK->PLLCTL = 0xC22E;
    CLK->PLLCTL = CLK_PLLCTL_32MHz_HXT;
    
    /* Waiting for clock ready */    
    while( CLK->STATUS & (CLK_STATUS_HXTSTB_Msk|CLK_STATUS_LIRCSTB_Msk||CLK_STATUS_LXTSTB_Msk|
                              CLK_STATUS_HIRC0STB_Msk|CLK_STATUS_HIRC1STB_Msk||CLK_STATUS_MIRCSTB_Msk|CLK_STATUS_PLLSTB_Msk)
                          != (CLK_STATUS_HXTSTB_Msk|CLK_STATUS_LIRCSTB_Msk||CLK_STATUS_LXTSTB_Msk|
                              CLK_STATUS_HIRC0STB_Msk|CLK_STATUS_HIRC1STB_Msk|CLK_STATUS_MIRCSTB_Msk|CLK_STATUS_PLLSTB_Msk));
   
    /* Test loop */
    printf("\n");
    for( u8HclkClkSrcSelIdx=0; u8HclkClkSrcSelIdx<sizeof(au32HclkClkSrcSel)/sizeof(uint32_t); u8HclkClkSrcSelIdx+=2 )
    {
        if(au32HclkClkSrcSel[u8HclkClkSrcSelIdx]!=CLK_CLKSEL0_HCLKSEL_PLL)
          u8TestClkDiv=1;
        else
          u8TestClkDiv=1;
        for(; u8TestClkDiv<=16; u8TestClkDiv++ )
        {
            //test 10K without clock source divider
            if( au32HclkClkSrcSel[u8HclkClkSrcSelIdx]==CLK_CLKSEL0_HCLKSEL_LIRC && u8TestClkDiv>0 ) continue;

            CLK_SetHCLK( au32HclkClkSrcSel[u8HclkClkSrcSelIdx], CLK_HCLK_CLK_DIVIDER(u8TestClkDiv));
            CU_ASSERT( (CLK->CLKSEL0 & CLK_CLKSEL0_HCLKSEL_Msk ) == au32HclkClkSrcSel[u8HclkClkSrcSelIdx] );
            CU_ASSERT( (CLK->CLKDIV0 & CLK_CLKDIV0_HCLKDIV_Msk) == (u8TestClkDiv-1) );
            CU_ASSERT( CLK_GetHCLKFreq() == (au32HclkClkSrcSel[u8HclkClkSrcSelIdx+1])/(u8TestClkDiv) );
            CU_ASSERT( CLK_GetCPUFreq() == (au32HclkClkSrcSel[u8HclkClkSrcSelIdx+1])/(u8TestClkDiv) ); 
            //CU_ASSERT( CLK_GetPCLKFreq() == (au32HclkClkSrcSel[u8HclkClkSrcSelIdx+1])/(u8TestClkDiv) );
            printf(".");
        }
        UART_WAIT_TX_EMPTY(UART0);
    } 
    printf("\n");
    //restore HCLK setting
    CLK->CLKSEL0 &= ~CLK_CLKSEL0_HCLKSEL_Msk; 
    CLK->CLKDIV0 &= ~CLK_CLKDIV0_HCLKDIV_Msk; 
    SYS_LockReg();
}

void TestFunc_CLK_SetCoreClock() 
{
    uint32_t TestParamA;    //A PLL frequency     
    SYS_UnlockReg();
    /* Test loop */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART0SEL_Msk) | CLK_CLKSEL1_UART0SEL_HIRC;  //UART0 select clock source to HIRC
    for( TestParamA=25000000; TestParamA<=32000000; TestParamA+=1000000 )
    {
        //test core clock from HIRC
        CLK->PWRCTL |= CLK_PWRCTL_HIRC0EN_Msk;
        while( (CLK->STATUS & CLK_STATUS_HIRC0STB_Msk) != CLK_STATUS_HIRC0STB_Msk );
        CLK->CLKSEL0 = CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HIRC;  
        CLK_EnablePLL(CLK_PLLCTL_PLL_SRC_HIRC, 32000000);   //Add for PLL from HIRC test   
                                                            //Switch to HIRC before test 
        CLK->PWRCTL &= ~CLK_PWRCTL_HXTEN_Msk;
        CLK_SetCoreClock(TestParamA);
        //printf("%d = %d\n",TestParamA,CLK_GetPLLClockFreq());
        //printf("%d = %d\n",TestParamA,CLK_GetHCLKFreq());
        UART_WAIT_TX_EMPTY(UART0);
        CU_ASSERT( (CLK_GetPLLClockFreq() >= ((TestParamA-1000000)*1)) && 
                    (CLK_GetPLLClockFreq() <= ((TestParamA+1000000)*1)) );  //PLL is 2*frequency
        CU_ASSERT( (CLK_GetHCLKFreq() >= (TestParamA-1000000)) && 
                   (CLK_GetHCLKFreq() <= (TestParamA+1000000)) );  //HCLK is frequency
    }     
  
    /* Test loop */
    CLK->PWRCTL = (CLK->PWRCTL) | CLK_PWRCTL_HXTEN_Msk;
    while( (CLK->STATUS & CLK_STATUS_HXTSTB_Msk) != CLK_STATUS_HXTSTB_Msk );
    for( TestParamA=25000000; TestParamA<=32000000; TestParamA+=1000000 )
    {
        CLK_SetCoreClock(TestParamA);
        CU_ASSERT( (CLK_GetPLLClockFreq() >= ((TestParamA-1000000)*1)) && 
                    (CLK_GetPLLClockFreq() <= ((TestParamA+1000000)*1)) );  //PLL is 2*frequency
        CU_ASSERT( (CLK_GetHCLKFreq() >= ((TestParamA-1000000))) && 
                    (CLK_GetHCLKFreq() <= ((TestParamA+1000000))) );  //HCLK is frequency
        //printf("%d = %d,",TestParamA,CLK_GetPLLClockFreq());
        //printf("%d = %d\n",TestParamA,CLK_GetHCLKFreq());
        UART_WAIT_TX_EMPTY(UART0);
    } 
   
    //restore HCLK setting          
    CLK->CLKSEL0 &= ~CLK_CLKSEL0_HCLKSEL_Msk; 
    CLK->CLKDIV0 &= ~CLK_CLKDIV0_HCLKDIV_Msk; 
    SYS_UnlockReg();
}

const uint32_t au32ModuleClkSrcSel[] = { 
UART1_MODULE     ,(CLK_BASE+0x018) ,CLK_CLKSEL2_UART1SEL_Msk     ,CLK_CLKSEL2_UART1SEL_HXT       ,
UART1_MODULE     ,(CLK_BASE+0x018) ,CLK_CLKSEL2_UART1SEL_Msk     ,CLK_CLKSEL2_UART1SEL_LXT       ,
UART1_MODULE     ,(CLK_BASE+0x018) ,CLK_CLKSEL2_UART1SEL_Msk     ,CLK_CLKSEL2_UART1SEL_PLL       ,
UART1_MODULE     ,(CLK_BASE+0x018) ,CLK_CLKSEL2_UART1SEL_Msk     ,CLK_CLKSEL2_UART1SEL_HIRC      ,
UART1_MODULE     ,(CLK_BASE+0x018) ,CLK_CLKSEL2_UART1SEL_Msk     ,CLK_CLKSEL2_UART1SEL_MIRC      ,
	    
CLKO_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_CLKOSEL_Msk      ,CLK_CLKSEL2_CLKOSEL_HXT        ,
CLKO_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_CLKOSEL_Msk      ,CLK_CLKSEL2_CLKOSEL_LXT        ,
CLKO_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_CLKOSEL_Msk      ,CLK_CLKSEL2_CLKOSEL_HCLK       ,
CLKO_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_CLKOSEL_Msk      ,CLK_CLKSEL2_CLKOSEL_HIRC       ,
CLKO_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_CLKOSEL_Msk      ,CLK_CLKSEL2_CLKOSEL_MIRC       ,
	
TMR3_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR3SEL_Msk      , CLK_CLKSEL2_TMR3SEL_HXT       ,
TMR3_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR3SEL_Msk      , CLK_CLKSEL2_TMR3SEL_LXT       ,
TMR3_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR3SEL_Msk      , CLK_CLKSEL2_TMR3SEL_LIRC      ,
TMR3_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR3SEL_Msk      , CLK_CLKSEL2_TMR3SEL_HIRC      ,
TMR3_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR3SEL_Msk      , CLK_CLKSEL2_TMR3SEL_MIRC      ,
TMR3_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR3SEL_Msk      , CLK_CLKSEL2_TMR3SEL_EXT       ,
	  
TMR2_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR2SEL_Msk      , CLK_CLKSEL2_TMR2SEL_HXT       ,
TMR2_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR2SEL_Msk      , CLK_CLKSEL2_TMR2SEL_LXT       ,
TMR2_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR2SEL_Msk      , CLK_CLKSEL2_TMR2SEL_LIRC      ,
TMR2_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR2SEL_Msk      , CLK_CLKSEL2_TMR2SEL_HIRC      ,
TMR2_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR2SEL_Msk      , CLK_CLKSEL2_TMR2SEL_MIRC      ,
TMR2_MODULE      ,(CLK_BASE+0x018) ,CLK_CLKSEL2_TMR2SEL_Msk      , CLK_CLKSEL2_TMR2SEL_EXT       ,	
	  
SC1_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SC1SEL_Msk        ,CLK_CLKSEL2_SC1SEL_HXT        ,
SC1_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SC1SEL_Msk        ,CLK_CLKSEL2_SC1SEL_PLL        ,
SC1_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SC1SEL_Msk        ,CLK_CLKSEL2_SC1SEL_HIRC       ,
SC1_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SC1SEL_Msk        ,CLK_CLKSEL2_SC1SEL_HCLK       ,
SC1_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SC1SEL_Msk        ,CLK_CLKSEL2_SC1SEL_MIRC       ,
  
SC0_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SC0SEL_Msk        ,CLK_CLKSEL2_SC0SEL_HXT        ,
SC0_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SC0SEL_Msk        ,CLK_CLKSEL2_SC0SEL_PLL        ,
SC0_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SC0SEL_Msk        ,CLK_CLKSEL2_SC0SEL_HIRC       ,
SC0_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SC0SEL_Msk        ,CLK_CLKSEL2_SC0SEL_HCLK       ,
SC0_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SC0SEL_Msk        ,CLK_CLKSEL2_SC0SEL_MIRC       ,
	  
SPI1_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SPI1SEL_Msk      ,CLK_CLKSEL2_SPI1SEL_HXT       ,
SPI1_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SPI1SEL_Msk      ,CLK_CLKSEL2_SPI1SEL_PLL       ,
SPI1_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SPI1SEL_Msk      ,CLK_CLKSEL2_SPI1SEL_HIRC      ,
SPI1_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SPI1SEL_Msk      ,CLK_CLKSEL2_SPI1SEL_HCLK      ,

SPI3_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SPI3SEL_Msk      ,CLK_CLKSEL2_SPI3SEL_HXT       ,
SPI3_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SPI3SEL_Msk      ,CLK_CLKSEL2_SPI3SEL_PLL       ,
SPI3_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SPI3SEL_Msk      ,CLK_CLKSEL2_SPI3SEL_HIRC      ,
SPI3_MODULE       ,(CLK_BASE+0x018) ,CLK_CLKSEL2_SPI3SEL_Msk      ,CLK_CLKSEL2_SPI3SEL_HCLK      ,

	
//UART0_MODULE     ,(CLK_BASE+0x014) ,CLK_CLKSEL1_UART0SEL_Msk      ,CLK_CLKSEL1_UART0SEL_HXT     ,
//UART0_MODULE     ,(CLK_BASE+0x014) ,CLK_CLKSEL1_UART0SEL_Msk      ,CLK_CLKSEL1_UART0SEL_LXT     ,
//UART0_MODULE     ,(CLK_BASE+0x014) ,CLK_CLKSEL1_UART0SEL_Msk      ,CLK_CLKSEL1_UART0SEL_PLL     ,
//UART0_MODULE     ,(CLK_BASE+0x014) ,CLK_CLKSEL1_UART0SEL_Msk      ,CLK_CLKSEL1_UART0SEL_HIRC    ,
//UART0_MODULE     ,(CLK_BASE+0x014) ,CLK_CLKSEL1_UART0SEL_Msk      ,CLK_CLKSEL1_UART0SEL_MIRC    ,

PWM0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_PWM0SEL_Msk       ,CLK_CLKSEL1_PWM0SEL_PLL   ,
PWM0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_PWM0SEL_Msk       ,CLK_CLKSEL1_PWM0SEL_PCLK0 ,

TMR0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR0SEL_Msk      ,CLK_CLKSEL1_TMR0SEL_HXT       ,
TMR0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR0SEL_Msk      ,CLK_CLKSEL1_TMR0SEL_LXT       ,
TMR0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR0SEL_Msk      ,CLK_CLKSEL1_TMR0SEL_LIRC      ,
TMR0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR0SEL_Msk      ,CLK_CLKSEL1_TMR0SEL_HIRC      ,
TMR0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR0SEL_Msk      ,CLK_CLKSEL1_TMR0SEL_MIRC      ,
TMR0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR0SEL_Msk      ,CLK_CLKSEL1_TMR0SEL_EXT       ,
	
TMR1_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR1SEL_Msk      ,CLK_CLKSEL1_TMR1SEL_HXT       ,
TMR1_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR1SEL_Msk      ,CLK_CLKSEL1_TMR1SEL_LXT       ,
TMR1_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR1SEL_Msk      ,CLK_CLKSEL1_TMR1SEL_LIRC      ,
TMR1_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR1SEL_Msk      ,CLK_CLKSEL1_TMR1SEL_HIRC      ,
TMR1_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR1SEL_Msk      ,CLK_CLKSEL1_TMR1SEL_MIRC      ,
TMR1_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_TMR1SEL_Msk      ,CLK_CLKSEL1_TMR1SEL_EXT       ,

ADC_MODULE       ,(CLK_BASE+0x014) ,CLK_CLKSEL1_ADCSEL_Msk       ,CLK_CLKSEL1_ADCSEL_HXT        ,
ADC_MODULE       ,(CLK_BASE+0x014) ,CLK_CLKSEL1_ADCSEL_Msk       ,CLK_CLKSEL1_ADCSEL_LXT        ,
ADC_MODULE       ,(CLK_BASE+0x014) ,CLK_CLKSEL1_ADCSEL_Msk       ,CLK_CLKSEL1_ADCSEL_PLL        ,
ADC_MODULE       ,(CLK_BASE+0x014) ,CLK_CLKSEL1_ADCSEL_Msk       ,CLK_CLKSEL1_ADCSEL_HIRC       ,
ADC_MODULE       ,(CLK_BASE+0x014) ,CLK_CLKSEL1_ADCSEL_Msk       ,CLK_CLKSEL1_ADCSEL_MIRC       ,
ADC_MODULE       ,(CLK_BASE+0x014) ,CLK_CLKSEL1_ADCSEL_Msk       ,CLK_CLKSEL1_ADCSEL_HCLK       ,
																																 
SPI0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_SPI0SEL_Msk      ,CLK_CLKSEL1_SPI0SEL_HXT       ,
SPI0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_SPI0SEL_Msk      ,CLK_CLKSEL1_SPI0SEL_PLL       ,
SPI0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_SPI0SEL_Msk      ,CLK_CLKSEL1_SPI0SEL_HIRC      ,
SPI0_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_SPI0SEL_Msk      ,CLK_CLKSEL1_SPI0SEL_HCLK      ,

SPI2_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_SPI2SEL_Msk      ,CLK_CLKSEL1_SPI2SEL_HXT       ,
SPI2_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_SPI2SEL_Msk      ,CLK_CLKSEL1_SPI2SEL_PLL       ,
SPI2_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_SPI2SEL_Msk      ,CLK_CLKSEL1_SPI2SEL_HIRC      ,
SPI2_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_SPI2SEL_Msk      ,CLK_CLKSEL1_SPI2SEL_HCLK      ,

WDT_MODULE       ,(CLK_BASE+0x014) ,CLK_CLKSEL1_WDTSEL_Msk       ,CLK_CLKSEL1_WDTSEL_LXT        ,
WDT_MODULE       ,(CLK_BASE+0x014) ,CLK_CLKSEL1_WDTSEL_Msk       ,CLK_CLKSEL1_WDTSEL_LIRC       ,
WDT_MODULE       ,(CLK_BASE+0x014) ,CLK_CLKSEL1_WDTSEL_Msk       ,CLK_CLKSEL1_WDTSEL_HCLKDIV2048,
WWDT_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_WWDTSEL_Msk      ,CLK_CLKSEL1_WWDTSEL_LIRC      ,
WWDT_MODULE      ,(CLK_BASE+0x014) ,CLK_CLKSEL1_WWDTSEL_Msk      ,CLK_CLKSEL1_WWDTSEL_HCLKDIV2048,
};

const uint32_t au32UartClkSrcSel[] = {
CLK_CLKSEL1_UART0SEL_HXT     ,
CLK_CLKSEL1_UART0SEL_LXT     ,	
CLK_CLKSEL1_UART0SEL_PLL     ,	
CLK_CLKSEL1_UART0SEL_HIRC    ,	
CLK_CLKSEL1_UART0SEL_MIRC    ,		
};

void TestFunc_CLK_SetModuleClock() 
{
    uint16_t u16ModuleClkSrcSelIdx;    //clock source    
    uint16_t u16TestClkDiv;    //clock divider
	
		SYS_UnlockReg();
    //wait UART print message finish before test
    UART_WAIT_TX_EMPTY(UART0);       
    
    //test module without divider and special case
    for( u16ModuleClkSrcSelIdx=0; u16ModuleClkSrcSelIdx<sizeof(au32ModuleClkSrcSel)/sizeof(uint32_t); u16ModuleClkSrcSelIdx+=4 )  		
    {        
        CLK_SetModuleClock( au32ModuleClkSrcSel[u16ModuleClkSrcSelIdx], 
                            au32ModuleClkSrcSel[u16ModuleClkSrcSelIdx+3], 
                            0);
        CU_ASSERT( (inp32(au32ModuleClkSrcSel[u16ModuleClkSrcSelIdx+1]) 
                    & au32ModuleClkSrcSel[u16ModuleClkSrcSelIdx+2]) 
                    == au32ModuleClkSrcSel[u16ModuleClkSrcSelIdx+3] );
//        printf("i=%d, val=%d\n",u16ModuleClkSrcSelIdx,((inp32(au32ModuleClkSrcSel[u16ModuleClkSrcSelIdx+1]) 
//                    & au32ModuleClkSrcSel[u16ModuleClkSrcSelIdx+2]) 
//                   == au32ModuleClkSrcSel[u16ModuleClkSrcSelIdx+3]));
    }

		//restore UART setting for printf
		CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_UART0_CLK_DIVIDER(1));
		UART_Open(UART0, 115200);

		
		//UART0/1_MODULE, UART clock source should be HIRC for printf    
		for( u16ModuleClkSrcSelIdx=0; u16ModuleClkSrcSelIdx<4; u16ModuleClkSrcSelIdx++ )  
		{
				//UART0
				for( u16TestClkDiv=1; u16TestClkDiv<=16; u16TestClkDiv++ )
				{               
						CLK_SetModuleClock( UART0_MODULE, au32UartClkSrcSel[u16ModuleClkSrcSelIdx], CLK_UART0_CLK_DIVIDER(u16TestClkDiv));
						CU_ASSERT( (CLK->CLKSEL1 & CLK_CLKSEL1_UART0SEL_Msk) == au32UartClkSrcSel[u16ModuleClkSrcSelIdx] );
						CU_ASSERT( (CLK->CLKDIV0 & CLK_CLKDIV0_UART0DIV_Msk) == (u16TestClkDiv-1)<<CLK_CLKDIV0_UART0DIV_Pos );
				}        
				
				//UART1
				for( u16TestClkDiv=1; u16TestClkDiv<=16; u16TestClkDiv++ )
				{               
						CLK_SetModuleClock( UART1_MODULE, au32UartClkSrcSel[u16ModuleClkSrcSelIdx], CLK_UART1_CLK_DIVIDER(u16TestClkDiv));
						CU_ASSERT( (CLK->CLKSEL2 & CLK_CLKSEL2_UART1SEL_Msk) == au32UartClkSrcSel[u16ModuleClkSrcSelIdx] );
						CU_ASSERT( (CLK->CLKDIV0 & CLK_CLKDIV0_UART1DIV_Msk) == (u16TestClkDiv-1)<<CLK_CLKDIV0_UART1DIV_Pos );
				}
		}
	
		//restore UART setting for printf
		CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_UART0_CLK_DIVIDER(1));
		UART_Open(UART0, 115200);
     
    //ADC_MODULE
    for( u16TestClkDiv=1; u16TestClkDiv<=256; u16TestClkDiv++ )
    {
        CLK_SetModuleClock( ADC_MODULE, 0, CLK_ADC_CLK_DIVIDER(u16TestClkDiv));
        CU_ASSERT( (CLK->CLKDIV0 & CLK_CLKDIV0_ADCDIV_Msk) == (u16TestClkDiv-1)<<CLK_CLKDIV0_ADCDIV_Pos );
    }

    //SC0_MODULE/SC1_MODULE
    for( u16TestClkDiv=1; u16TestClkDiv<16; u16TestClkDiv++ )
    {
        CLK_SetModuleClock( SC0_MODULE, 0, CLK_SC0_CLK_DIVIDER(u16TestClkDiv));
        CU_ASSERT( (CLK->CLKDIV0 & CLK_CLKDIV0_SC0DIV_Msk) == (u16TestClkDiv-1)<<CLK_CLKDIV0_SC0DIV_Pos );
    }
    for( u16TestClkDiv=1; u16TestClkDiv<16; u16TestClkDiv++ )
    {
        CLK_SetModuleClock( SC1_MODULE, 0, CLK_SC1_CLK_DIVIDER(u16TestClkDiv));
        CU_ASSERT( (CLK->CLKDIV1 & CLK_CLKDIV1_SC1DIV_Msk) == (u16TestClkDiv-1)<<CLK_CLKDIV1_SC1DIV_Pos );
    }

    //TMR0_MODULE/TMR1_MODULE/TMR2_MODULE/TMR3_MODULE
    for( u16TestClkDiv=1; u16TestClkDiv<16; u16TestClkDiv++ )
    {
        CLK_SetModuleClock( TMR0_MODULE, 0, CLK_TMR0_CLK_DIVIDER(u16TestClkDiv));
        CU_ASSERT( (CLK->CLKDIV1 & CLK_CLKDIV1_TMR0DIV_Msk) == (u16TestClkDiv-1)<<CLK_CLKDIV1_TMR0DIV_Pos );
    }
    for( u16TestClkDiv=1; u16TestClkDiv<16; u16TestClkDiv++ )
    {
        CLK_SetModuleClock( TMR1_MODULE, 0, CLK_TMR1_CLK_DIVIDER(u16TestClkDiv));
        CU_ASSERT( (CLK->CLKDIV1 & CLK_CLKDIV1_TMR1DIV_Msk) == (u16TestClkDiv-1)<<CLK_CLKDIV1_TMR1DIV_Pos );
    }
    for( u16TestClkDiv=1; u16TestClkDiv<16; u16TestClkDiv++ )
    {
        CLK_SetModuleClock( TMR2_MODULE, 0, CLK_TMR2_CLK_DIVIDER(u16TestClkDiv));
        CU_ASSERT( (CLK->CLKDIV1 & CLK_CLKDIV1_TMR2DIV_Msk) == (u16TestClkDiv-1)<<CLK_CLKDIV1_TMR2DIV_Pos );
    }
    for( u16TestClkDiv=1; u16TestClkDiv<16; u16TestClkDiv++ )
    {
        CLK_SetModuleClock( TMR3_MODULE, 0, CLK_TMR3_CLK_DIVIDER(u16TestClkDiv));
        CU_ASSERT( (CLK->CLKDIV1 & CLK_CLKDIV1_TMR3DIV_Msk) == (u16TestClkDiv-1)<<CLK_CLKDIV1_TMR3DIV_Pos );
    }
		
		//restore HCLK setting          
    CLK->CLKSEL0 &= ~CLK_CLKSEL0_HCLKSEL_Msk; 
    CLK->CLKDIV0 &= ~CLK_CLKDIV0_HCLKDIV_Msk; 
		 /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);

}

const uint32_t au32SysTickClkSrcSel[] = { 
    CLK_CLKSEL0_STCLKSEL_HCLK,  
    CLK_CLKSEL0_STCLKSEL_HCLK_DIV8,
};

void SysTick_Handler(void)
{
}
void TestFunc_CLK_SetSysTickClock() 
{
    uint8_t  u8SysTickClkSrcSelIdx;    //systick clock source   

    CLK_SysTickDelay(10);
    CU_ASSERT(SysTick->CTRL == 0);

    SYS_UnlockReg();
    /* Test loop */
//     for( u8SysTickClkSrcSelIdx=0; u8SysTickClkSrcSelIdx<sizeof(au32SysTickClkSrcSel)/sizeof(uint32_t); u8SysTickClkSrcSelIdx++ )
//     {
//         if(au32SysTickClkSrcSel[u8SysTickClkSrcSelIdx]==CLK_CLKSEL0_STCLKSEL_HCLK) continue;
//         CLK_SetSysTickClockSrc( au32SysTickClkSrcSel[u8SysTickClkSrcSelIdx] );
//         CU_ASSERT( (CLK->CLKSEL0 & CLK_CLKSEL0_STCLKSEL_Msk) == au32SysTickClkSrcSel[u8SysTickClkSrcSelIdx] );
//     } 

    for( u8SysTickClkSrcSelIdx=0; u8SysTickClkSrcSelIdx<sizeof(au32SysTickClkSrcSel)/sizeof(uint32_t); u8SysTickClkSrcSelIdx++ )
    {
        CLK_EnableSysTick(au32SysTickClkSrcSel[u8SysTickClkSrcSelIdx],0x55AA55);
        CU_ASSERT( (SysTick->LOAD & SysTick_LOAD_RELOAD_Msk) == 0x55AA55);

        CLK_EnableSysTick(au32SysTickClkSrcSel[u8SysTickClkSrcSelIdx],0xAA55AA);
        CU_ASSERT( (SysTick->LOAD & SysTick_LOAD_RELOAD_Msk) == 0xAA55AA);
        
        CU_ASSERT( (SysTick->CTRL & SysTick_CTRL_ENABLE_Msk) == SysTick_CTRL_ENABLE_Msk );
        if(au32SysTickClkSrcSel[u8SysTickClkSrcSelIdx]!=CLK_CLKSEL0_STCLKSEL_HCLK)
        {
           //CU_ASSERT( (CLK->CLKSEL0 & CLK_CLKSEL0_STCLKSEL_Msk) == au32SysTickClkSrcSel[u8SysTickClkSrcSelIdx] );
           CU_ASSERT( (SysTick->CTRL & SysTick_CTRL_CLKSOURCE_Msk) == 0 );
        }else{
           CU_ASSERT( (SysTick->CTRL & SysTick_CTRL_CLKSOURCE_Msk) == SysTick_CTRL_CLKSOURCE_Msk );
        }
        CLK_DisableSysTick();
        CU_ASSERT(SysTick->CTRL == 0);
    }


    SYS_LockReg();
}

void TestFunc_CLK_EnableXtalRC() 
{
     SYS_UnlockReg();
    //wait UART send message finish before change clock
    UART_WAIT_TX_EMPTY(UART0);
    
    CU_ASSERT(CLK_CLKSTATUS_CLK_SW_FAIL==(1<<7));

    //set HCLK to HXT and test: HIRC0(HIRC1), MIRC
    CLK->PWRCTL = (CLK->PWRCTL) | CLK_STATUS_HXTSTB_Msk;
    while( (CLK->STATUS & CLK_STATUS_HXTSTB_Msk) != CLK_STATUS_HXTSTB_Msk );
    CLK->CLKSEL0 = CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HXT;   

    //test HIRC0
    CLK_EnableXtalRC(CLK_PWRCTL_HIRC0EN_Msk);
    CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_HIRC0EN_Msk) == CLK_PWRCTL_HIRC0EN_Msk );   
//     CLK_DisableXtalRC(CLK_PWRCTL_HIRC0EN_Msk);   
//     CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_HIRC0EN_Msk) == 0 );    

//     //test HIRC1
//     CLK_EnableXtalRC(CLK_PWRCTL_HIRC1EN_Msk);
//     CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_HIRC1EN_Msk) == CLK_PWRCTL_HIRC1EN_Msk );   
//     CLK_DisableXtalRC(CLK_PWRCTL_HIRC1EN_Msk);   
//     CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_HIRC1EN_Msk) == 0 );    

    //test MIRC
    CLK_EnableXtalRC(CLK_PWRCTL_MIRCEN_Msk);
    CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_MIRCEN_Msk) == CLK_PWRCTL_MIRCEN_Msk );   
    CLK_DisableXtalRC(CLK_PWRCTL_MIRCEN_Msk);   
    CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_MIRCEN_Msk) == 0 );    

    //set HCLK to HIRC and test: HXT, LXT, LIRC, MIRC
    CLK->PWRCTL |= CLK_PWRCTL_HIRC0EN_Msk;
    while( (CLK->STATUS & CLK_STATUS_HIRC0STB_Msk) != CLK_STATUS_HIRC0STB_Msk );
    CLK->CLKSEL0 = CLK->CLKSEL0 & (~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HIRC;
    
    //test HXT
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);
    CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_HXTEN_Msk) == CLK_PWRCTL_HXTEN_Msk );   
    CLK_DisableXtalRC(CLK_PWRCTL_HXTEN_Msk);   
    CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_HXTEN_Msk) == 0 );

    //test LXT
    CLK_EnableXtalRC(CLK_PWRCTL_LXTEN_Msk);
    CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_LXTEN_Msk) == CLK_PWRCTL_LXTEN_Msk );   
    CLK_DisableXtalRC(CLK_PWRCTL_LXTEN_Msk);   
    CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_LXTEN_Msk) == 0 );   

    //test LIRC
    CLK_EnableXtalRC(CLK_PWRCTL_LIRCEN_Msk);
    CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_LIRCEN_Msk) == CLK_PWRCTL_LIRCEN_Msk );   
    CLK_DisableXtalRC(CLK_PWRCTL_LIRCEN_Msk);   
    CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_LIRCEN_Msk) == 0 ); 
		
		//test MIRC
    CLK_EnableXtalRC(CLK_PWRCTL_MIRCEN_Msk);
    CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_MIRCEN_Msk) == CLK_PWRCTL_MIRCEN_Msk );   
    CLK_DisableXtalRC(CLK_PWRCTL_MIRCEN_Msk);   
    CU_ASSERT( (CLK->PWRCTL & CLK_PWRCTL_MIRCEN_Msk) == 0 );    
		
		/* Enable external 12MHz HXT, 32KHz LXT and HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_LIRCEN_Msk | CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk);

	  /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_LIRCSTB_Msk | CLK_STATUS_HIRC0STB_Msk | CLK_STATUS_HIRC1STB_Msk | CLK_STATUS_MIRCSTB_Msk);
		
		UART_Open(UART0, 115200);
		SYS_LockReg();
}


const uint32_t au32ModuleClkEnSel[] = { 
GPIO_MODULE     ,0x50000204,CLK_AHBCLK_GPIOCKEN_Msk,
PDMA_MODULE     ,0x50000204,CLK_AHBCLK_PDMACKEN_Msk,
ISP_MODULE      ,0x50000204,CLK_AHBCLK_ISPCKEN_Msk,	
SRAM_MODULE     ,0x50000204,CLK_AHBCLK_SRAMCKEN_Msk,
STC_MODULE      ,0x50000204,CLK_AHBCLK_STCKEN_Msk,

WDT_MODULE      ,0x50000208,CLK_APBCLK_WDTCKEN_Msk,
RTC_MODULE      ,0x50000208,CLK_APBCLK_RTCCKEN_Msk,
TMR0_MODULE     ,0x50000208,CLK_APBCLK_TMR0CKEN_Msk,
TMR1_MODULE     ,0x50000208,CLK_APBCLK_TMR1CKEN_Msk,
TMR2_MODULE     ,0x50000208,CLK_APBCLK_TMR2CKEN_Msk,
TMR3_MODULE     ,0x50000208,CLK_APBCLK_TMR3CKEN_Msk,
CLKO_MODULE     ,0x50000208,CLK_APBCLK_CLKOCKEN_Msk,
//DSRC_MODULE     ,0x50000208,CLK_APBCLK_DSRCCKEN_Msk,
I2C0_MODULE     ,0x50000208,CLK_APBCLK_I2C0CKEN_Msk,
I2C1_MODULE     ,0x50000208,CLK_APBCLK_I2C1CKEN_Msk,
ACMP0_MODULE    ,0x50000208,CLK_APBCLK_ACMP0CKEN_Msk,
SPI0_MODULE     ,0x50000208,CLK_APBCLK_SPI0CKEN_Msk,
SPI1_MODULE     ,0x50000208,CLK_APBCLK_SPI1CKEN_Msk,
SPI2_MODULE     ,0x50000208,CLK_APBCLK_SPI2CKEN_Msk,
SPI3_MODULE     ,0x50000208,CLK_APBCLK_SPI3CKEN_Msk,	
UART0_MODULE    ,0x50000208,CLK_APBCLK_UART0CKEN_Msk,
UART1_MODULE    ,0x50000208,CLK_APBCLK_UART1CKEN_Msk,
PWM0_MODULE     ,0x50000208,CLK_APBCLK_PWM0CKEN_Msk,
ADC_MODULE      ,0x50000208,CLK_APBCLK_ADCCKEN_Msk,
SC0_MODULE      ,0x50000208,CLK_APBCLK_SC0CKEN_Msk,
SC1_MODULE      ,0x50000208,CLK_APBCLK_SC1CKEN_Msk,
};

void TestFunc_CLK_EnableModuleClock() 
{
    uint8_t u8ModuleClkEnSelIdx;    //clock enable    
    SYS_UnlockReg();
    //wait UART print message finish
    UART_WAIT_TX_EMPTY(UART0);
    
    /* test loop */
    for( u8ModuleClkEnSelIdx=0; u8ModuleClkEnSelIdx<sizeof(au32ModuleClkEnSel)/sizeof(uint32_t); u8ModuleClkEnSelIdx+=3 )  
    {
        CLK_EnableModuleClock(au32ModuleClkEnSel[u8ModuleClkEnSelIdx]); 
        CU_ASSERT( (inp32(au32ModuleClkEnSel[u8ModuleClkEnSelIdx+1]) & au32ModuleClkEnSel[u8ModuleClkEnSelIdx+2]) == au32ModuleClkEnSel[u8ModuleClkEnSelIdx+2] );  
        CLK_DisableModuleClock(au32ModuleClkEnSel[u8ModuleClkEnSelIdx]);
        CU_ASSERT( (inp32(au32ModuleClkEnSel[u8ModuleClkEnSelIdx+1]) & au32ModuleClkEnSel[u8ModuleClkEnSelIdx+2]) == 0 );
        UART_WAIT_TX_EMPTY(UART0);
    }    
    //enable UART after test
    CLK->APBCLK |= CLK_APBCLK_UART0CKEN_Msk;    
    SYS_LockReg();
}

void TestFunc_CLK_EnablePLL() 
{
    uint32_t TestParamA;    //A PLL frequency
    SYS_UnlockReg();
	
    /* Test loop -> For PLL clock source is HXT */
    for( TestParamA=16000000; TestParamA<=32000000; TestParamA+=1000000 )
    {
        //CLK->PLLCTL |= CLK_PLLCTL_PD_Msk;

        CLK_DisablePLL();
        while( (CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == CLK_STATUS_PLLSTB_Msk );
        CU_ASSERT( CLK->PLLCTL & CLK_PLLCTL_PD_Msk );  
        CLK_EnablePLL( CLK_PLLCTL_PLL_SRC_HXT, TestParamA);

//         printf("%d = %d\n",TestParamA,CLK_GetPLLClockFreq());
        CU_ASSERT( (CLK_GetPLLClockFreq() > (TestParamA-3000000)) && 
                   (CLK_GetPLLClockFreq() < (TestParamA+3000000)) );
        CU_ASSERT( (CLK->PLLCTL & CLK_PLLCTL_PLLSRC_Msk)== CLK_PLLCTL_PLL_SRC_HXT);
    } 
    
    /* Test loop -> For PLL clock source is HIRC */
    for( TestParamA=16000000; TestParamA<=32000000; TestParamA+=1000000 )
    {
        //CLK->PLLCTL |= CLK_PLLCTL_PD_Msk;

        CLK_DisablePLL();
        while( (CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == CLK_STATUS_PLLSTB_Msk );
        CU_ASSERT( CLK->PLLCTL & CLK_PLLCTL_PD_Msk );

        CLK_EnablePLL( CLK_PLLCTL_PLL_SRC_HIRC, TestParamA);
//         printf("%d = %d\n",TestParamA,CLK_GetPLLClockFreq());
        CU_ASSERT( (CLK_GetPLLClockFreq() > (TestParamA-3000000)) && 
                   (CLK_GetPLLClockFreq() < (TestParamA+3000000)) );
        CU_ASSERT( (CLK->PLLCTL & CLK_PLLCTL_PLLSRC_Msk)== CLK_PLLCTL_PLL_SRC_HIRC );
    } 
		
		/* Test loop -> For PLL clock source is MIRC */
    for( TestParamA=16000000; TestParamA<=32000000; TestParamA+=1000000 )
    {
        //CLK->PLLCTL |= CLK_PLLCTL_PD_Msk;

        CLK_DisablePLL();
        while( (CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == CLK_STATUS_PLLSTB_Msk );
        CU_ASSERT( CLK->PLLCTL & CLK_PLLCTL_PD_Msk );

        CLK_EnablePLL( CLK_PLLCTL_PLL_SRC_MIRC, TestParamA);
//         printf("%d = %d\n",TestParamA,CLK_GetPLLClockFreq());
        CU_ASSERT( (CLK_GetPLLClockFreq() > (TestParamA-3000000)) && 
                   (CLK_GetPLLClockFreq() < (TestParamA+3000000)) );
        CU_ASSERT( (CLK->PLLCTL & CLK_PLLCTL_PLLSRC_Msk)== CLK_PLLCTL_PLL_SRC_MIRC );
    }
    
    //Disable PLL
    CLK->PLLCTL |= CLK_PLLCTL_PD_Msk; 
    
//     printf("CLK_PLLCTL_32MHz_HXT = %x\n",CLK_PLLCTL_32MHz_HXT);
		CU_ASSERT(CLK_PLLCTL_36MHz_HXT==0xB24);
    CU_ASSERT(CLK_PLLCTL_32MHz_HXT==0xB20);
    CU_ASSERT(CLK_PLLCTL_28MHz_HXT==0xB1C);
    CU_ASSERT(CLK_PLLCTL_24MHz_HXT==0xB18);
    CU_ASSERT(CLK_PLLCTL_22MHz_HXT==0xB16);
    CU_ASSERT(CLK_PLLCTL_16MHz_HXT==0xB10);
    
// 		printf("CLK_PLLCTL_32MHz_HIRC0 = %x\n",CLK_PLLCTL_32MHz_HIRC0);
		CU_ASSERT(CLK_PLLCTL_36MHz_HIRC0==0x20B24);
    CU_ASSERT(CLK_PLLCTL_32MHz_HIRC0==0x20B20);
    CU_ASSERT(CLK_PLLCTL_28MHz_HIRC0==0x20B1C);
    CU_ASSERT(CLK_PLLCTL_24MHz_HIRC0==0x20B18);
    CU_ASSERT(CLK_PLLCTL_22MHz_HIRC0==0x20B16);
    CU_ASSERT(CLK_PLLCTL_16MHz_HIRC0==0x20B10);
		
// 		printf("CLK_PLLCTL_32MHz_HIRC1 = %x\n",CLK_PLLCTL_32MHz_HIRC1);
		CU_ASSERT(CLK_PLLCTL_36MHz_HIRC1==0x22324);
    CU_ASSERT(CLK_PLLCTL_32MHz_HIRC1==0x22320);
    CU_ASSERT(CLK_PLLCTL_28MHz_HIRC1==0x2231C);
    CU_ASSERT(CLK_PLLCTL_24MHz_HIRC1==0x22318);
    CU_ASSERT(CLK_PLLCTL_22MHz_HIRC1==0x22316);
    CU_ASSERT(CLK_PLLCTL_16MHz_HIRC1==0x22310);
		
// 		printf("CLK_PLLCTL_32MHz_MIRC = %x\n",CLK_PLLCTL_32MHz_MIRC);
		CU_ASSERT(CLK_PLLCTL_36MHz_MIRC==0x40324);
		CU_ASSERT(CLK_PLLCTL_32MHz_MIRC==0x40320);
    CU_ASSERT(CLK_PLLCTL_28MHz_MIRC==0x4031C);
    CU_ASSERT(CLK_PLLCTL_24MHz_MIRC==0x40318);
    CU_ASSERT(CLK_PLLCTL_22MHz_MIRC==0x40316);
    CU_ASSERT(CLK_PLLCTL_16MHz_MIRC==0x40310);

    for( TestParamA=1; TestParamA<=0xF; TestParamA+=1 )
    {
		 //printf("\nCLK->PLLCTL = %x \n", CLK->PLLCTL);
     CLK->PLLCTL=(CLK->PLLCTL & ~CLK_PLLCTL_INDIV_Msk) | CLK_PLL_SRC_N(TestParamA);
     CU_ASSERT((CLK->PLLCTL&CLK_PLLCTL_INDIV_Msk)==(TestParamA-1)<<8);
		 //printf("\nCLK->PLLCTL = %x  (CLK->PLLCTL&CLK_PLLCTL_INDIV_Msk) = %x,  (TestParamA-1)<<8 = %x\n", CLK->PLLCTL, (CLK->PLLCTL&CLK_PLLCTL_INDIV_Msk), (TestParamA-1)<<8);
    }

    for( TestParamA=0; TestParamA<=0xF; TestParamA+=1 )
    {
     CLK->PLLCTL=(CLK->PLLCTL & ~CLK_PLLCTL_PLLMLP_Msk) | CLK_PLL_MLP(TestParamA);
     CU_ASSERT((CLK->PLLCTL&CLK_PLLCTL_PLLMLP_Msk)==(TestParamA));		 
    }

    SYS_LockReg();
}

void TestFunc_CLK_WaitClockReady() 
{
    //wait UART send message finish before change clock
    UART_WAIT_TX_EMPTY(UART0);
    SYS_UnlockReg();
    //set HCLK to HXT and test: HIRC, PLL, MIRC
    CLK->PWRCTL = (CLK->PWRCTL) | CLK_PWRCTL_HXTEN_Msk;
    while( (CLK->STATUS & CLK_STATUS_HXTSTB_Msk) != CLK_STATUS_HXTSTB_Msk );
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HXT;   

    //test HIRC
    CLK->PWRCTL &= ~CLK_PWRCTL_HIRC0EN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_HIRC0STB_Msk)==0 );
    CLK->PWRCTL |= CLK_PWRCTL_HIRC0EN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_HIRC0STB_Msk)==1 );
    
	  //test MIRC
    CLK->PWRCTL &= ~CLK_PWRCTL_MIRCEN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_MIRCSTB_Msk)==0 );
    CLK->PWRCTL |= CLK_PWRCTL_MIRCEN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_MIRCSTB_Msk)==1 );
	
    //test PLL    
    CLK->PLLCTL = 0xB20;  
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_PLLSTB_Msk)==1 ); 
    CLK->PLLCTL |= CLK_PLLCTL_PD_Msk;     
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_PLLSTB_Msk)==0 );     
    
    //set HCLK to HIRC and test: HXT, LIRC, MIRC
    CLK->PWRCTL |= CLK_PWRCTL_HIRC0EN_Msk;
    while( (CLK->STATUS & CLK_STATUS_HIRC0STB_Msk) != CLK_STATUS_HIRC0STB_Msk );
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HIRC;
    
    //test HXT
    CLK->PWRCTL &= ~CLK_PWRCTL_HXTEN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk)==0 );   
    CLK->PWRCTL = (CLK->PWRCTL) | CLK_PWRCTL_HXTEN_Msk;
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk)==1 );    

    //test LIRC
    CLK->PWRCTL &= ~CLK_PWRCTL_LIRCEN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk)==0 );
    CLK->PWRCTL |= CLK_PWRCTL_LIRCEN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk)==1 );

		//test MIRC
		CLK->PWRCTL &= ~CLK_PWRCTL_MIRCEN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_MIRCSTB_Msk)==0 );
    CLK->PWRCTL |= CLK_PWRCTL_MIRCEN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_MIRCSTB_Msk)==1 );
		
		
		//set HCLK to MIRC and test: HXT, LIRC, HIRC
    CLK->PWRCTL |= CLK_PWRCTL_MIRCEN_Msk;
    while( (CLK->STATUS & CLK_STATUS_MIRCSTB_Msk) != CLK_STATUS_MIRCSTB_Msk );
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_MIRC;
    
    //test HXT
    CLK->PWRCTL &= ~CLK_PWRCTL_HXTEN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk)==0 );   
    CLK->PWRCTL = (CLK->PWRCTL) | CLK_PWRCTL_HXTEN_Msk;
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk)==1 );    

    //test LIRC
    CLK->PWRCTL &= ~CLK_PWRCTL_LIRCEN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk)==0 );
    CLK->PWRCTL |= CLK_PWRCTL_LIRCEN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_LIRCSTB_Msk)==1 );

		//test HIRC
		CLK->PWRCTL &= ~CLK_PWRCTL_HIRC0EN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_HIRC0STB_Msk)==0 );
    CLK->PWRCTL |= CLK_PWRCTL_HIRC0EN_Msk;    
    CU_ASSERT( CLK_WaitClockReady(CLK_STATUS_HIRC0STB_Msk)==1 );
		
		CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_HCLK_CLK_DIVIDER(1));
		UART_Open(UART0, 115200);
    SYS_LockReg();
}

void TestFunc_CLK_TestMacro() 
{
    uint32_t TestParamA;    //A macro parameter     
    
    //APBCLK 
    for( TestParamA=0; TestParamA<(0x1+1); TestParamA++ )
    {
        CU_ASSERT( MODULE_APBCLK( MODULE_APBCLK_ENC(TestParamA) ) == TestParamA );  
    }
    
    //CLKSEL
    for( TestParamA=0; TestParamA<(0x3+1); TestParamA++ )
    {
        CU_ASSERT( MODULE_CLKSEL( MODULE_CLKSEL_ENC(TestParamA) ) == TestParamA );  
    }   

    //CLKSEL_Msk
    for( TestParamA=0; TestParamA<(0xf+1); TestParamA++ )
    {
        CU_ASSERT( MODULE_CLKSEL_Msk( MODULE_CLKSEL_Msk_ENC(TestParamA) ) == TestParamA );  
    }     
    
    //CLKSEL_Pos
    for( TestParamA=0; TestParamA<(0x1f+1); TestParamA++ )
    {
        CU_ASSERT( MODULE_CLKSEL_Pos( MODULE_CLKSEL_Pos_ENC(TestParamA) ) == TestParamA );  
    } 
    
    //CLKDIV
    for( TestParamA=0; TestParamA<(0x3+1); TestParamA++ )
    {
        CU_ASSERT( MODULE_CLKDIV( MODULE_CLKDIV_ENC(TestParamA) ) == TestParamA );  
    }     
    
    //CLKDIV_Msk
    for( TestParamA=0; TestParamA<(0xff+1); TestParamA++ )
    {
        CU_ASSERT( MODULE_CLKDIV_Msk( MODULE_CLKDIV_Msk_ENC(TestParamA) ) == TestParamA );  
    }     

    //CLKDIV_Pos
    for( TestParamA=0; TestParamA<(0x1f+1); TestParamA++ )
    {
        CU_ASSERT( MODULE_CLKDIV_Pos( MODULE_CLKDIV_Pos_ENC(TestParamA) ) == TestParamA );  
    }  

    //IP_EN
    for( TestParamA=0; TestParamA<(0x1f+1); TestParamA++ )
    {
        CU_ASSERT( MODULE_IP_EN_Pos( MODULE_IP_EN_Pos_ENC(TestParamA) ) == TestParamA );  
    }      
}

void TestFunc_CLK_TestConstant() 
{

    //CLK_PLLCTL_32MHz_HXT
    CLK->PLLCTL |= CLK_PLLCTL_PD_Msk;
    while( (CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == CLK_STATUS_PLLSTB_Msk );  
    CLK->PLLCTL = CLK_PLLCTL_32MHz_HXT;
    while( (CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == 0 );
    CU_ASSERT( CLK_GetPLLClockFreq() == 32000000 );

    //CLK_PLLCTL_32MHz_HIRC
    CLK->PLLCTL |= CLK_PLLCTL_PD_Msk;    
    while( (CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == CLK_STATUS_PLLSTB_Msk );  
    CLK->PLLCTL = CLK_PLLCTL_32MHz_HIRC0;
    while( (CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == 0 );
    CU_ASSERT( CLK_GetPLLClockFreq() == 32000000 );
	
	
		//CLK_PLLCTL_32MHz_HIRC
    CLK->PLLCTL |= CLK_PLLCTL_PD_Msk;    
    while( (CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == CLK_STATUS_PLLSTB_Msk );  
    CLK->PLLCTL = CLK_PLLCTL_32MHz_MIRC;
    while( (CLK->STATUS & CLK_STATUS_PLLSTB_Msk) == 0 );
    CU_ASSERT( CLK_GetPLLClockFreq() == 32000000 );

//     //FREQ_12MHZ
//     CU_ASSERT( FREQ_12MHZ == 12000000 );  
//     //FREQ_24MHZ
//     CU_ASSERT( FREQ_24MHZ == 24000000 );    
//     //FREQ_32MHZ
//     CU_ASSERT( FREQ_32MHZ == 32000000 );     
//     //FREQ_42MHZ
//     CU_ASSERT( FREQ_42MHZ == 42000000 );    
//     //FREQ_48MHZ
//     CU_ASSERT( FREQ_48MHZ == 48000000 );   
//     //FREQ_120MHZ
//     CU_ASSERT( FREQ_120MHZ == 120000000 );  
//     //FREQ_128MHZ
//     CU_ASSERT( FREQ_128MHZ == 128000000 );

    //FREQ_16MHZ
    CU_ASSERT( FREQ_16MHZ == 16000000 );
    //FREQ_48MHZ
    CU_ASSERT( FREQ_36MHZ == 36000000 );


    //CLK_PLLCTL_NR
    //for( i=2; i<=33; i++)
    //{
    //    CU_ASSERT( CLK_PLLCTL_NR(i) == ((i-2)<<9) ); 
    //}

    //CLK_PLLCTL_NF
    //for( i=2; i<=513; i++)
    //{
    //    CU_ASSERT( CLK_PLLCTL_NF(i) == (i-2) ); 
    //}    

    //CLK_PLLCTL_NO_1
    //CU_ASSERT( CLK_PLLCTL_NO_1 == (0<<14) ); 
    //CLK_PLLCTL_NO_2
    //CU_ASSERT( CLK_PLLCTL_NO_2 == (1<<14) ); 
    //CLK_PLLCTL_NO_4
    //CU_ASSERT( CLK_PLLCTL_NO_4 == (3<<14) );  

    //MODULE_NoMsk NA
    CU_ASSERT( MODULE_NoMsk == MODULE_NoMsk );
    SYS_LockReg();
}

void TestFunc_CLK_TestPowerDown() 
{
    SYS_UnlockReg();
    CLK_EnableModuleClock(WDT_MODULE);
    CLK_SetModuleClock(WDT_MODULE, 0, 0);
    
    /* Enable WDT NVIC */
    NVIC_EnableIRQ(WDT_IRQn);

    /* Enable WDT wake-up function and select time-out interval to 2^16 * WDT clock then start WDT counting */
    WDT_Open(WDT_TIMEOUT_2POW14, NULL, FALSE, TRUE); 

    /* Enable WDT interrupt function */
    WDT_EnableInt();

    /* System entry into Power-down Mode */
    CLK->PWRCTL |= CLK_PWRCTL_PDWKIEN_Msk;    
    UART_WAIT_TX_EMPTY(UART0);
	
    CLK_PowerDown();   
    
    CU_ASSERT( CLK->PWRCTL & CLK_PWRCTL_PDWKIEN_Msk ); 
    
    /* Clear Power-down wake-up interrupt flag */
    CLK->PWRCTL |= CLK_PWRCTL_PDWKIEN_Msk;  

    CLK_Idle();

    /* Disable WDT counting */
    WDT_Close();    

		//restore UART setting for printf
		CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_UART0_CLK_DIVIDER(1));
		UART_Open(UART0, 115200);
		
    SYS_LockReg();
}

// uint32_t MCLKSrcSel[]=
// {
// CLK_MCLKO_MCLK_SEL_ISP_CLK 	,0x00,
// CLK_MCLKO_MCLK_SEL_HIRC			,0x01,
// CLK_MCLKO_MCLK_SEL_HXT			,0x02,
// CLK_MCLKO_MCLK_SEL_LXT			,0x03,
// CLK_MCLKO_MCLK_SEL_LIRC			,0x04,
// CLK_MCLKO_MCLK_SEL_PLLO			,0x05,
// CLK_MCLKO_MCLK_SEL_PLLI			,0x06,
// CLK_MCLKO_MCLK_SEL_SYSTICK	,0x07,
// CLK_MCLKO_MCLK_SEL_HCLK			,0x08,
// CLK_MCLKO_MCLK_SEL_PCLK			,0x0A,
// CLK_MCLKO_MCLK_SEL_TMR0			,0x20,
// CLK_MCLKO_MCLK_SEL_TMR1			,0x21,
// CLK_MCLKO_MCLK_SEL_UART0		,0x22,
// CLK_MCLKO_MCLK_SEL_USB			,0x23,
// CLK_MCLKO_MCLK_SEL_ADC 			,0x24,
// CLK_MCLKO_MCLK_SEL_WDT 			,0x25,
// CLK_MCLKO_MCLK_SEL_PWM0CH01	,0x26,
// CLK_MCLKO_MCLK_SEL_PWM0CH23	,0x27,
// CLK_MCLKO_MCLK_SEL_LCD			,0x29,
// CLK_MCLKO_MCLK_SEL_TMR2			,0x38,
// CLK_MCLKO_MCLK_SEL_TMR3			,0x39,
// CLK_MCLKO_MCLK_SEL_UART1		,0x3A,
// CLK_MCLKO_MCLK_SEL_PWM1CH01	,0x3B,
// CLK_MCLKO_MCLK_SEL_PWM1CH23	,0x3C,
// CLK_MCLKO_MCLK_SEL_I2S			,0x3D,
// CLK_MCLKO_MCLK_SEL_SC0			,0x3E,
// CLK_MCLKO_MCLK_SEL_SC1			,0x3F,
// };
void TestFunc_CLK_TestOthers()
{
//   uint32_t i;
//   for(i=0;i<sizeof(MCLKSrcSel)/sizeof(uint32_t);i+=2)
//   {
//     CLK->MCLKO = (CLK->MCLKO & ~CLK_MCLKO_MCLK_SEL_Msk) | MCLKSrcSel[i];
//     CU_ASSERT( (CLK->MCLKO & CLK_MCLKO_MCLK_SEL_Msk)== (MCLKSrcSel[i+1]<<CLK_MCLKO_MCLK_SEL_Pos) );
//   }

  CU_ASSERT(CLK_PWRCTL_HXT_EN==CLK_PWRCTL_HXTEN_Msk);
  CU_ASSERT(CLK_PWRCTL_LXT_EN==CLK_PWRCTL_LXTEN_Msk);
  CU_ASSERT(CLK_PWRCTL_HIRC0_EN==CLK_PWRCTL_HIRC0EN_Msk);
	CU_ASSERT(CLK_PWRCTL_HIRC1_EN==CLK_PWRCTL_HIRC1EN_Msk);
  CU_ASSERT(CLK_PWRCTL_LIRC_EN==CLK_PWRCTL_LIRCEN_Msk);
  CU_ASSERT(CLK_PWRCTL_DELY_EN==CLK_PWRCTL_PDWKDLY_Msk);
  CU_ASSERT(CLK_PWRCTL_WAKEINT_EN==CLK_PWRCTL_PDWKIEN_Msk);
  CU_ASSERT(CLK_PWRCTL_PWRDOWN_EN==CLK_PWRCTL_PDEN_Msk);
  CU_ASSERT(CLK_PWRCTL_HXT_SELXT==CLK_PWRCTL_HXTSLTYP_Msk);

  CU_ASSERT(CLK_PWRCTL_HXT_GAIN_4M==(0<<CLK_PWRCTL_HXTGAIN_Pos));
  CU_ASSERT(CLK_PWRCTL_HXT_GAIN_4M_8M==(1<<CLK_PWRCTL_HXTGAIN_Pos));
  CU_ASSERT(CLK_PWRCTL_HXT_GAIN_8M_12M==(2<<CLK_PWRCTL_HXTGAIN_Pos));
  CU_ASSERT(CLK_PWRCTL_HXT_GAIN_12M_16M==(3<<CLK_PWRCTL_HXTGAIN_Pos));
  CU_ASSERT(CLK_PWRCTL_HXT_GAIN_16M_24M==(4<<CLK_PWRCTL_HXTGAIN_Pos));
  CU_ASSERT(CLK_PWRCTL_HXT_GAIN_24M_32M==(5<<CLK_PWRCTL_HXTGAIN_Pos));
  CU_ASSERT(CLK_PWRCTL_HXT_GAIN_32M_36M==(6<<CLK_PWRCTL_HXTGAIN_Pos));
  CU_ASSERT(CLK_PWRCTL_HXT_GAIN_36M==(7<<CLK_PWRCTL_HXTGAIN_Pos));	

  CU_ASSERT(CLK_CLKO_EN==CLK_CLKOCTL_CLKOEN_Msk);

  CU_ASSERT(CLK_WK_INTSTS_IS==CLK_WKINTSTS_PDWKIF_Msk);
	
	
}

CU_TestInfo CLK_TestCases[] =
{
#if 1
    {"Testing CLK_EnableCKO Function:", TestFunc_CLK_EnableCKO},    
    {"Testing CLK_GetHXTFreq Function:", TestFunc_CLK_GetHXTFreq},
    {"Testing CLK_GetHCLKFreq Function:", TestFunc_CLK_GetHCLKFreq},
    {"Testing CLK_EnableXtalRC Function:", TestFunc_CLK_EnableXtalRC},
    {"Testing CLK_SetCoreClock Function:", TestFunc_CLK_SetCoreClock},	
	  {"Testing CLK_SetSysTickClockSrc Function:", TestFunc_CLK_SetSysTickClock},
    {"Testing CLK_EnableModuleClock Function:", TestFunc_CLK_EnableModuleClock},
    {"Testing CLK_SetModuleClock Function:", TestFunc_CLK_SetModuleClock},
#endif

#if 0
		{"Testing CLK_EnablePLL Function:", TestFunc_CLK_EnablePLL},
		{"Testing Macro:", TestFunc_CLK_TestMacro},
		{"Testing Constant:", TestFunc_CLK_TestConstant},
		{"Testing Others:", TestFunc_CLK_TestOthers},
		{"Testing CLK_WaitClockReady Function:", TestFunc_CLK_WaitClockReady},	// need to wait 
    {"Testing PowerDown:", TestFunc_CLK_TestPowerDown},  // show WDT message only 
#endif

    CU_TEST_INFO_NULL
};

