/*---------------------------------------------------------------------------------------------------------*/
/* Nuvoton Technology Corporation confidential                                                             */
/*                                                                                                         */
/* Copyright (c) 2015 by Nuvoton Technology Corporation                                                    */
/* All rights reserved                                                                                     */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/
/* File Contents:                                                                                          */
/*   pdma_cunit.c                                                                                           */
/*            The test function of PDMA for CUnit.                                                          */
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
#include "pdma_cunit.h"


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
#define PDMA_MAX_CH 5

#define PDMA_CH1 1
#define PDMA_CH2 2
#define PDMA_CH3 3
#define PDMA_CH4 4

uint32_t TenBitsPatternTable[4] = {0x3FF, 0x25A, 0x1A5, 0x000}; /* 10 bits test pattern */
uint32_t SixteenBitsPatternTable[4] = {0xFFFF, 0x5A5A, 0xA5A5, 0x0000}; /* 16 bits test pattern */
uint32_t ThirtyTwoBitsPatternTable[4] = {0xFFFFFFFF, 0x5A5A5A5A, 0xA5A5A5A5, 0x00000000}; /* 32 bits test pattern */

/*---------------------------------------------------------------------------------------------------------*/
/*  Constant Definitions                                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
uint8_t SrcArray[256];
uint8_t DestArray[256];
uint32_t PDMA_TEST_LENGTH = 64;
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
    {"PDMA MACRO", suite_success_init, suite_success_clean, NULL, NULL, PDMA_MACRO},
    {"PDMA API", suite_success_init, suite_success_clean, NULL, NULL, PDMA_API},
    CU_SUITE_INFO_NULL
};


void Func_PDMA_Open() 
{
    uint32_t u32TestCh = 0;
    
    //test case 1: enable/disable one channel each time, then check each action
    for (u32TestCh = 1; u32TestCh < PDMA_MAX_CH; u32TestCh++)
    {
        PDMA_Open(1 << u32TestCh);
        CU_ASSERT_EQUAL(PDMAGCR->GCTL & (DMA_GCR_GCTL_CKEN1_Msk << (u32TestCh-1)), 1 << (8 + u32TestCh));
      
        PDMA_Close();
        CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0);
    }
    
    //test case 2: enable/disable one channel each time, then check at final action
    for (u32TestCh = 1; u32TestCh < PDMA_MAX_CH; u32TestCh++)
    {
        PDMA_Open(1 << u32TestCh);
    }
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0x00001E00);

    PDMA_Close();
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0);

    //test case 3: enable/disable multiple channels each time
    PDMA_Open(1 << PDMA_CH1);
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0x00000200);
    
    PDMA_Close();
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0);
    
    PDMA_Open((1 << PDMA_CH1) | (1 << PDMA_CH2));
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0x00000600);
    
    PDMA_Close();
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0);
    
    PDMA_Open((1 << PDMA_CH1) | (1 << PDMA_CH2) | (1 << PDMA_CH3));
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0x00000E00);
    
    PDMA_Close();
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0);
    
    PDMA_Open((1 << PDMA_CH1) | (1 << PDMA_CH2) | (1 << PDMA_CH3) | (1 << PDMA_CH4));
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0x00001E00);
    
    PDMA_Close();
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0);

}

void Func_PDMA_SetTransferCnt() 
{
    uint32_t u32TestCh = 0;
    uint32_t u32TransferCountPatternIdx = 0, u32TransferCountPatternCount = 0, u32TransferCountPatternData = 0;
    PDMA_CH_T *pdma;
    
    u32TransferCountPatternCount = sizeof(SixteenBitsPatternTable)/sizeof(uint32_t);
    
    for (u32TestCh = 1; u32TestCh < PDMA_MAX_CH; u32TestCh++)
    {
        for (u32TransferCountPatternIdx = 0; u32TransferCountPatternIdx < u32TransferCountPatternCount; u32TransferCountPatternIdx++)
        {
            pdma = (PDMA_CH_T *)((uint32_t) PDMA1_BASE + (0x100 * (u32TestCh-1)));
            u32TransferCountPatternData = SixteenBitsPatternTable[u32TransferCountPatternIdx];

            PDMA_SetTransferCnt(u32TestCh, PDMA_WIDTH_8, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TXWIDTH_Msk, 1 << 19);
            CU_ASSERT_EQUAL(pdma->CNTn, u32TransferCountPatternData);

            PDMA_SetTransferCnt(u32TestCh, PDMA_WIDTH_16, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TXWIDTH_Msk, 2 << 19);
            CU_ASSERT_EQUAL(pdma->CNTn, u32TransferCountPatternData & 0xFFFF);

            PDMA_SetTransferCnt(u32TestCh, PDMA_WIDTH_32, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TXWIDTH_Msk, 0 << 19);
            CU_ASSERT_EQUAL(pdma->CNTn, u32TransferCountPatternData & 0xFFFF);
        }
    }
}

void Func_PDMA_SetTimeOut()
{
    uint32_t u32TestCh = 0;
    PDMA_CH_T *pdma;

    for (u32TestCh = 1; u32TestCh < PDMA_MAX_CH; u32TestCh++)
    {
        pdma = (PDMA_CH_T *)((uint32_t) PDMA1_BASE + (0x100 * (u32TestCh-1)));
        PDMA_SetTimeOut(u32TestCh, 1, 0x55aa);
        CU_ASSERT_EQUAL(pdma->TOCn & PDMA_CH_TOCn_TOC_Msk, 0x55aa);
        CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TOUTEN_Msk, 0x1000);

        PDMA_SetTimeOut(u32TestCh, 0, 0);
        CU_ASSERT_EQUAL(pdma->TOCn & PDMA_CH_TOCn_TOC_Msk, 0);
        CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TOUTEN_Msk, 0);
    }
}

void Func_PDMA_SetTransferAddr() 
{
    uint32_t u32TestCh = 0;
    uint32_t u32TransferAddressPatternIdx = 0, u32TransferAddressPatternCount = 0, u32TransferAddressPatternData = 0;
    PDMA_CH_T *pdma;
    
    u32TransferAddressPatternCount = sizeof(ThirtyTwoBitsPatternTable)/sizeof(uint32_t);
    
    for (u32TestCh = 1; u32TestCh < PDMA_MAX_CH; u32TestCh++)
    {
        for (u32TransferAddressPatternIdx = 0; u32TransferAddressPatternIdx < u32TransferAddressPatternCount; u32TransferAddressPatternIdx++)
        {
            pdma = (PDMA_CH_T *)((uint32_t) PDMA1_BASE + (0x100 * (u32TestCh-1)));
            u32TransferAddressPatternData = ThirtyTwoBitsPatternTable[u32TransferAddressPatternIdx];

            PDMA_Open(1 << u32TestCh);
            CU_ASSERT_EQUAL(PDMAGCR->GCTL & (DMA_GCR_GCTL_CKEN1_Msk << (u32TestCh-1)), 1 << (8 + u32TestCh));
            
            PDMA_SetTransferAddr(u32TestCh, u32TransferAddressPatternData, PDMA_SAR_FIX, u32TransferAddressPatternData, PDMA_DAR_FIX);
            CU_ASSERT_EQUAL(pdma->SAn, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->DAn, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_SASEL_Msk, 2 << 4);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_DASEL_Msk, 2 << 6);
            
            PDMA_SetTransferAddr(u32TestCh, u32TransferAddressPatternData, PDMA_SAR_INC, u32TransferAddressPatternData, PDMA_DAR_INC);
            CU_ASSERT_EQUAL(pdma->SAn, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->DAn, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_SASEL_Msk, 0 << 4);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_DASEL_Msk, 0 << 6);
            
            PDMA_SetTransferAddr(u32TestCh, u32TransferAddressPatternData, PDMA_SAR_WRA, u32TransferAddressPatternData, PDMA_DAR_WRA);
            CU_ASSERT_EQUAL(pdma->SAn, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->DAn, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_SASEL_Msk, 3 << 4);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_DASEL_Msk, 3 << 6);
        }
    }
    PDMA_Close();
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0);
}

void Func_PDMA_SetTransferMode() 
{
    uint32_t u32TestCh = 0;

    /* CH1 ~ CH3 */
    for (u32TestCh = 1; u32TestCh <= 3; u32TestCh++)
    {
        /* PDMA_SPI0_TX */
        PDMA_SetTransferMode(u32TestCh, PDMA_SPI0_TX, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_SPI0_TX << (u32TestCh * 8));

        /* PDMA_SPI1_TX */
        PDMA_SetTransferMode(u32TestCh, PDMA_SPI1_TX, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_SPI1_TX << (u32TestCh * 8));

        /* PDMA_UART0_TX */
        PDMA_SetTransferMode(u32TestCh, PDMA_UART0_TX, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_UART0_TX << (u32TestCh * 8));

        /* PDMA_UART1_TX */
        PDMA_SetTransferMode(u32TestCh, PDMA_UART1_TX, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_UART1_TX << (u32TestCh * 8));

        /* PDMA_TMR0 */
        PDMA_SetTransferMode(u32TestCh, PDMA_TMR0, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_TMR0 << (u32TestCh * 8));

        /* PDMA_TMR1 */
        PDMA_SetTransferMode(u32TestCh, PDMA_TMR1, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_TMR1 << (u32TestCh * 8));

        /* PDMA_TMR2 */
        PDMA_SetTransferMode(u32TestCh, PDMA_TMR2, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_TMR2 << (u32TestCh * 8));

        /* PDMA_TMR3 */
        PDMA_SetTransferMode(u32TestCh, PDMA_TMR3, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_TMR3 << (u32TestCh * 8));

        /* PDMA_SPI0_RX */
        PDMA_SetTransferMode(u32TestCh, PDMA_SPI0_RX, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_SPI0_RX << (u32TestCh * 8));

        /* PDMA_SPI1_RX */
        PDMA_SetTransferMode(u32TestCh, PDMA_SPI1_RX, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_SPI1_RX << (u32TestCh * 8));

        /* PDMA_UART0_RX */
        PDMA_SetTransferMode(u32TestCh, PDMA_UART0_RX, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_UART0_RX << (u32TestCh * 8));

        /* PDMA_UART1_RX */
        PDMA_SetTransferMode(u32TestCh, PDMA_UART1_RX, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_UART1_RX << (u32TestCh * 8));

        /* PDMA_ADC */
        PDMA_SetTransferMode(u32TestCh, PDMA_ADC, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_ADC << (u32TestCh * 8));

        /* PDMA_MEM */
        PDMA_SetTransferMode(u32TestCh, PDMA_MEM, NULL, NULL);
        CU_ASSERT_EQUAL(PDMAGCR->REQSEL0 & (0x1f << (u32TestCh * 8)), PDMA_MEM << (u32TestCh * 8));
    }

    /* CH4 */
    /* PDMA_SPI0_TX */
    PDMA_SetTransferMode(u32TestCh, PDMA_SPI0_TX, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_SPI0_TX);

    /* PDMA_SPI1_TX */
    PDMA_SetTransferMode(u32TestCh, PDMA_SPI1_TX, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_SPI1_TX);

    /* PDMA_UART0_TX */
    PDMA_SetTransferMode(u32TestCh, PDMA_UART0_TX, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_UART0_TX);

    /* PDMA_UART1_TX */
    PDMA_SetTransferMode(u32TestCh, PDMA_UART1_TX, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_UART1_TX);

    /* PDMA_TMR0 */
    PDMA_SetTransferMode(u32TestCh, PDMA_TMR0, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_TMR0);

    /* PDMA_TMR1 */
    PDMA_SetTransferMode(u32TestCh, PDMA_TMR1, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_TMR1);

    /* PDMA_TMR2 */
    PDMA_SetTransferMode(u32TestCh, PDMA_TMR2, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_TMR2);

    /* PDMA_TMR3 */
    PDMA_SetTransferMode(u32TestCh, PDMA_TMR3, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_TMR3);

    /* PDMA_SPI0_RX */
    PDMA_SetTransferMode(u32TestCh, PDMA_SPI0_RX, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_SPI0_RX);

    /* PDMA_SPI1_RX */
    PDMA_SetTransferMode(u32TestCh, PDMA_SPI1_RX, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_SPI1_RX);

    /* PDMA_UART0_RX */
    PDMA_SetTransferMode(u32TestCh, PDMA_UART0_RX, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_UART0_RX);

    /* PDMA_UART1_RX */
    PDMA_SetTransferMode(u32TestCh, PDMA_UART1_RX, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_UART1_RX);

    /* PDMA_ADC */
    PDMA_SetTransferMode(u32TestCh, PDMA_ADC, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_ADC);

    /* PDMA_MEM */
    PDMA_SetTransferMode(u32TestCh, PDMA_MEM, NULL, NULL);
    CU_ASSERT_EQUAL(PDMAGCR->REQSEL1 & DMA_GCR_REQSEL1_REQSRC4_Msk, PDMA_MEM);
}

