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
#include "dsrc_cunit.h"
#include "dsrc.h"


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


void DSRC_Reset(void){
  SYS->IPRST2 |= (1<<7);
  SYS->IPRST2 &= ~(1<<7);
}

const uint32_t OpenSel[] = {
DSRC_MODE_FM0,1,1,
DSRC_MODE_FM0,0,1,
DSRC_MODE_FM0,0,0,
DSRC_MODE_FM0,1,0,
DSRC_MODE_MONCHESTER,1,1,
DSRC_MODE_MONCHESTER,0,1,
DSRC_MODE_MONCHESTER,0,0,
DSRC_MODE_MONCHESTER,1,0,
DSRC_MODE_SPI,1,1,
DSRC_MODE_SPI,0,1,
DSRC_MODE_SPI,0,0,
DSRC_MODE_SPI,1,0,
};

void API_DSRC_Open(void) 
{
  uint32_t i;
  for(i=0;i<sizeof(OpenSel)/sizeof(uint32_t);i+=3)  
  {
    DSRC_Open(DSRC,OpenSel[i],OpenSel[i+1],OpenSel[i+2]);
    CU_ASSERT( (DSRC->CTL & (DSRC_MODE_Msk)) == OpenSel[i]  );
    CU_ASSERT( (DSRC->CTL & DSRC_CTL_CRCEN_Msk) == OpenSel[i+1]<<DSRC_CTL_CRCEN_Pos );
    if( OpenSel[i+2]==1)
    {
      CU_ASSERT((DSRC->CTL & DSRC_CTL_TRDMAEN_Msk)==OpenSel[i+2]<<DSRC_CTL_TRDMAEN_Pos )
      CU_ASSERT((DSRC->CTL & DSRC_CTL_TTDMAEN_Msk)==OpenSel[i+2]<<DSRC_CTL_TTDMAEN_Pos )
    }
    
    CU_ASSERT((DSRC->CTL &DSRC_CTL_DSRCEN_Msk)==DSRC_CTL_DSRCEN_Msk);
  }
  
  
}

void API_DSRC_Close(void){
  
   DSRC_Open(DSRC,DSRC_MODE_FM0,1,1);
   DSRC_Close(DSRC);
   CU_ASSERT((DSRC->CTL &DSRC_CTL_DSRCEN_Msk)==0);
}


uint32_t interrput_idx[]={
  DSRC_INTEN_CRCCORIE_Msk,DSRC_INTEN_CRCCORIE_Pos,
  DSRC_INTEN_CRCERRIE_Msk,DSRC_INTEN_CRCERRIE_Pos,
  DSRC_INTEN_STRFRMIE_Msk,DSRC_INTEN_STRFRMIE_Pos,
  DSRC_INTEN_STPFRMIE_Msk,DSRC_INTEN_STPFRMIE_Pos,
  DSRC_INTEN_RXDATERRIE_Msk,DSRC_INTEN_RXDATERRIE_Pos,
  DSRC_INTEN_BRATERRIE_Msk,DSRC_INTEN_BRATERRIE_Pos,
  DSRC_INTEN_TTBPDIE_Msk,DSRC_INTEN_TTBPDIE_Pos,
  DSRC_INTEN_RTBPDIE_Msk,DSRC_INTEN_RTBPDIE_Pos,
  DSRC_INTEN_TXDONEIE_Msk,DSRC_INTEN_TXDONEIE_Pos,
  DSRC_INTEN_T2TOIE_Msk,DSRC_INTEN_T2TOIE_Pos,
  DSRC_INTEN_T3TOIE_Msk,DSRC_INTEN_T3TOIE_Pos,
  DSRC_INTEN_T4TOIE_Msk,DSRC_INTEN_T4TOIE_Pos
};

void DSRC_EnableDisableInt_macro(void){
  uint32_t i;
  for(i=0;i<sizeof(interrput_idx)/sizeof(uint32_t);i+=2)
  {
    DSRC_EnableInt(dsrc,interrput_idx[i]);
    CU_ASSERT((DSRC->INTEN & interrput_idx[i])==1<<interrput_idx[i+1] );
    
    
    CU_ASSERT(DSRC_GET_INT(dsrc,interrput_idx[i])==1);
    
    DSRC_DisableInt(dsrc,interrput_idx[i]);
    CU_ASSERT((DSRC->INTEN & interrput_idx[i])==0 );
    
    CU_ASSERT(DSRC_GET_INT(dsrc,interrput_idx[i])==0);
  }
  
}

