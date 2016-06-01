/**************************************************************************//**
 * @file     dsrc.c
 * @version  V1.00
 * $Revision: 5 $
 * $Date: 15/12/29 12:56p $
 * @brief    NANO103 series DSRC driver source file
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano103.h"
#include "dsrc.h"

/** @addtogroup NANO103_Device_Driver NANO103 Device Driver
  @{
*/

/** @addtogroup NANO103_DSRC_Driver DSRC Driver
  @{
*/


/** @addtogroup NANO103_DSRC_EXPORTED_FUNCTIONS DSRC Exported Functions
  @{
*/

/**
  * @brief  This function make DSRC module be ready to transfer.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32Mode decides the DSRC module is operating. Valid values are:
  *              - \ref DSRC_MODE_FM0
  *              - \ref DSRC_MODE_MONCHESTER
  *              - \ref DSRC_MODE_SPI
  * @param[in]  u32crcEn is enable/disable crc engine. 0 : Disable CRC engine. 1: Enable CRC engine.
  * @param[in]  u32pdmaEn is enable/disable pdma engine. 0 : Disable PDMA engine. 1: Enable PDMA engine.
  * @return None.
  */
void DSRC_Open(DSRC_T *dsrc,uint32_t u32Mode,uint32_t u32crcEn,uint32_t u32pdmaEn)
{
  DSRC->CTL = (DSRC->CTL & ~DSRC_MODE_Msk) | u32Mode;
  
  /* Enable DSRC */
  DSRC->CTL |= DSRC_CTL_DSRCEN_Msk;
  
  /* Enable TBP */
  DSRC_ENABLE_TBP(DSRC);
  
  /* Enable CRC */
  if(u32crcEn==1)
    DSRC_ENABLE_CRC(DSRC);
  else
    DSRC_DISABLE_CRC(DSRC);  //Disable CRC 
  
  if(u32pdmaEn==1)
  {
    /* Enable RX PDMA */
    DSRC_TRIGGER_RX_PDMA(DSRC);
    /* Enable TX PDMA */
    DSRC_TRIGGER_TX_PDMA(DSRC);
  }else{
    /* Disable RX PDMA */
    DSRC->CTL &= ~(1<<DSRC_CTL_TRDMAEN_Pos);
    /* Enable TX PDMA */
    DSRC->CTL &= ~(1<<DSRC_CTL_TTDMAEN_Pos);
  }
}

/**
  * @brief Reset DSRC module and disable DSRC peripheral clock.
  * @param[in]  dsrc is the base address of DSRC module.
  * @return none
  */
void DSRC_Close(DSRC_T *dsrc){
   DSRC->CTL &= ~DSRC_CTL_DSRCEN_Msk;
}
  
  
/**
  * @brief  This function make DSRC module be ready to transfer preamble.
  * @param[in]  dsrc is the base address of DSRC module.
  * @param[in]  u32Mode decides the DSRC module is premable size. Valid values are:
  *              - \ref DSRC_CTL_PREAMFMT_16, It's pattern is u32Val[15:0]
  *              - \ref DSRC_CTL_PREAMFMT_32, It's pattern is u32Val[31:0]
  * @param[in]  u32Val is preamble
  * @return none
  */
void DSRC_SetPreamble(DSRC_T *dsrc,uint32_t u32Mode,uint32_t u32Val)
{
  DSRC->CTL = (DSRC->CTL & ~DSRC_CTL_PREAMFMT_Msk) | (u32Mode);
  DSRC->PREAMBLE = u32Val;
}

/*@}*/ /* end of group NANO103_DSRC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NANO103_DSRC_Driver */

/*@}*/ /* end of group NANO103_Device_Driver */

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
