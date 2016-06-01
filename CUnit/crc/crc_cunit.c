/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                             */
/*                                                                                                         */
/* Copyright (c) 2015 by Nuvoton Technology Corporation                                                    */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   crc_cunit.c                                                                                           */
/*            The test function of CRC for CUnit.                                                          */
/* Project:   FA8253                                                                                       */
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
#include "crc_cunit.h"


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
    {"CRC MACRO", suite_success_init, suite_success_clean, NULL, NULL, CRC_MACRO},
    {"CRC API", suite_success_init, suite_success_clean, NULL, NULL, CRC_API},
    CU_SUITE_INFO_NULL
};

int32_t CRC_InitClock(void)
{    
    CLK->AHBCLK |= CLK_AHBCLK_PDMACKEN_Msk;
    
    return 0;
}

const uint32_t au32ModeSel[] = {
    CRC_CCITT,
    CRC_8,
    CRC_16,
    CRC_32,
};
const uint32_t au32AttrSel[] = {
    CRC_WDATA_RVS,
    CRC_CHECKSUM_RVS,
    CRC_WDATA_COM,
    CRC_CHECKSUM_COM,
};
const uint32_t au32DWLSel[] = {
    CRC_CPU_WDATA_8,
    CRC_CPU_WDATA_16,
    CRC_CPU_WDATA_32,
};
const uint32_t au32SEEDTbl[] = {
    0x00000000,
    0x00000001,
    0x55555555,
    0xAAAAAAAA,
    0x80000000,
    0xFFFFFFFF,
};
void CONSTANT_CRC_SETTING(void)
{
    volatile uint32_t i;
    
    if(CRC_InitClock() != 0) {
        CU_FAIL("CRC Clock FAIL");  
        return ;
    }
       
    /* Select CRC Polynomial Mode */
    for(i=0; i<sizeof(au32ModeSel)/4; i++) { 
        D_msg("ModeSel %d\n", i);        
        CRC_Open(au32ModeSel[i], CRC_WDATA_RVS|CRC_CHECKSUM_COM, 0x5A5A5A5A+i, CRC_CPU_WDATA_32);
        CU_ASSERT_EQUAL(PDMACRC->CTL, ((i<<30)|0x29000001)); 
        CU_ASSERT_EQUAL(PDMACRC->SEED, (0x5A5A5A5A+i)); 
    }

    /* Select CRC Attribute */
    for(i=0; i<sizeof(au32AttrSel)/4; i++) { 
        D_msg("AttrSel %d\n", i);        
        CRC_Open(CRC_16, au32AttrSel[i], 0xA5A5A5A5+i, CRC_CPU_WDATA_32);
        CU_ASSERT_EQUAL(PDMACRC->CTL, ((1<<(24+i))|0xA0000001)); 
        CU_ASSERT_EQUAL(PDMACRC->SEED, (0xA5A5A5A5+i)); 
    }
            
    /* Select CRC Write Data Length */
    for(i=0; i<sizeof(au32DWLSel)/4; i++) { 
        D_msg("DWLSel %d\n", i);        
        CRC_Open(CRC_8, CRC_WDATA_RVS|CRC_CHECKSUM_RVS, i, au32DWLSel[i]);
        CU_ASSERT_EQUAL(PDMACRC->CTL, ((i<<28)|0x43000001)); 
        CU_ASSERT_EQUAL(PDMACRC->SEED, i); 
    }
}

void MACRO_SEED(void)
{
    volatile uint32_t i;
    
    if(CRC_InitClock() != 0) {
        CU_FAIL("CRC Clock FAIL");  
        return ;
    }
       
    /* Check CRC_SET_SEED() and CRC_GET_SEED() */
    for(i=0; i<sizeof(au32SEEDTbl)/4; i++) 
    { 
        D_msg("SEEDTbl %d\n", i);        
        CRC_SET_SEED(au32SEEDTbl[i]);        
        CU_ASSERT_EQUAL(PDMACRC->CTL, 0x63000001); 
        CU_ASSERT_EQUAL(CRC_GET_SEED(), au32SEEDTbl[i]); 
    }
}