void DSRC_Other_macro(void)
{
  DSRC_Reset();
  
  DSRC_TRIGGER_RX_PDMA(DSRC);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_TRDMAEN_Msk)==1<<DSRC_CTL_TRDMAEN_Pos);
  
  DSRC_TRIGGER_TX_PDMA(DSRC);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_TTDMAEN_Msk)==1<<DSRC_CTL_TTDMAEN_Pos);
  
  DSRC_Open(DSRC,DSRC_MODE_FM0,1,1);
  DSRC_SET_CRC_FMT(DSRC,0,1,1,1);
  DSRC_FORCE_TX(DSRC);
  CU_ASSERT(DSRC_GET_CurStateMachine(DSRC)==0x6);
  DSRC_FORCE_RX(DSRC);
  CU_ASSERT(DSRC_GET_CurStateMachine(DSRC)==0x1);
  
  DSRC_Reset();
  
  DSRC_RXON(DSRC);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_RXON_Msk)==1<<DSRC_CTL_RXON_Pos);
  DSRC_RXOFF(DSRC);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_RXON_Msk)==0);
  
  DSRC_WKPOL_TRIGGER(DSRC,DSRC_CTL_WKPOL_RISING);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_WKPOL_Msk)==0x0<<DSRC_CTL_WKPOL_Pos);
  DSRC_WKPOL_TRIGGER(DSRC,DSRC_CTL_WKPOL_FALLING);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_WKPOL_Msk)==0x1<<DSRC_CTL_WKPOL_Pos);
  DSRC_WKPOL_TRIGGER(DSRC,DSRC_CTL_WKPOL_BOTH);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_WKPOL_Msk)==0x2<<DSRC_CTL_WKPOL_Pos);
  
  {
    uint32_t time_idx[]={
      0x2, 0x200000,
      0x3, 0xFFFFFF,
      0x1F,0x000001,
    };
    uint32_t i;
    for(i=0;i<sizeof(time_idx)/sizeof(uint32_t);i+=2)
    {
      DSRC_SET_TIMEROUT(DSRC,2,time_idx[i],time_idx[i+1]);
      CU_ASSERT((DSRC->TO[2] )==((time_idx[i]<<24)+time_idx[i+1]));
      DSRC_SET_TIMEROUT(DSRC,3,time_idx[i],time_idx[i+1]);
      CU_ASSERT((DSRC->TO[2] )==((time_idx[i]<<24)+time_idx[i+1]));
      DSRC_SET_TIMEROUT(DSRC,4,time_idx[i],time_idx[i+1]);
      CU_ASSERT((DSRC->TO[2] )==((time_idx[i]<<24)+time_idx[i+1]));
    }
  }
  
  {
    uint32_t i;
    for(i=0;i<=0xF;i++)
    {
      DSRC_SET_CRC_FMT(DSRC,((i>>0)&1),((i>>1)&1),((i>>2)&1),((i>>3)&1));
      CU_ASSERT((DSRC->CTL & (DSRC_CTL_CRCMSB_Msk|DSRC_CTL_CHKSREV_Msk|DSRC_CTL_CHKFMT_Msk|DSRC_CTL_CRCBSWAP_Msk))==
        ((((i>>0)&1)<<DSRC_CTL_CRCMSB_Pos)|
        (((i>>1)&1)<<DSRC_CTL_CHKSREV_Pos)|
        (((i>>2)&1)<<DSRC_CTL_CHKFMT_Pos) |
        (((i>>3)&1)<<DSRC_CTL_CRCBSWAP_Pos)) );
    }
  }
  
  {
    uint32_t i;
    for(i=0;i<=0x7;i++)
    {
      DSRC_SET_BATE_CTL(DSRC,i);
      CU_ASSERT((DSRC->CTL & DSRC_CTL_BRATEACC_Msk)==i<<DSRC_CTL_BRATEACC_Pos);
    }
  }
  
  {
    uint32_t i;
    for(i=0;i<=0x3;i++)
    {
      DSRC_SET_DEGLITCH(DSRC,i);
      CU_ASSERT((DSRCS->CTL2 & DSRC_CTL2_CODECDEGSEL_Msk)==i<<DSRC_CTL2_CODECDEGSEL_Pos);
    }
  }
  
  
  DSRC_ENABLE_CRC(DSRC);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_CRCEN_Msk)==1<<DSRC_CTL_CRCEN_Pos);
  
  DSRC_DISABLE_CRC(DSRC);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_CRCEN_Msk)==0<<DSRC_CTL_CRCEN_Pos);
  
  DSRC_ENABLE_TBP(DSRC);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_TBPEN_Msk)==1<<DSRC_CTL_TBPEN_Pos);
  
  DSRC_DISABLE_TBP(DSRC);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_TBPEN_Msk)==0<<DSRC_CTL_TBPEN_Pos);
  
  DSRC_ENABLE_SWRX(DSRC);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_SWRXEN_Msk)==1<<DSRC_CTL_SWRXEN_Pos);
  
  DSRC_DISABLE_SWRX(DSRC);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_SWRXEN_Msk)==0<<DSRC_CTL_SWRXEN_Pos);
  
  DSRC_SET_SEED(DSRC,0);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_SEEDM_Msk)==0<<DSRC_CTL_SEEDM_Pos);
  
  DSRC_SET_SEED(DSRC,1);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_SEEDM_Msk)==1<<DSRC_CTL_SEEDM_Pos);
  
  
  {
    uint32_t status_idx[]={
      DSRC_STATUS_CRCCOR_Msk,DSRC_STATUS_CRCCOR_Pos,
      DSRC_STATUS_CRCERR_Msk,DSRC_STATUS_CRCERR_Pos,
      DSRC_STATUS_STRFRM_Msk,DSRC_STATUS_STRFRM_Pos,
      DSRC_STATUS_STPFRM_Msk,DSRC_STATUS_STPFRM_Pos,
      DSRC_STATUS_RXDATERR_Msk,DSRC_STATUS_RXDATERR_Pos,
      DSRC_STATUS_BRATERR_Msk,DSRC_STATUS_BRATERR_Pos,
      DSRC_STATUS_TTBPD_Msk,DSRC_STATUS_TTBPD_Pos,
      DSRC_STATUS_RTBPD_Msk,DSRC_STATUS_RTBPD_Pos,
      DSRC_STATUS_TXDONE_Msk,DSRC_STATUS_TXDONE_Pos,
      DSRC_STATUS_TTBPFULL_Msk,DSRC_STATUS_TTBPFULL_Pos,
      DSRC_STATUS_T2TO_Msk,DSRC_STATUS_T2TO_Pos,
      DSRC_STATUS_T3TO_Msk,DSRC_STATUS_T3TO_Pos,
      DSRC_STATUS_T4TO_Msk,DSRC_STATUS_T4TO_Pos,
      DSRC_STATUS_CRC0_OK_Msk,DSRC_STATUS_CRC0_OK_Pos,
      DSRC_STATUS_CRC1_OK_Msk,DSRC_STATUS_CRC1_OK_Pos,
      DSRC_STATUS_EPWKF_Msk,DSRC_STATUS_EPWKF_Pos,
    };
    uint32_t i;
    
    for(i=0;i<sizeof(status_idx)/sizeof(uint32_t);i+=2){
      CU_ASSERT(DSRC_GET_FLAG(DSRC,status_idx[i]) == (DSRC->STATUS & status_idx[i]));
      DSRC_CLR_FLAG(DSRC,status_idx[i]);
      CU_ASSERT((DSRC->STATUS & status_idx[i]) == 0);
    }
  }
  
}