void Func_PDMA_Trigger() 
{
    uint32_t u32TestCh = 0;
    PDMA_CH_T *pdma;
    
    for (u32TestCh = 1; u32TestCh < PDMA_MAX_CH; u32TestCh++)
    {
        pdma = (PDMA_CH_T *)((uint32_t) PDMA1_BASE + (0x100 * (u32TestCh-1)));
        
        PDMA_Open(1 << u32TestCh);
        CU_ASSERT_EQUAL(PDMAGCR->GCTL & (DMA_GCR_GCTL_CKEN1_Msk << (u32TestCh-1)), 1 << (8 + u32TestCh));

        PDMA_Trigger(u32TestCh);
        //CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TRIGEN_Msk, 1 << 23); //hardware will auto clear this bit
        CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_CHEN_Msk, 1);
        
        PDMA_Close();
        CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0);
    }
}

void Func_PDMA_EnableInt() 
{
    uint32_t u32TestCh = 0;
    PDMA_CH_T *pdma;
    
    for (u32TestCh = 1; u32TestCh < PDMA_MAX_CH; u32TestCh++)
    {
        pdma = (PDMA_CH_T *)((uint32_t) PDMA1_BASE + (0x100 * (u32TestCh-1)));
        
        PDMA_EnableInt(u32TestCh, PDMA_CH_INTENn_TABTIEN_Msk);
        CU_ASSERT_EQUAL(pdma->INTENn & PDMA_CH_INTENn_TABTIEN_Msk, 1);

        PDMA_EnableInt(u32TestCh, PDMA_CH_INTENn_TDIEN_Msk);
        CU_ASSERT_EQUAL(pdma->INTENn & PDMA_CH_INTENn_TDIEN_Msk, 1 << 1);

        PDMA_DisableInt(u32TestCh, PDMA_CH_INTENn_TABTIEN_Msk);
        CU_ASSERT_EQUAL(pdma->INTENn & PDMA_CH_INTENn_TABTIEN_Msk, 0);

        PDMA_DisableInt(u32TestCh, PDMA_CH_INTENn_TDIEN_Msk);
        CU_ASSERT_EQUAL(pdma->INTENn & PDMA_CH_INTENn_TDIEN_Msk, 0 << 1);
    }
}