void MACRO_INTERRUPT(void)
{
    volatile uint32_t i;
    
    if(CRC_InitClock() != 0) {
        CU_FAIL("CRC Clock FAIL");  
        return ;
    }
                   
    /* Check CRC_ENABLE_INT() and CRC_DISABLE_INT() */
        PDMACRC->DMAINTEN = 0x0;
        CRC_ENABLE_INT(DMA_CRC_DMAINTEN_TDIEN_Msk|DMA_CRC_DMAINTEN_TABTIEN_Msk);
        CU_ASSERT_EQUAL(PDMACRC->DMAINTEN, 0x3); 

        PDMACRC->DMAINTEN = 0x0;
        CRC_ENABLE_INT(DMA_CRC_DMAINTEN_TDIEN_Msk);
        CU_ASSERT_EQUAL(PDMACRC->DMAINTEN, 0x2); 

        PDMACRC->DMAINTEN = 0x0;
        CRC_ENABLE_INT(DMA_CRC_DMAINTEN_TABTIEN_Msk);
        CU_ASSERT_EQUAL(PDMACRC->DMAINTEN, 0x1); 

        PDMACRC->DMAINTEN = 0x3;
        CRC_DISABLE_INT(DMA_CRC_DMAINTEN_TDIEN_Msk);
        CU_ASSERT_EQUAL(PDMACRC->DMAINTEN, 0x1); 

        PDMACRC->DMAINTEN = 0x3;
        CRC_DISABLE_INT(DMA_CRC_DMAINTEN_TABTIEN_Msk);
        CU_ASSERT_EQUAL(PDMACRC->DMAINTEN, 0x2); 

        PDMACRC->DMAINTEN = 0x3;
        CRC_DISABLE_INT(DMA_CRC_DMAINTEN_TDIEN_Msk|DMA_CRC_DMAINTEN_TABTIEN_Msk);
        CU_ASSERT_EQUAL(PDMACRC->DMAINTEN, 0x0); 
}

void MACRO_WRITE_DATA(void)
{
    volatile uint32_t i;
    
    if(CRC_InitClock() != 0) {
        CU_FAIL("CRC Clock FAIL");  
        return ;
    }
                   
    /* Check CRC_WRITE_DATA() and CRC_GetChecksum() */
        /* CRC-8, DL-8, Seed 0xFFFFFFFF, WD 0x55555555 --> 0x5F5F5F5F */
        CRC_Open(CRC_8, 0x0, 0xFFFFFFFF, CRC_CPU_WDATA_8);
        CRC_WRITE_DATA(0x55555555);
        CU_ASSERT_EQUAL(CRC_GetChecksum(), 0x5F); 
        CU_ASSERT_EQUAL(PDMACRC->DAT, 0x55555555); 
        
        /* CRC-16, DL-8, Seed 0xFFFFFFFF, WD 0xAAAAAAAA --> 0xFEFEFEFE */
        CRC_Open(CRC_16, 0x0, 0xFFFFFFFF, CRC_CPU_WDATA_8);
        CRC_WRITE_DATA(0xAAAAAAAA);
        CU_ASSERT_EQUAL(CRC_GetChecksum(), 0xFEFE); 
        CU_ASSERT_EQUAL(PDMACRC->DAT, 0xAAAAAAAA); 
        
        /* CRC-CCITT, DL-8, Seed 0xFFFFFFFF, WD 0x12345678 --> 0x1E6F1E6F */
        CRC_Open(CRC_CCITT, 0x0, 0xFFFFFFFF, CRC_CPU_WDATA_8);
        CRC_WRITE_DATA(0x12345678);
        CU_ASSERT_EQUAL(CRC_GetChecksum(), 0x1E6F); 
        CU_ASSERT_EQUAL(PDMACRC->DAT, 0x12345678); 
        
        /* CRC-32, DL-8, Seed 0xFFFFFFFF, WD 0x5A5A5A5A --> 0x1915C265 */
        CRC_Open(CRC_32, 0x0, 0xFFFFFFFF, CRC_CPU_WDATA_8);
        CRC_WRITE_DATA(0x5A5A5A5A);
        CU_ASSERT_EQUAL(CRC_GetChecksum(), 0x1915C265); 
        CU_ASSERT_EQUAL(PDMACRC->DAT, 0x5A5A5A5A); 
}

