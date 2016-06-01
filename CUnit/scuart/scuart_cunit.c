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
#include "scuart_cunit.h"


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
    {"SCUART MACRO & CONSTANT", suite_success_init, suite_success_clean, NULL, NULL, SCUART_MACRO_CONSTANT},
    {"SCUART API", suite_success_init, suite_success_clean, NULL, NULL, SCUART_API},
    CU_SUITE_INFO_NULL
};

SC_T *pSC[SC_INTERFACE_NUM] = {
    SC0, 
    SC1,
};

const uint32_t au32CHARLen[] = {
    SCUART_CHAR_LEN_5,
    SCUART_CHAR_LEN_6,
    SCUART_CHAR_LEN_7,
    SCUART_CHAR_LEN_8,
};

const uint32_t au32PARITYSel[] = {
    SCUART_PARITY_NONE,
    SCUART_PARITY_ODD,
    SCUART_PARITY_EVEN,
};

const uint32_t au32Result[] = {
    0x00000031+0x40,
    0x00000031+0x80,
    0x00000031+0x00,
    0x00000021+0x40,
    0x00000021+0x80,
    0x00000021+0x00,
    0x00000011+0x40,
    0x00000011+0x80,
    0x00000011+0x00,
    0x00000001+0x40,
    0x00000001+0x80,
    0x00000001+0x00,
};

int32_t SCUART_InitClock(void)
{
    volatile uint32_t u32Timeout = SystemCoreClock/1000;
    
    /* Enable HXT */
    CLK->PWRCTL |= CLK_PWRCTL_HXTEN_Msk;

    /* Waiting for clock ready */
    while(u32Timeout-- > 0) {
        if((CLK->STATUS&CLK_STATUS_HXTSTB_Msk) == CLK_STATUS_HXTSTB_Msk) {
            CLK->APBCLK |= (CLK_APBCLK_SC0CKEN_Msk | CLK_APBCLK_SC1CKEN_Msk);  
            
            CLK->CLKSEL2 = (CLK->CLKSEL2 & ~(CLK_CLKSEL2_SC0SEL_Msk | CLK_CLKSEL2_SC1SEL_Msk)) | CLK_CLKSEL2_SC0SEL_HXT | CLK_CLKSEL2_SC1SEL_HXT;
#if 1
            /* Set PA.8 and PA.9 pin for SC UART mode */
            SYS->GPA_MFPH &= ~(SYS_GPA_MFPH_PA8MFP_Msk | SYS_GPA_MFPH_PA9MFP_Msk);
            SYS->GPA_MFPH |= (SYS_GPA_MFPH_PA8MFP_SC0_CLK | SYS_GPA_MFPH_PA9MFP_SC0_DAT);
#endif
            return 0;
        }
    }
    
    return -1;
}