void Func_PDMA_GET_INT_STATUS() 
{
    uint32_t u32TestCh = 0;
    PDMA_CH_T *pdma;
    
    for (u32TestCh = 1; u32TestCh < PDMA_MAX_CH; u32TestCh++)
    {
        pdma = (PDMA_CH_T *)((uint32_t) PDMA1_BASE + (0x100 * (u32TestCh-1)));
        
        PDMA_Open(1 << u32TestCh);
        CU_ASSERT_EQUAL(PDMAGCR->GCTL & (DMA_GCR_GCTL_CKEN1_Msk << (u32TestCh-1)), 1 << (8 + u32TestCh));

        //abort interrupt flag test
        PDMA_SetTransferCnt(u32TestCh, PDMA_WIDTH_32, PDMA_TEST_LENGTH);
        PDMA_SetTransferAddr(u32TestCh, (uint32_t)SrcArray, PDMA_SAR_INC, (uint32_t)0, PDMA_DAR_INC);
        
        PDMA_Trigger(u32TestCh);
        CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_CHEN_Msk, 1);

        CU_ASSERT_EQUAL(PDMA_GET_CH_INT_STS(u32TestCh) & PDMA_CH_INTSTSn_TABTIF_Msk, 1);

        CU_ASSERT_EQUAL(PDMA_GET_INT_STATUS() & (DMA_GCR_GINTSTS_IF1_Msk << (u32TestCh-1)), 1 << u32TestCh);

        PDMA_CLR_CH_INT_FLAG(u32TestCh, PDMA_CH_INTSTSn_TABTIF_Msk);
        CU_ASSERT_EQUAL(pdma->INTSTSn & PDMA_CH_INTSTSn_TABTIF_Msk, 0);


        /* Reset PDMA */
        SYS->IPRST1 |=  SYS_IPRST1_PDMARST_Msk;
        SYS->IPRST1 &= ~SYS_IPRST1_PDMARST_Msk;

        PDMA_Open(1 << u32TestCh);
        CU_ASSERT_EQUAL(PDMAGCR->GCTL & (DMA_GCR_GCTL_CKEN1_Msk << (u32TestCh-1)), 1 << (8 + u32TestCh));

        //block transfer done interrupt flag test
        PDMA_SetTransferCnt(u32TestCh, PDMA_WIDTH_32, PDMA_TEST_LENGTH);
        PDMA_SetTransferAddr(u32TestCh, (uint32_t)SrcArray, PDMA_SAR_INC, (uint32_t)DestArray, PDMA_DAR_INC);

        PDMA_EnableInt(u32TestCh, PDMA_CH_INTENn_TDIEN_Msk);
        CU_ASSERT_EQUAL(pdma->INTENn & PDMA_CH_INTENn_TDIEN_Msk, 1 << 1);

        PDMA_Trigger(u32TestCh);
        CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_CHEN_Msk, 1);
        CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TRIGEN_Msk, 1 << 23);
        while( PDMA_IS_CH_BUSY(u32TestCh) == 1 );
        CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TRIGEN_Msk, 0 << 23);

        CU_ASSERT_EQUAL(PDMA_GET_CH_INT_STS(u32TestCh) & PDMA_CH_INTSTSn_TDIF_Msk, 1 << 1);

        CU_ASSERT_EQUAL(PDMA_GET_INT_STATUS() & (DMA_GCR_GINTSTS_IF1_Msk << (u32TestCh-1)), 1 << u32TestCh);

        PDMA_CLR_CH_INT_FLAG(u32TestCh, PDMA_CH_INTSTSn_TDIF_Msk);
        CU_ASSERT_EQUAL(pdma->INTSTSn & PDMA_CH_INTSTSn_TDIF_Msk, 0 << 1);

        PDMA_DisableInt(u32TestCh, PDMA_CH_INTENn_TDIEN_Msk);
        CU_ASSERT_EQUAL(pdma->INTENn & PDMA_CH_INTENn_TDIEN_Msk, 0 << 1);
    }
    
    PDMA_Close();
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0);
}

