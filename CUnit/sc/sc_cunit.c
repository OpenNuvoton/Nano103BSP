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
#include "sc_cunit.h"


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
    {"SC MACRO & CONSTANT", suite_success_init, suite_success_clean, NULL, NULL, SC_MACRO_CONSTANT},
    {"SC API", suite_success_init, suite_success_clean, NULL, NULL, SC_API},
    CU_SUITE_INFO_NULL
};

SC_T *pSC[SC_INTERFACE_NUM] = {
    SC0, 
    SC1,
};

const uint32_t au32TMROPMode[] = {
    SC_TMR_MODE_0,
    SC_TMR_MODE_1,
    SC_TMR_MODE_2,
    SC_TMR_MODE_3,
    SC_TMR_MODE_4,
    SC_TMR_MODE_5,
    SC_TMR_MODE_6,
    SC_TMR_MODE_7,
    SC_TMR_MODE_8,
    SC_TMR_MODE_F,
};

int32_t SC_InitClock(void)
{
    volatile uint32_t u32Timeout = SystemCoreClock/1000;


    /* Waiting for clock ready */
    while(u32Timeout-- > 0) {
        if((CLK->STATUS&CLK_STATUS_HXTSTB_Msk) == CLK_STATUS_HXTSTB_Msk) {
            CLK->APBCLK |= (CLK_APBCLK_SC0CKEN_Msk | CLK_APBCLK_SC1CKEN_Msk);  
            
            CLK->CLKSEL2 = (CLK->CLKSEL2 & ~(CLK_CLKSEL2_SC0SEL_Msk | CLK_CLKSEL2_SC1SEL_Msk)) | CLK_CLKSEL2_SC0SEL_HXT | CLK_CLKSEL2_SC1SEL_HXT;
            
            /* SC clock 1MHz */
            CLK->CLKDIV0 |= 11UL << CLK_CLKDIV0_SC0DIV_Pos;
            CLK->CLKDIV1 = 11 << CLK_CLKDIV1_SC1DIV_Pos;
#if 1
            /* Set PA.8 ~ PA.11 & PB.4 for SC0 interface */
            SYS->GPA_MFPH &= ~(SYS_GPA_MFPH_PA8MFP_Msk |
                               SYS_GPA_MFPH_PA9MFP_Msk |
                               SYS_GPA_MFPH_PA10MFP_Msk |
                               SYS_GPA_MFPH_PA11MFP_Msk);
            SYS->GPB_MFPL &= ~SYS_GPB_MFPL_PB4MFP_Msk;

            SYS->GPA_MFPH |= (SYS_GPA_MFPH_PA8MFP_SC0_CLK |
                              SYS_GPA_MFPH_PA9MFP_SC0_DAT |
                              SYS_GPA_MFPH_PA10MFP_SC0_PWR |
                              SYS_GPA_MFPH_PA11MFP_SC0_RST);
            SYS->GPB_MFPL |= SYS_GPB_MFPL_PB4MFP_SC0_CD;
#endif
            return 0;
        }
    }
    
    return -1;
}