void MACRO_CONSTANT_Test(void)
{    
    uint32_t i, j, k;
    uint8_t u8RDA[4];
    
    /* Reset all SC */
        SYS->IPRST2 = SYS_IPRST2_SC0RST_Msk | SYS_IPRST2_SC1RST_Msk;
        SYS->IPRST2 = 0x0;

    if(SCUART_InitClock() != 0) {
        CU_FAIL("SCUART Init Clock FAIL");  
        return ;
    }
    
    /* Check CONSTANT: 
        SCUART_CHAR_LEN_5, SCUART_CHAR_LEN_6, SCUART_CHAR_LEN_7, SCUART_CHAR_LEN_8,
        SCUART_PARITY_NONE, SCUART_PARITY_ODD, SCUART_PARITY_EVEN,
        SCUART_STOP_BIT_1, SCUART_STOP_BIT_2
       Check SCUART_SetLineConfig() and SCUART_Close() API
    */
        // SC0
        k = 0;
        for(i=0; i<sizeof(au32CHARLen)/sizeof(au32CHARLen[0]); i++) {
            for(j=0; j<sizeof(au32PARITYSel)/sizeof(au32PARITYSel[0]); j++) {
                SCUART_SetLineConfig(pSC[0], 115200, au32CHARLen[i], au32PARITYSel[j], SCUART_STOP_BIT_1);
                D_msg("BAUD:%d, REG:0x%08x\n", u32CurBAUD, pSC[0]->UARTCTL); 
                CU_ASSERT_EQUAL(pSC[0]->CTL, 0x8001);          
                CU_ASSERT_EQUAL(pSC[0]->UARTCTL, au32Result[k++]); 
                SCUART_Close(pSC[0]);
                CU_ASSERT_EQUAL(pSC[0]->CTL, 0x40000000);  // SYNC bit not cleared since SCEN disabled...
                CU_ASSERT_EQUAL(pSC[0]->UARTCTL, 0x0);          
            }
        }
        k = 0;
        for(i=0; i<sizeof(au32CHARLen)/sizeof(au32CHARLen[0]); i++) {
            for(j=0; j<sizeof(au32PARITYSel)/sizeof(au32PARITYSel[0]); j++) {
                SCUART_SetLineConfig(pSC[0], 115200, au32CHARLen[i], au32PARITYSel[j], SCUART_STOP_BIT_2);
                D_msg("BAUD:%d, REG:0x%08x\n", u32CurBAUD, pSC[0]->UARTCTL); 
                CU_ASSERT_EQUAL(pSC[0]->CTL, 0x0001);          
                CU_ASSERT_EQUAL(pSC[0]->UARTCTL, au32Result[k++]);          
                SCUART_Close(pSC[0]);
                CU_ASSERT_EQUAL(pSC[0]->CTL, 0x40000000);  // SYNC bit not cleared since SCEN disabled...
                CU_ASSERT_EQUAL(pSC[0]->UARTCTL, 0x0);          
            }
        }
        // SC1
        k = 0;
        for(i=0; i<sizeof(au32CHARLen)/sizeof(au32CHARLen[0]); i++) {
            for(j=0; j<sizeof(au32PARITYSel)/sizeof(au32PARITYSel[0]); j++) {
                SCUART_SetLineConfig(pSC[1], 115200, au32CHARLen[i], au32PARITYSel[j], SCUART_STOP_BIT_1);
                D_msg("BAUD:%d, REG:0x%08x\n", u32CurBAUD, pSC[1]->UARTCTL); 
                CU_ASSERT_EQUAL(pSC[1]->CTL, 0x8001);          
                CU_ASSERT_EQUAL(pSC[1]->UARTCTL, au32Result[k++]);          
                SCUART_Close(pSC[1]); 
                CU_ASSERT_EQUAL(pSC[1]->CTL, 0x40000000);  // SYNC bit not cleared since SCEN disabled...
                CU_ASSERT_EQUAL(pSC[1]->UARTCTL, 0x0);          
            }
        }
        k = 0;
        for(i=0; i<sizeof(au32CHARLen)/sizeof(au32CHARLen[0]); i++) {
            for(j=0; j<sizeof(au32PARITYSel)/sizeof(au32PARITYSel[0]); j++) {
                SCUART_SetLineConfig(pSC[1], 115200, au32CHARLen[i], au32PARITYSel[j], SCUART_STOP_BIT_2);
                D_msg("BAUD:%d, REG:0x%08x\n", u32CurBAUD, pSC[1]->UARTCTL); 
                CU_ASSERT_EQUAL(pSC[1]->CTL, 0x0001);          
                CU_ASSERT_EQUAL(pSC[1]->UARTCTL, au32Result[k++]);          
                SCUART_Close(pSC[1]);
                CU_ASSERT_EQUAL(pSC[1]->CTL, 0x40000000);  // SYNC bit not cleared since SCEN disabled...
                CU_ASSERT_EQUAL(pSC[1]->UARTCTL, 0x0);          
            }
        }
        
    /* Check MACRO: SCUART_ENABLE_INT(), SCUART_DISABLE_INT() */
        SCUART_ENABLE_INT(pSC[0], SC_INTEN_RXTOIEN_Msk);
            CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x200);          
        SCUART_DISABLE_INT(pSC[0], SC_INTEN_RXTOIEN_Msk);
            CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x000);          
        SCUART_ENABLE_INT(pSC[0], SC_INTEN_TERRIEN_Msk);
            CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x4);          
        SCUART_DISABLE_INT(pSC[0], SC_INTEN_TERRIEN_Msk);
            CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x0);          
        SCUART_ENABLE_INT(pSC[0], SC_INTEN_TBEIEN_Msk);
            CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x2);          
        SCUART_DISABLE_INT(pSC[0], SC_INTEN_TBEIEN_Msk);
            CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x0);          
        SCUART_ENABLE_INT(pSC[0], SC_INTEN_RDAIEN_Msk);
            CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x1);          
        SCUART_DISABLE_INT(pSC[0], SC_INTEN_RDAIEN_Msk);
            CU_ASSERT_EQUAL(pSC[0]->INTEN, 0x0);   

    /* Check MACRO: SCUART_GET_TX_EMPTY(), SCUART_IS_TX_FULL(), SCUART_IS_TX_EMPTY(), SCUART_GET_INT_FLAG() */
        SC0->PINCTL |= BIT31; /* Enable INTERNAL Loop Back Test */
        SCUART_Open(pSC[0], 100000);
            CU_ASSERT_EQUAL(pSC[0]->STATUS, 0x00000202);   
            CU_ASSERT_EQUAL(SCUART_GET_TX_EMPTY(pSC[0]), SC_STATUS_TXEMPTY_Msk);   
            CU_ASSERT_EQUAL(SCUART_IS_TX_FULL(pSC[0]), 0);   
            CU_ASSERT_EQUAL(SCUART_IS_TX_EMPTY(pSC[0]), 1);   
            CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[0], SC_INTSTS_TERRIF_Msk), 0);   
            CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[0], SC_INTSTS_TBEIF_Msk), 1);   
            
    /* Check MACRO: SCUART_GET_RX_FULL(), SCUART_IS_RX_FULL(), SCUART_IS_RX_READY() */
            CU_ASSERT_NOT_EQUAL(SCUART_GET_RX_FULL(pSC[0]), SC_STATUS_RXFULL_Msk);   
            CU_ASSERT_EQUAL(SCUART_IS_RX_FULL(pSC[0]), 0);   
            CU_ASSERT_EQUAL(SCUART_IS_RX_READY(pSC[0]), 0);   
    
    /* Check MACRO: S
        CUART_WRITE(), SCUART_WAIT_TX_EMPTY(), SCUART_GET_ERR_FLAG(), SCUART_CLR_INT_FLAG(), SCUART_CLR_ERR_FLAG(), 
    */
        SCUART_WRITE(pSC[0], 0xA1);
        SCUART_WRITE(pSC[0], 0xA2);
        SCUART_WRITE(pSC[0], 0xA3);
        //SCUART_WAIT_TX_EMPTY(pSC[0]);
            //printf(" [3 0x%08x]\n", pSC[0]->STATUS);
            CU_ASSERT_EQUAL(SCUART_GET_TX_EMPTY(pSC[0]), 0);   
            //printf(" [3 0x%08x]\n", pSC[0]->STATUS);
        pSC[0]->ALTCTL |= (SC_ALTCTL_RXRST_Msk | SC_ALTCTL_TXRST_Msk); CLK_SysTickDelay(100);
            CU_ASSERT_EQUAL(pSC[0]->STATUS, 0x00000202);           
        SCUART_WRITE(pSC[0], 0xB1);
        SCUART_WRITE(pSC[0], 0xB2);
        SCUART_WRITE(pSC[0], 0xB3);
        SCUART_WRITE(pSC[0], 0xB4);
        SCUART_WRITE(pSC[0], 0xB5);
        SCUART_WAIT_TX_EMPTY(pSC[0]);
            //printf(" [5 0x%08x]\n", pSC[0]->STATUS);
            CU_ASSERT_EQUAL(pSC[0]->STATUS, 0x04000502);   
            CU_ASSERT_EQUAL(SCUART_IS_TX_FULL(pSC[0]), 1); 
            CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[0], SC_INTSTS_TERRIF_Msk), 1);   
            CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[0], SC_INTSTS_TBEIF_Msk), 0);  
            CU_ASSERT_EQUAL(SCUART_GET_ERR_FLAG(pSC[0]), 0);              
        SCUART_CLR_INT_FLAG(pSC[0], SC_INTSTS_TERRIF_Msk); /* no effect */
            CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[0], SC_INTSTS_TERRIF_Msk), 1);   
        SCUART_CLR_ERR_FLAG(pSC[0], SC_STATUS_TXOV_Msk);
                    printf("==>%x  %x\n", pSC[0]->INTSTS, pSC[0]->STATUS);
            CU_ASSERT_EQUAL(SCUART_GET_INT_FLAG(pSC[0], SC_INTSTS_TERRIF_Msk), 0);                     
         CU_ASSERT_EQUAL(pSC[0]->STATUS & SC_STATUS_TXOV_Msk, 0);
            printf("==>%x  %x\n", pSC[0]->INTSTS, pSC[0]->STATUS);
        while(SCUART_IS_RX_READY(pSC[0]) == 0) {}; CLK_SysTickDelay(10000);
        i = 0;
        while(SCUART_GET_RX_EMPTY(pSC[0]) == 0) {
            u8RDA[i++] = SCUART_READ(pSC[0]);
        }
        printf("==>%x  %x\n", pSC[0]->INTSTS, pSC[0]->STATUS);
            CU_ASSERT_EQUAL(pSC[0]->STATUS, 0x00000202);           
            CU_ASSERT_EQUAL(pSC[0]->INTSTS, 0x00000002);      

    printf("==>%x  %x\n", pSC[0]->INTSTS, pSC[0]->STATUS);        
            CU_ASSERT_EQUAL(u8RDA[0], 0xB1);           
            CU_ASSERT_EQUAL(u8RDA[1], 0xB2);           
            CU_ASSERT_EQUAL(u8RDA[2], 0xB3);           
            CU_ASSERT_EQUAL(u8RDA[3], 0xB4);           