void API_DSRC_SetPreamble(void)
{
  DSRC_SetPreamble(DSRC,DSRC_CTL_PREAMFMT_16,0xFFFF0000);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_PREAMFMT_Msk)==0<<DSRC_CTL_PREAMFMT_Pos);
  CU_ASSERT(DSRC->PREAMBLE ==0xFFFF0000);
  DSRC_SetPreamble(DSRC,DSRC_CTL_PREAMFMT_32,0x12345678);
  CU_ASSERT((DSRC->CTL & DSRC_CTL_PREAMFMT_Msk)==1<<DSRC_CTL_PREAMFMT_Pos);
  CU_ASSERT(DSRC->PREAMBLE ==0x12345678);
}

CU_TestInfo DSRC_MACRO[] =
{
    {"Check Enable/Disable/Get Int",                          DSRC_EnableDisableInt_macro}, 
    {"Check Other Macro ",                          DSRC_Other_macro}, 
    CU_TEST_INFO_NULL
};

CU_TestInfo DSRC_API[] =
{
    {"Check DSRC Open ",   API_DSRC_Open}, 
    {"Check DSRC Close",   API_DSRC_Close}, 
    {"Check DSRC SetPreamble",   API_DSRC_SetPreamble},
    CU_TEST_INFO_NULL
};

CU_SuiteInfo suites[] =
{
    {"DSRC MACRO", suite_success_init, suite_success_clean, NULL, NULL, DSRC_MACRO},
    {"DSRC API", suite_success_init, suite_success_clean, NULL, NULL, DSRC_API},
    CU_SUITE_INFO_NULL
};