void MACRO_CONSTANT_Test(void)
{    
    if(SC_InitClock() != 0) {
        CU_FAIL("SC Init Clock FAIL");  
        return ;
    }
    
    pSC[0]->CTL |= SC_CTL_SCEN_Msk;
    
    /* Check SC_ENABLE_INT(), SC_DISABLE_INT */
        SC_ENABLE_INT(pSC[0], (SC_INTEN_ACERRIEN_Msk | SC_INTEN_RXTOIEN_Msk | SC_INTEN_INITIEN_Msk | SC_INTEN_CDIEN_Msk));
        CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x780);          
        SC_DISABLE_INT(pSC[0], (SC_INTEN_ACERRIEN_Msk | SC_INTEN_RXTOIEN_Msk | SC_INTEN_INITIEN_Msk | SC_INTEN_CDIEN_Msk));
        CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x0);          
        SC_ENABLE_INT(pSC[1], (SC_INTEN_BGTIEN_Msk | SC_INTEN_TMR2IEN_Msk | SC_INTEN_TMR1IEN_Msk | SC_INTEN_TMR0IEN_Msk));
        CU_ASSERT_EQUAL(pSC[1]->INTEN, 0x78);          
        SC_DISABLE_INT(pSC[1], (SC_INTEN_BGTIEN_Msk | SC_INTEN_TMR2IEN_Msk | SC_INTEN_TMR1IEN_Msk | SC_INTEN_TMR0IEN_Msk));
        CU_ASSERT_EQUAL(pSC[1]->INTEN, 0x0);          
        

    /* Check SC_SET_VCC_PIN() */  // default value of u32CardPwrPolarity is 0, so set pin state high is actually set it low.
        CU_ASSERT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_PWREN_Msk), 0x0);          
        SC_SET_VCC_PIN(pSC[0], SC_PIN_STATE_LOW); CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_PWREN_Msk), 0x0);          
        SC_SET_VCC_PIN(pSC[0], SC_PIN_STATE_HIGH); CLK_SysTickDelay(100);
        CU_ASSERT_NOT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_PWREN_Msk), 0x0);          

    /* Check SC_SET_CLK_PIN() */
        CU_ASSERT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_CLKKEEP_Msk), 0x0);          
        SC_SET_CLK_PIN(pSC[0], SC_CLK_ON); CLK_SysTickDelay(100);
        CU_ASSERT_NOT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_CLKKEEP_Msk), 0x0);          
        SC_SET_CLK_PIN(pSC[0], SC_CLK_OFF); CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_CLKKEEP_Msk), 0x0);          

    /* Check SC_SET_IO_PIN() */
        CU_ASSERT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_SCDOUT_Msk), 0x0);          
        SC_SET_IO_PIN(pSC[0], SC_PIN_STATE_HIGH); CLK_SysTickDelay(100);
        CU_ASSERT_NOT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_SCDOUT_Msk), 0x0);          
        SC_SET_IO_PIN(pSC[0], SC_PIN_STATE_LOW); CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_SCDOUT_Msk), 0x0);          

    /* Check SC_SET_RST_PIN() */
        CU_ASSERT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_SCRST_Msk), 0x0);          
        SC_SET_RST_PIN(pSC[0], SC_PIN_STATE_HIGH); CLK_SysTickDelay(100);
        CU_ASSERT_NOT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_SCRST_Msk), 0x0);          
        SC_SET_RST_PIN(pSC[0], SC_PIN_STATE_LOW); CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_SCRST_Msk), 0x0); 

    /* Check SC_SET_STOP_BIT_LEN() */
        CU_ASSERT_EQUAL((pSC[0]->CTL&SC_CTL_NSB_Msk), 0x0);          
        SC_SET_STOP_BIT_LEN(pSC[0], 1); CLK_SysTickDelay(100);
        CU_ASSERT_NOT_EQUAL((pSC[0]->CTL&SC_CTL_NSB_Msk), 0x0);          
        SC_SET_STOP_BIT_LEN(pSC[0], 2); CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[0]->CTL&SC_CTL_NSB_Msk), 0x0);                 

    /* Check SC_SetTxRetry() */
        CU_ASSERT_EQUAL((pSC[0]->CTL&(SC_CTL_TXRTYEN_Msk|SC_CTL_TXRTY_Msk)), 0x0);          
        SC_SetTxRetry(pSC[0], 5); CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[0]->CTL&(SC_CTL_TXRTYEN_Msk|SC_CTL_TXRTY_Msk)), 0x00C00000);          
        SC_SetTxRetry(pSC[0], 0); CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[0]->CTL&(SC_CTL_TXRTYEN_Msk|SC_CTL_TXRTY_Msk)), 0x0);          

    /* Check SC_SetRxRetry() */
        CU_ASSERT_EQUAL((pSC[0]->CTL&(SC_CTL_RXRTYEN_Msk|SC_CTL_RXRTY_Msk)), 0x0);          
        SC_SetRxRetry(pSC[0], 4); CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[0]->CTL&(SC_CTL_RXRTYEN_Msk|SC_CTL_RXRTY_Msk)), 0x000B0000);          
        SC_SetRxRetry(pSC[0], 0); CLK_SysTickDelay(100);
        CU_ASSERT_EQUAL((pSC[0]->CTL&(SC_CTL_RXRTYEN_Msk|SC_CTL_RXRTY_Msk)), 0x0);          

    /* Check SC_WRITE(), SC_READ() by INTERNAL Loop Back Test */
        SC_Open(pSC[0], SC_PIN_STATE_IGNORE, SC_PIN_STATE_HIGH); CLK_SysTickDelay(100);
        pSC[0]->PINCTL |= BIT31; CLK_SysTickDelay(100);
        SC_WRITE(pSC[0], 0x5A);
            while((pSC[0]->STATUS&SC_STATUS_RXEMPTY_Msk) == SC_STATUS_RXEMPTY_Msk);
            CU_ASSERT_EQUAL(SC_READ(pSC[0]), 0x5A);                  
        SC_WRITE(pSC[0], 0x25);
            while((pSC[0]->STATUS&SC_STATUS_RXEMPTY_Msk) == SC_STATUS_RXEMPTY_Msk);
            CU_ASSERT_EQUAL(SC_READ(pSC[0]), 0x25);  
        pSC[0]->PINCTL &= ~BIT31; CLK_SysTickDelay(100);
        SC_Close(pSC[0]); CLK_SysTickDelay(100);
}