void Func_PDMA_SET_SRC_ADDR() 
{
    uint32_t u32TestCh = 0;
    uint32_t u32TransferAddressPatternIdx = 0, u32TransferAddressPatternCount = 0, u32TransferAddressPatternData = 0;
    PDMA_CH_T *pdma;
    
    u32TransferAddressPatternCount = sizeof(ThirtyTwoBitsPatternTable)/sizeof(uint32_t);
    
    for (u32TestCh = 1; u32TestCh < PDMA_MAX_CH; u32TestCh++)
    {
        for (u32TransferAddressPatternIdx = 0; u32TransferAddressPatternIdx < u32TransferAddressPatternCount; u32TransferAddressPatternIdx++)
        {
            pdma = (PDMA_CH_T *)((uint32_t) PDMA1_BASE + (0x100 * (u32TestCh-1)));
            u32TransferAddressPatternData = ThirtyTwoBitsPatternTable[u32TransferAddressPatternIdx];

            PDMA_Open(1 << u32TestCh);
            CU_ASSERT_EQUAL(PDMAGCR->GCTL & (DMA_GCR_GCTL_CKEN1_Msk << (u32TestCh-1)), 1 << (8 + u32TestCh));
            
            PDMA_SET_SRC_ADDR(u32TestCh, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->SAn, u32TransferAddressPatternData);

            PDMA_SET_DST_ADDR(u32TestCh, u32TransferAddressPatternData);
            CU_ASSERT_EQUAL(pdma->DAn, u32TransferAddressPatternData);
        }
    }
    PDMA_Close();
    CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0);
}