SC0->PINCTL &= ~BIT31; /* Disable INTERNAL Loop Back Test */
}

uint8_t au8TxBuf[] = "ABC";
void API_SCUART_Test(void)
{        
    uint32_t u32Baud[3];
    uint8_t au8RxBuf[3];
    
    /* Reset all SC */
        SYS->IPRST2 = SYS_IPRST2_SC0RST_Msk | SYS_IPRST2_SC1RST_Msk;
        SYS->IPRST2 = 0x0;

    if(SCUART_InitClock() != 0) {
        CU_FAIL("SCUART Init Clock FAIL");  
        return ;
    }
        
    /* Check SCUART_SetTimeoutCnt() */
    SCUART_SetTimeoutCnt(pSC[0], 0x5A);
        CU_ASSERT_EQUAL(pSC[0]->RXTOUT, 0x5A); 
    SCUART_SetTimeoutCnt(pSC[0], 0x1FF);
        CU_ASSERT_EQUAL(pSC[0]->RXTOUT, 0x1FF); 
    SCUART_SetTimeoutCnt(pSC[0], 0x0);
        CU_ASSERT_EQUAL(pSC[0]->RXTOUT, 0x0); 
    
    /* Check SCUART_Open() */
        u32Baud[0] = SCUART_Open(pSC[0], 115200);
        u32Baud[1] = SCUART_Open(pSC[1], 115200);
            CU_ASSERT_EQUAL(u32Baud[0], u32Baud[1]);          
            if( (u32Baud[0] > (115200+2000)) || (u32Baud[0] < (115200-2000)) ) {
                CU_FAIL("Baudrate calculate FAIL");  
            }
            CU_ASSERT_EQUAL(pSC[0]->CTL,    0x8001);          
            CU_ASSERT_EQUAL(pSC[0]->UARTCTL,  0x41);          
            CU_ASSERT_EQUAL(pSC[0]->ETUCTL,  (__HXT/115200)-1); 

    /* Check SCUART_Write(), SCUART_Read(), SCUART_GET_TX_FULL() and SCUART_GET_RX_EMPTY() */
  SC0->PINCTL |= BIT31; /* Enable INTERNAL Loop Back Test */
        SCUART_Write(pSC[0], au8TxBuf, sizeof(au8TxBuf)); /* Include SCUART_GET_TX_FULL() */
        while((pSC[0]->STATUS&(SC_STATUS_RXPOINT_Msk|0x7)) != 0x00030000) {};
            CU_ASSERT_EQUAL(SCUART_GET_TX_FULL(pSC[0]), 0); 
        SCUART_Read(pSC[0], au8RxBuf, sizeof(au8RxBuf)); /* Include SCUART_GET_RX_EMPTY() */
            CU_ASSERT_EQUAL(au8RxBuf[0], 'A'); 
            CU_ASSERT_EQUAL(au8RxBuf[1], 'B'); 
            CU_ASSERT_EQUAL(au8RxBuf[2], 'C'); 
        while((pSC[0]->STATUS&(SC_STATUS_TXACT_Msk|SC_STATUS_RXACT_Msk)) != 0x0);
        while(SCUART_GET_RX_EMPTY(pSC[0]) == 0) {
            SCUART_READ(pSC[0]);         
        }
                       
        SCUART_WRITE(pSC[0], 0xA5);
        printf("b=0x%08x\n", pSC[0]->STATUS);
        
        SC0->PINCTL &= ~BIT31; /* Disable INTERNAL Loop Back Test */
}

CU_TestInfo SCUART_MACRO_CONSTANT[] =
{
    {"Check SCUART MACRO and CONSTANT ", MACRO_CONSTANT_Test}, 
    CU_TEST_INFO_NULL
};

CU_TestInfo SCUART_API[] =
{
    {"Check SCUART API ", API_SCUART_Test}, 
    CU_TEST_INFO_NULL
};