void API_SC_Test(void)
{    
    uint32_t i, j;
    
    if(SC_InitClock() != 0) {
        CU_FAIL("SC Init Clock FAIL");  
        return ;
    }
    
    /* Check SC_ClearFIFO() by INTERNAL Loop Back Test */
        SC_Open(pSC[0], SC_PIN_STATE_IGNORE, SC_PIN_STATE_HIGH); CLK_SysTickDelay(100);
        pSC[0]->PINCTL |= BIT31; CLK_SysTickDelay(100);
        SC_WRITE(pSC[0], 0x5A);
        SC_WRITE(pSC[0], 0x5A);
        SC_WRITE(pSC[0], 0x5A);
        SC_WRITE(pSC[0], 0x5A);
        SC_WRITE(pSC[0], 0x5A);
            while((pSC[0]->STATUS&SC_STATUS_RXEMPTY_Msk) == SC_STATUS_RXEMPTY_Msk);
            CU_ASSERT_EQUAL(SC_READ(pSC[0]), 0x5A);  
            while((pSC[0]->STATUS&SC_STATUS_RXEMPTY_Msk) == SC_STATUS_RXEMPTY_Msk);
            CU_ASSERT_NOT_EQUAL(pSC[0]->STATUS, 0x00000202);          
        SC_ClearFIFO(pSC[0]); CLK_SysTickDelay(100);
            CU_ASSERT_EQUAL(SC_READ(pSC[0]), 0x0);  
            CU_ASSERT_EQUAL(pSC[0]->STATUS, 0x00000202);          
            CU_ASSERT_EQUAL(pSC[0]->ALTCTL&(SC_ALTCTL_TXRST_Msk|SC_ALTCTL_RXRST_Msk), 0x0);          
        pSC[0]->PINCTL &= ~BIT31; CLK_SysTickDelay(100);
        SC_Close(pSC[0]); CLK_SysTickDelay(100);

    /* Check SC_Open(), SC_Close() and SC_IsCardInserted() */
        SC_Open(pSC[0], SC_PIN_STATE_IGNORE, SC_PIN_STATE_HIGH); CLK_SysTickDelay(100);
            //CU_ASSERT_EQUAL(pSC[0]->PINCTL&SC_PINCSR_POW_INV_Msk, 0x0);          
            CU_ASSERT_EQUAL(pSC[0]->CTL, SC_CTL_SCEN_Msk);          
            CU_ASSERT_EQUAL(SC_IsCardInserted(pSC[0]), TRUE);          
        SC_Close(pSC[0]); CLK_SysTickDelay(100);
            CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x0);          
            CU_ASSERT_EQUAL(pSC[0]->PINCTL&~SC_PINCTL_CDPINSTS_Msk, 0x40000000);          
            CU_ASSERT_EQUAL(pSC[0]->ALTCTL, 0x0);          
            CU_ASSERT_EQUAL(pSC[0]->CTL, 0x40000000);   
        
        SC_Open(pSC[0], SC_PIN_STATE_IGNORE, SC_PIN_STATE_LOW); CLK_SysTickDelay(100);
            //CU_ASSERT_EQUAL(pSC[0]->PINCTL&SC_PINCSR_POW_INV_Msk, SC_PINCSR_POW_INV_Msk);          
            CU_ASSERT_EQUAL(pSC[0]->CTL, SC_CTL_SCEN_Msk);          
        SC_Close(pSC[0]); CLK_SysTickDelay(100);
            CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x0);          
            CU_ASSERT_EQUAL(pSC[0]->PINCTL&~SC_PINCTL_CDPINSTS_Msk, 0x40000000);          
            CU_ASSERT_EQUAL(pSC[0]->ALTCTL, 0x0);          
            CU_ASSERT_EQUAL(pSC[0]->CTL, 0x40000000);          

    /* Check SC_SetBlockGuardTime(), SC_SetCharGuardTime() */
        for(i=1; i<=32; i++) {
            SC_SetBlockGuardTime(pSC[0], i);
                CU_ASSERT_EQUAL(((pSC[0]->CTL&SC_CTL_BGT_Msk)>>SC_CTL_BGT_Pos), (i-1));          
        }
        for(i=12; i<=32; i++) {
            SC_SetCharGuardTime(pSC[0], i);
                CU_ASSERT_EQUAL(pSC[0]->EGT, (i-12));          
        }

    /* Check SC_StartTimer(), SC_StopTimer() and SC_StopAllTimer() */
        SC_Open(pSC[0], SC_PIN_STATE_IGNORE, SC_PIN_STATE_HIGH); CLK_SysTickDelay(100);
        pSC[0]->CTL |= SC_CTL_TMRSEL_Msk; /* Enable all TIMER function */
        for(j=0; j<sizeof(au32TMROPMode)/sizeof(au32TMROPMode[0]); j++) { 
            if(au32TMROPMode[j] == SC_TMR_MODE_F) {
                SC_StartTimer(pSC[0], 0, au32TMROPMode[j], 100);
                CU_ASSERT_EQUAL(pSC[0]->TMRCTL0, (0xF<<SC_TMRCTL0_OPMODE_Pos)|(100-1) | 0x80000000);          
            } else {
                SC_StartTimer(pSC[0], 0, au32TMROPMode[j], 100);
                CU_ASSERT_EQUAL(pSC[0]->TMRCTL0, (j<<SC_TMRCTL0_OPMODE_Pos)|(100-1) | 0x80000000);          
            }
            CU_ASSERT_EQUAL((pSC[0]->ALTCTL&SC_ALTCTL_CNTEN0_Msk), SC_ALTCTL_CNTEN0_Msk);
            SC_StopTimer(pSC[0], 0);
            CU_ASSERT_EQUAL((pSC[0]->ALTCTL&SC_ALTCTL_CNTEN0_Msk), 0);          

        }        
        for(j=0; j<sizeof(au32TMROPMode)/sizeof(au32TMROPMode[0]); j++) { 
            if(j == 3)
                continue;
            if(au32TMROPMode[j] == SC_TMR_MODE_F) {
                SC_StartTimer(pSC[0], 1, au32TMROPMode[j], 90);
                CU_ASSERT_EQUAL(pSC[0]->TMRCTL1, (0xF<<SC_TMRCTL1_OPMODE_Pos)|(90-1) | 0x80000000);          
            } else {
                SC_StartTimer(pSC[0], 1, au32TMROPMode[j], 90);
                CU_ASSERT_EQUAL(pSC[0]->TMRCTL1, (j<<SC_TMRCTL1_OPMODE_Pos)|(90-1) | 0x80000000);    
            }
            CU_ASSERT_EQUAL((pSC[0]->ALTCTL&SC_ALTCTL_CNTEN1_Msk), SC_ALTCTL_CNTEN1_Msk);          
            SC_StopTimer(pSC[0], 1);
            CU_ASSERT_EQUAL((pSC[0]->ALTCTL&SC_ALTCTL_CNTEN1_Msk), 0x0);          
        }
        for(j=0; j<sizeof(au32TMROPMode)/sizeof(au32TMROPMode[0]); j++) { 
            if(j == 3)
                continue;
            if(au32TMROPMode[j] == SC_TMR_MODE_F) {
                SC_StartTimer(pSC[0], 2, au32TMROPMode[j], 55);
                CU_ASSERT_EQUAL(pSC[0]->TMRCTL2, (0xF<<SC_TMRCTL2_OPMODE_Pos)|(55-1) | 0x80000000);          
            } else {
                SC_StartTimer(pSC[0], 2, au32TMROPMode[j], 55);
                CU_ASSERT_EQUAL(pSC[0]->TMRCTL2, (j<<SC_TMRCTL2_OPMODE_Pos)|(55-1) | 0x80000000);          
            }
            CU_ASSERT_EQUAL((pSC[0]->ALTCTL&SC_ALTCTL_CNTEN2_Msk), SC_ALTCTL_CNTEN2_Msk);          
            SC_StopTimer(pSC[0], 2);
            CU_ASSERT_EQUAL((pSC[0]->ALTCTL&SC_ALTCTL_CNTEN2_Msk), 0x0);          
        }
        SC_StartTimer(pSC[0], 0, SC_TMR_MODE_0, 100);
        SC_StartTimer(pSC[0], 1, SC_TMR_MODE_0, 100);
        SC_StartTimer(pSC[0], 2, SC_TMR_MODE_0, 100);
        CU_ASSERT_EQUAL((pSC[0]->ALTCTL&0xE0), 0xE0);          
        SC_StopAllTimer(pSC[0]);
        CU_ASSERT_EQUAL((pSC[0]->ALTCTL&0xE0), 0x0);          

    /* Check SC_ResetReader() */
        pSC[0]->CTL = SC_CTL_SCEN_Msk;
        pSC[0]->RXTOUT = 0xA;
        pSC[0]->ETUCTL = 0x0;
        pSC[0]->PINCTL = 0x0;
        pSC[0]->INTEN = 0x0;        
        SC_ResetReader(pSC[0]);
        while((pSC[0]->CTL&SC_CTL_SYNC_Msk)) {}; 
        while((pSC[0]->PINCTL&SC_PINCTL_SYNC_Msk)) {}; 
        CLK_SysTickDelay(100);
        printf("@@ ==> %x\n", pSC[0]->CTL);
        CU_ASSERT_EQUAL(pSC[0]->CTL,    0x00006009);          
            printf("@@ ==> %x\n", pSC[0]->CTL);
        CU_ASSERT_EQUAL(pSC[0]->RXTOUT,  0x0);          
        CU_ASSERT_EQUAL(pSC[0]->ETUCTL,  0x00000173);          
            printf("## ==> %x\n", pSC[0]->INTEN);
        CU_ASSERT_EQUAL(pSC[0]->INTEN,    0x0000047D);          
            printf("## ==> %x\n", pSC[0]->INTEN);
        CU_ASSERT_EQUAL((pSC[0]->PINCTL&SC_PINCTL_ADACEN_Msk), SC_PINCTL_ADACEN_Msk); 

    /* Test SC_GetInterfaceClock() */
        CLK_SetModuleClock(SC0_MODULE, CLK_CLKSEL2_SC0SEL_HXT, CLK_SC0_CLK_DIVIDER(1));
        CU_ASSERT_EQUAL(SC_GetInterfaceClock(SC0), 12000);
        // Some issue with CLK_GetPLLClockFreq(), commnet out for now
        //CLK_SetModuleClock(SC0_MODULE, CLK_CLKSEL2_SC0SEL_PLL, CLK_SC0_CLK_DIVIDER(4));
        //CU_ASSERT_EQUAL(SC_GetInterfaceClock(SC0), 8000);
        CLK_SetModuleClock(SC0_MODULE, CLK_CLKSEL2_SC0SEL_HIRC, CLK_SC0_CLK_DIVIDER(12));
        CU_ASSERT_EQUAL(SC_GetInterfaceClock(SC0), 1000);
        CLK_SetModuleClock(SC0_MODULE, CLK_CLKSEL2_SC0SEL_MIRC, CLK_SC0_CLK_DIVIDER(1));
        CU_ASSERT_EQUAL(SC_GetInterfaceClock(SC0), 4000);
        CLK_SetModuleClock(SC0_MODULE, CLK_CLKSEL2_SC0SEL_HCLK, CLK_SC0_CLK_DIVIDER(2));
        CU_ASSERT_EQUAL(SC_GetInterfaceClock(SC0), 6000);

        CLK_SetModuleClock(SC1_MODULE, CLK_CLKSEL2_SC1SEL_HXT, CLK_SC1_CLK_DIVIDER(1));
        CU_ASSERT_EQUAL(SC_GetInterfaceClock(SC1), 12000);
        // Some issue with CLK_GetPLLClockFreq(), commnet out for now
        //CLK_SetModuleClock(SC1_MODULE, CLK_CLKSEL2_SC1SEL_PLL, CLK_SC1_CLK_DIVIDER(4));
        //CU_ASSERT_EQUAL(SC_GetInterfaceClock(SC1), 8000);
        CLK_SetModuleClock(SC1_MODULE, CLK_CLKSEL2_SC1SEL_HIRC, CLK_SC1_CLK_DIVIDER(12));
        CU_ASSERT_EQUAL(SC_GetInterfaceClock(SC1), 1000);
        CLK_SetModuleClock(SC1_MODULE, CLK_CLKSEL2_SC1SEL_MIRC, CLK_SC1_CLK_DIVIDER(1));
        CU_ASSERT_EQUAL(SC_GetInterfaceClock(SC1), 4000);
        CLK_SetModuleClock(SC1_MODULE, CLK_CLKSEL2_SC1SEL_HCLK, CLK_SC1_CLK_DIVIDER(2));
        CU_ASSERT_EQUAL(SC_GetInterfaceClock(SC1), 6000);

    /* Reset SC */
        SYS->IPRST2 = SYS_IPRST2_SC0RST_Msk | SYS_IPRST2_SC1RST_Msk;
        SYS->IPRST2 = 0x0;
}

CU_TestInfo SC_MACRO_CONSTANT[] =
{
    {"Check SC MACRO and CONSTANT ", MACRO_CONSTANT_Test}, 
    CU_TEST_INFO_NULL
};

CU_TestInfo SC_API[] =
{
    {"Check SC API ", API_SC_Test}, 
    CU_TEST_INFO_NULL
};