void Func_PDMA_SET_TRANS_CNT() 
{
    uint32_t u32TestCh = 0;
    uint32_t u32TransferCountPatternIdx = 0, u32TransferCountPatternCount = 0, u32TransferCountPatternData = 0;
    PDMA_CH_T *pdma;
    
    u32TransferCountPatternCount = sizeof(SixteenBitsPatternTable)/sizeof(uint32_t);
    
    for (u32TestCh = 1; u32TestCh < PDMA_MAX_CH; u32TestCh++)
    {
        for (u32TransferCountPatternIdx = 0; u32TransferCountPatternIdx < u32TransferCountPatternCount; u32TransferCountPatternIdx++)
        {
            pdma = (PDMA_CH_T *)((uint32_t) PDMA1_BASE + (0x100 * (u32TestCh-1)));
            u32TransferCountPatternData = SixteenBitsPatternTable[u32TransferCountPatternIdx];

            pdma->CTLn = (pdma->CTLn & ~PDMA_CH_CTLn_TXWIDTH_Msk) | PDMA_WIDTH_8;
            PDMA_SET_TRANS_CNT(u32TestCh, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TXWIDTH_Msk, 1 << 19);
            CU_ASSERT_EQUAL(pdma->CNTn, u32TransferCountPatternData);

            pdma->CTLn = (pdma->CTLn & ~PDMA_CH_CTLn_TXWIDTH_Msk) | PDMA_WIDTH_16;
            PDMA_SET_TRANS_CNT(u32TestCh, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TXWIDTH_Msk, 2 << 19);
            CU_ASSERT_EQUAL(pdma->CNTn, u32TransferCountPatternData & 0xFFFF);

            pdma->CTLn = (pdma->CTLn & ~PDMA_CH_CTLn_TXWIDTH_Msk) | PDMA_WIDTH_32;
            PDMA_SET_TRANS_CNT(u32TestCh, u32TransferCountPatternData);
            CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TXWIDTH_Msk, 0 << 19);
            CU_ASSERT_EQUAL(pdma->CNTn, u32TransferCountPatternData & 0xFFFF);
        }
    }
}