volatile uint8_t g_u8IsTargetAbortINTFlag = 0, g_u8IsBlockTransferDoneINTFlag = 0;
void PDMA_IRQHandler(void)
{
    volatile uint32_t u32IntStatus = CRC_GET_INT_FLAG();

    /* Check CRC interrupt status */
    if(u32IntStatus & DMA_CRC_DMAISTS_TDIF_Msk) {
        /* Clear Block Transfer Done interrupt flag */
        CRC_CLR_INT_FLAG(DMA_CRC_DMAISTS_TDIF_Msk);

        g_u8IsBlockTransferDoneINTFlag++;
    } else if(u32IntStatus & DMA_CRC_DMAISTS_TABTIF_Msk) {
        /* Clear Target Abort interrupt flag */
        CRC_CLR_INT_FLAG(DMA_CRC_DMAISTS_TABTIF_Msk);

        g_u8IsTargetAbortINTFlag++;
    }
}

void API_CRC_OpenAndDMA(void) 
{
    volatile uint32_t i, u32Timeout = 0;
    const uint8_t acCRCSrcPattern[] = "123456789";
    uint32_t u32TargetChecksum = 0x29B1, u32CalChecksum = 0;
    
    if(CRC_InitClock() != 0) {
        CU_FAIL("CRC Clock FAIL");  
        return ;
    }
       
    /* Clear all CRC interrupt source flag status to 0 */
    g_u8IsTargetAbortINTFlag = g_u8IsBlockTransferDoneINTFlag = 0;

    /* Enable CRC NVIC (the same channel as PDMA) */
    NVIC_EnableIRQ(PDMA_IRQn);

    /* Configure CRC operation settings for CRC-CCITT DMA mode */
    CRC_Open(CRC_CCITT, 0, 0xFFFF, 0);

    /* Enable DMA Target Abort and Block Transfer Done interrupt function */
    CRC_ENABLE_INT(DMA_CRC_DMAINTEN_TABTIEN_Msk | DMA_CRC_DMAINTEN_TDIEN_Msk);

    /* Trigger CRC DMA transfer */
    CRC_StartDMATransfer((uint32_t)acCRCSrcPattern, strlen((char *)acCRCSrcPattern));

    /* Wait CRC interrupt flag occurred */
    while(1){
        if((g_u8IsTargetAbortINTFlag == 1) || (g_u8IsBlockTransferDoneINTFlag == 1) || (u32Timeout > SystemCoreClock/1000))
            break;
            
        u32Timeout++;        
    }

    /* Disable CRC NVIC */
    NVIC_DisableIRQ(PDMA_IRQn);

    /* Get CRC-CCITT checksum value */
    u32CalChecksum = CRC_GetChecksum();
    if(g_u8IsBlockTransferDoneINTFlag != 1) {
        CU_FAIL("CRC DMA FAIL");  
        goto exit_test;
    } else {
        if((u32CalChecksum == u32TargetChecksum) && (CRC_GET_INT_FLAG() == 0x0)) {
            ;
        } else {
            CU_FAIL("CRC Checksum FAIL");  
            goto exit_test;
        }        
    }
        
exit_test:  
    /* Disable CRC function */
    PDMACRC->CTL &= ~DMA_CRC_CTL_CRCEN_Msk;    
}

CU_TestInfo CRC_MACRO[] =
{
    {"Check CRC CONSTANT ",                       CONSTANT_CRC_SETTING}, 
    {"Check CRC_SET_SEED and CRC_GET_SEED ",      MACRO_SEED}, 
    {"Check CRC_ENABLE_INT and CRC_DISABLE_INT ", MACRO_INTERRUPT}, 
    {"Check CRC_WRITE_DATA ",                     MACRO_WRITE_DATA}, 
    CU_TEST_INFO_NULL
};

CU_TestInfo CRC_API[] =
{
    {"Check CRC_Open, CRC_StartDMATransfer and CRC_GetChecksum API ", API_CRC_OpenAndDMA}, 
    CU_TEST_INFO_NULL
};