void Func_PDMA_STOP() 
{
    uint32_t u32TestCh = 0;
    PDMA_CH_T *pdma;
    
    for (u32TestCh = 1; u32TestCh < PDMA_MAX_CH; u32TestCh++)
    {
        pdma = (PDMA_CH_T *)((uint32_t) PDMA1_BASE + (0x100 * (u32TestCh-1)));
        
        PDMA_Open(1 << u32TestCh);
        CU_ASSERT_EQUAL(PDMAGCR->GCTL & (DMA_GCR_GCTL_CKEN1_Msk << (u32TestCh-1)), 1 << (8 + u32TestCh));

        PDMA_Trigger(u32TestCh);
        //CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_TRIGEN_Msk, 1 << 23); //hardware will auto clear this bit
        CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_CHEN_Msk, 1);
        
        PDMA_STOP(u32TestCh);
        CU_ASSERT_EQUAL(pdma->CTLn & PDMA_CH_CTLn_CHEN_Msk, 0);
        
        PDMA_Close();
        CU_ASSERT_EQUAL(PDMAGCR->GCTL, 0);
    }
}

CU_TestInfo PDMA_MACRO[] =
{
    {"Test PDMA_GET_INT_STATUS()/PDMA_GET_CH_INT_STS()/PDMA_CLR_CH_INT_FLAG()/PDMA_IS_CH_BUSY():", Func_PDMA_GET_INT_STATUS},
    {"Test PDMA_SET_SRC_ADDR()/PDMA_SET_DST_ADDR():", Func_PDMA_SET_SRC_ADDR},
    {"Test PDMA_SET_TRANS_CNT():", Func_PDMA_SET_TRANS_CNT},
    {"Test PDMA_STOP():", Func_PDMA_STOP},

    CU_TEST_INFO_NULL
};

CU_TestInfo PDMA_API[] =
{
    {"Test PDMA_Open()/PDMA_Close():", Func_PDMA_Open},
    {"Test PDMA_SetTransferCnt():", Func_PDMA_SetTransferCnt},
    {"Test PDMA_SetTransferAddr():", Func_PDMA_SetTransferAddr},
    {"Test PDMA_SetTransferMode():", Func_PDMA_SetTransferMode},
    {"Test PDMA_Trigger():", Func_PDMA_Trigger},
    {"Test PDMA_EnableInt()/PDMA_DisableInt():", Func_PDMA_EnableInt},
    {"Test PDMA_SetTimeOut():", Func_PDMA_SetTimeOut},

    CU_TEST_INFO_NULL
};

