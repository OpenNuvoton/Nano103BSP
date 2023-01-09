/**************************************************************************//**
 * @file     fmc.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/12/18 1:53p $
 * @brief    Nano 103 FMC driver source file
 *
 * @note
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

//* Includes ------------------------------------------------------------------*/
#include <stdio.h>

#include "Nano103.h"


/** @addtogroup NANO103_Device_Driver NANO103 Device Driver
  @{
*/

/** @addtogroup NANO103_FMC_Driver FMC Driver
  @{
*/

int32_t g_FMC_i32ErrCode = 0; /*!< FMC global error code */

/** @addtogroup NANO103_FMC_EXPORTED_FUNCTIONS FMC Exported Functions
  @{
*/


/**
  * @brief Disable FMC ISP function.
  * @return None
  */
void FMC_Close(void)
{
    FMC->ISPCTL &= ~FMC_ISPCTL_ISPEN_Msk;
}

/**
  * @brief Execute FMC_ISPCMD_PAGE_ERASE command to erase a flash page. The page size is 512 bytes.
  * @param[in]  u32PageAddr Address of the flash page to be erased.
  *             It must be a 512-byte aligned address.
  * @return ISP page erase success or not.
  * @retval   0  Success
  * @retval   -1  Erase failed
  */
int32_t FMC_Erase(uint32_t u32PageAddr)
{
    uint32_t u32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    FMC->ISPCMD = FMC_ISPCMD_PAGE_ERASE;
    FMC->ISPADDR = u32PageAddr;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    u32TimeOutCnt = FMC_TIMEOUT_ERASE;
    while(FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk)			/* Waiting for ISP Done */
    {
        if(--u32TimeOutCnt == 0)
        {
            g_FMC_i32ErrCode = -1;
            return -1;
        }
    }

    if (FMC->ISPCTL & FMC_ISPCTL_ISPFF_Msk)
    {
        FMC->ISPCTL |= FMC_ISPCTL_ISPFF_Msk;
        g_FMC_i32ErrCode = -1;
        return -1;
    }

    return 0;
}

/**
  * @brief Get the current boot source.
  * @return The current boot source.
  * @retval   0  Is boot from APROM.
  * @retval   1  Is boot from LDROM.
  */
int32_t FMC_GetBootSource (void)
{
    if (FMC->ISPCTL & FMC_ISPCTL_BS_Msk)
        return 1;
    else
        return 0;
}


/**
  * @brief Enable FMC ISP function
  * @return None
  */
void FMC_Open(void)
{
    FMC->ISPCTL |=  FMC_ISPCTL_ISPEN_Msk;
}


/**
  * @brief Execute ISP command to read a word from flash.
  * @param[in]  u32Addr Address of the flash location to be read.
  *             It must be a word aligned address.
  * @return The word data read from specified flash address.
  *          Return 0xFFFFFFFF if read failed.
  *
  * @note     Global error code g_FMC_i32ErrCode
  *           -1  Read time-out
  */
uint32_t FMC_Read(uint32_t u32Addr)
{
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    FMC->ISPCMD = FMC_ISPCMD_READ;
    FMC->ISPADDR = u32Addr;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    i32TimeOutCnt = FMC_TIMEOUT_READ;
    while ((i32TimeOutCnt-- > 0) && (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk)) {}
    if (i32TimeOutCnt <= 0)
    {
        g_FMC_i32ErrCode = -1;
        return 0xFFFFFFFF;
    }
    return FMC->ISPDAT;
}


/**
  * @brief    Read company ID.
  * @return   The company ID.
  *
  * @note     Global error code g_FMC_i32ErrCode
  *           -1  Read time-out
  */
uint32_t FMC_ReadCID(void)
{
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    FMC->ISPCMD = FMC_ISPCMD_READ_CID;
    FMC->ISPADDR = 0x0;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    i32TimeOutCnt = FMC_TIMEOUT_READ;
    while ((i32TimeOutCnt-- > 0) && (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk)) {}
    if (i32TimeOutCnt <= 0)
    {
        g_FMC_i32ErrCode = -1;
        return 0xFFFFFFFF;
    }

    return FMC->ISPDAT;
}


/**
  * @brief    Read product ID.
  * @return   The product ID.
  *
  * @note     Global error code g_FMC_i32ErrCode
  *           -1  Read time-out
  */
uint32_t FMC_ReadPID(void)
{
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    FMC->ISPCMD = FMC_ISPCMD_READ_PID;
    FMC->ISPADDR = 0x04;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    i32TimeOutCnt = FMC_TIMEOUT_READ;
    while ((i32TimeOutCnt-- > 0) && (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk)) {}
    if (i32TimeOutCnt <= 0)
    {
        g_FMC_i32ErrCode = -1;
        return 0xFFFFFFFF;
    }

    return FMC->ISPDAT;
}


/**
  * @brief    This function reads one of the four UCID.
  * @param[in]   u32Index  Index of the UCID to read. u32Index must be 0, 1, 2, or 3.
  * @return   The UCID.
  *
  * @note     Global error code g_FMC_i32ErrCode
  *           -1  Read time-out
  */
uint32_t FMC_ReadUCID(uint32_t u32Index)
{
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    FMC->ISPCMD = FMC_ISPCMD_READ_UID;
    FMC->ISPADDR = (0x04 * u32Index) + 0x10;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    i32TimeOutCnt = FMC_TIMEOUT_READ;
    while ((i32TimeOutCnt-- > 0) && (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk)) {}
    if (i32TimeOutCnt <= 0)
    {
        g_FMC_i32ErrCode = -1;
        return 0xFFFFFFFF;
    }

    return FMC->ISPDAT;
}


/**
  * @brief    This function reads one of the three UID.
  * @param[in]  u32Index Index of the UID to read. u32Index must be 0, 1, or 2.
  * @return   The UID.
  *
  * @note     Global error code g_FMC_i32ErrCode
  *           -1  Read time-out
  */
uint32_t FMC_ReadUID(uint32_t u32Index)
{
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    FMC->ISPCMD = FMC_ISPCMD_READ_UID;
    FMC->ISPADDR = 0x04 * u32Index;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    i32TimeOutCnt = FMC_TIMEOUT_READ;
    while ((i32TimeOutCnt-- > 0) && (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk)) {}
    if (i32TimeOutCnt <= 0)
    {
        g_FMC_i32ErrCode = -1;
        return 0xFFFFFFFF;
    }

    return FMC->ISPDAT;
}


/**
  * @brief    Get the base address of Data Flash if enabled.
  * @return   The base address of Data Flash
  */
uint32_t FMC_ReadDataFlashBaseAddr(void)
{
    return FMC->DFBA;
}


/**
  * @brief    This function will force re-map assigned flash page to CPU address 0x0.
  * @param[in]  u32PageAddr Address of the page to be mapped to CPU address 0x0.
  * @return  None
  *
  * @note     Global error code g_FMC_i32ErrCode
  *           -1  Time-out
  */
void FMC_SetVectorPageAddr(uint32_t u32PageAddr)
{
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    FMC->ISPCMD = FMC_ISPCMD_VECMAP;
    FMC->ISPADDR = u32PageAddr;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    i32TimeOutCnt = FMC_TIMEOUT_WRITE;
    while ((i32TimeOutCnt-- > 0) && (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk)) {}
    if (i32TimeOutCnt <= 0)
        g_FMC_i32ErrCode = -1;
}


/**
  * @brief    Obtain the current vector page address setting.
  * @return   The vector page address.
  */
uint32_t FMC_GetVectorPageAddr(void)
{
    return (FMC->ISPSTS & 0x0FFFFF00ul);
}


/**
  * @brief Execute ISP command to program a word to flash.
  * @param[in]  u32Addr Address of the flash location to be programmed.
  *             It must be a word aligned address.
  * @param[in]  u32Data The word data to be programmed.
  * @return   0   Success
  * @return   -1  Program Failed
  *
  * @note     Global error code g_FMC_i32ErrCode
  *           -1  Program failed or time-out
  */
int32_t FMC_Write(uint32_t u32Addr, uint32_t u32Data)
{
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    FMC->ISPCMD = FMC_ISPCMD_PROGRAM;
    FMC->ISPADDR = u32Addr;
    FMC->ISPDAT = u32Data;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    i32TimeOutCnt = FMC_TIMEOUT_WRITE;
    while ((i32TimeOutCnt-- > 0) && (FMC->ISPTRG & FMC_ISPTRG_ISPGO_Msk)) {}
    if (i32TimeOutCnt <= 0)
    {
        g_FMC_i32ErrCode = -1;
        return -1;
    }

    return 0;
}


/**
  * @brief Execute ISP command to read User Configuration.
  * @param[out]  u32Config A two-word array.
  *              u32Config[0] holds CONFIG0, while u32Config[1] holds CONFIG1.
  * @param[in] u32Count Available word count in u32Config.
  * @return Success or not.
  * @retval   0  Success.
  * @retval   -1  Invalid parameter.
  */
int32_t FMC_ReadConfig(uint32_t *u32Config, uint32_t u32Count)
{
    u32Config[0] = FMC_Read(FMC_CONFIG_BASE);
    if (u32Count < 2)
        return 0;
    u32Config[1] = FMC_Read(FMC_CONFIG_BASE+4);
    return 0;
}


/**
  * @brief Execute ISP command to write User Configuration.
  * @param[in] u32Config A two-word array.
  *            u32Config[0] holds CONFIG0, while u32Config[1] holds CONFIG1.
  * @param[in] u32Count Available word count in u32Config.
  * @return Success or not.
  * @retval   0  Success.
  * @retval   -1  Invalid parameter.
  */
int32_t FMC_WriteConfig(uint32_t *u32Config, uint32_t u32Count)
{
    FMC_ENABLE_CFG_UPDATE();
    FMC_Erase(FMC_CONFIG_BASE);
    FMC_Write(FMC_CONFIG_BASE, u32Config[0]);
    FMC_Write(FMC_CONFIG_BASE+4, u32Config[1]);
    FMC_DISABLE_CFG_UPDATE();
    return 0;
}


/**
  * @brief Run CRC32 checksum calculation and get result.
  * @param[in]  u32Addr   Starting flash address. It must be a page aligned address.
  * @param[in]  u32Count  Byte count of flash to be calculated. It must be multiple of 512 bytes.
  * @param[out] u32ChkSum The CRC32 checksum if caculation is successful.
  * @return Success or not.
  * @retval   0           Success
  * @retval   -1          Failed
  * @retval   -2          Invalid parameter.
  *
  * @note     Global error code g_FMC_i32ErrCode
  *           -1  Run/Read check sum time-out failed
  *           -2  u32addr or u32count must be aligned with 512
  */
int32_t  FMC_GetChkSum(uint32_t u32Addr, uint32_t u32Count, uint32_t *u32ChkSum)
{
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    if ((u32Addr % 512) || (u32Count % 512))
    {
        g_FMC_i32ErrCode = -2;
        return 0xFFFFFFFF;
    }

    *u32ChkSum = 0;

    FMC->ISPCMD  = FMC_ISPCMD_RUN_CKS;
    FMC->ISPADDR = u32Addr;
    FMC->ISPDAT  = u32Count;
    FMC->ISPTRG  = FMC_ISPTRG_ISPGO_Msk;

    i32TimeOutCnt = FMC_TIMEOUT_CHKSUM;
    while ((i32TimeOutCnt-- > 0) && (FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk)) {}
    if (i32TimeOutCnt <= 0)
    {
        g_FMC_i32ErrCode = -1;
        return 0xFFFFFFFF;
    }

    if (FMC_GET_FAIL_FLAG())
        return -1;

    FMC->ISPCMD = FMC_ISPCMD_READ_CKS;
    FMC->ISPADDR    = u32Addr;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;

    i32TimeOutCnt = FMC_TIMEOUT_CHKSUM;
    while ((i32TimeOutCnt-- > 0) && (FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk)) {}
    if (i32TimeOutCnt <= 0)
    {
        g_FMC_i32ErrCode = -1;
        return 0xFFFFFFFF;
    }

    if (FMC_GET_FAIL_FLAG())
        return -1;

    *u32ChkSum = FMC->ISPDAT;
    return 0;
}


/**
  * @brief Run flash all one verification and get result.
  * @param[in] u32Addr   Starting flash address. It must be a page aligned address.
  * @param[in] u32Count  Byte count of flash to be calculated. It must be multiple of 512 bytes.
  * @retval   READ_ALLONE_YES       The contents of verified flash area are 0xFFFFFFFF.
  * @retval   READ_ALLONE_NOT       Some contents of verified flash area are not 0xFFFFFFFF.
  * @retval   READ_ALLONE_CMD_FAIL  Unexpected error occurred.
  *
  * @note     Global error code g_FMC_i32ErrCode
  *           -1  RUN_ALL_ONE or CHECK_ALL_ONE commands time-out
  */
uint32_t  FMC_CheckAllOne(uint32_t u32Addr, uint32_t u32Count)
{
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    FMC->ISPSTS = 0x80;   // clear check alll one bit

    FMC->ISPCMD = FMC_ISPCMD_RUN_ALL1;
    FMC->ISPADDR    = u32Addr;
    FMC->ISPDAT = u32Count;
    FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;


    i32TimeOutCnt = FMC_TIMEOUT_CHKALLONE;
    while ((i32TimeOutCnt-- > 0) && (FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk)) {}
    if (i32TimeOutCnt <= 0)
    {
        g_FMC_i32ErrCode = -1;
        return READ_ALLONE_CMD_FAIL;
    }

    if (FMC_GET_FAIL_FLAG())
    {
        //printf("FMC_ISPCMD_RUN_ALL1 ISP failed!\n");
        return READ_ALLONE_CMD_FAIL;
    }

    i32TimeOutCnt = FMC_TIMEOUT_CHKALLONE;
    do
    {
        FMC->ISPCMD = FMC_ISPCMD_READ_ALL1;
        FMC->ISPADDR    = u32Addr;
        FMC->ISPTRG = FMC_ISPTRG_ISPGO_Msk;
        while ((i32TimeOutCnt-- > 0) && (FMC->ISPSTS & FMC_ISPSTS_ISPBUSY_Msk)) {}
        if (i32TimeOutCnt <= 0)
        {
            g_FMC_i32ErrCode = -1;
            return READ_ALLONE_CMD_FAIL;
        }
    }
    while (FMC->ISPDAT == 0);

    if (FMC_GET_FAIL_FLAG())
    {
        printf("FMC_ISPCMD_READ_ALL1 ISP failed!\n");
        g_FMC_i32ErrCode = -1;
        return READ_ALLONE_CMD_FAIL;
    }

    if ((FMC->ISPDAT == READ_ALLONE_YES) || (FMC->ISPDAT == READ_ALLONE_NOT))
        return FMC->ISPDAT;

    return READ_ALLONE_CMD_FAIL;
}


/**
  * @brief    Setup security key.
  * @param[in] key      Key0~2 to be setup.
  * @param[in] kpmax    Maximum unmatched power-on counting number.
  * @param[in] kemax    Maximum unmatched counting number.
  * @param[in] lock_CONFIG   1: Security key lock CONFIG to write-protect. 0: Don't lock CONFIG.
  * @retval   0     Success.
  * @retval   -1    Key has been setup, It's not allowed to setup key again.
  * @retval   -2    Failed to erase flash.
  * @retval   -3    Failed to program key.
  * @retval   -4    Key lock function failed.
  * @retval   -5    CONFIG lock function failed.
  * @retval   -7    KPMAX function failed.
  * @retval   -8    KEMAX function failed.
  *
  * @note     Global error code g_FMC_i32ErrCode
  *                 Same as the return value of this function.
  */
int32_t  FMC_SKey_Setup(uint32_t key[3], uint32_t kpmax, uint32_t kemax, int lock_CONFIG)
{
    uint32_t  lock_ctrl = 0;
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    if (FMC->KEYSTS != 0)
    {
        g_FMC_i32ErrCode = -1;
        return g_FMC_i32ErrCode;
    }

    if (FMC_Erase(FMC_KPROM_BASE))
    {
        g_FMC_i32ErrCode = -2;
        return g_FMC_i32ErrCode;
    }

    if (FMC_Erase(FMC_KPROM_BASE+FMC_FLASH_PAGE_SIZE))
    {
        g_FMC_i32ErrCode = -2;
        return g_FMC_i32ErrCode;
    }

    if (!lock_CONFIG)
        lock_ctrl |= 0x1;

    FMC_Write(FMC_KPROM_BASE, key[0]);
    FMC_Write(FMC_KPROM_BASE+0x4, key[1]);
    FMC_Write(FMC_KPROM_BASE+0x8, key[2]);
    FMC_Write(FMC_KPROM_BASE+0xC, kpmax);
    FMC_Write(FMC_KPROM_BASE+0x10, kemax);
    FMC_Write(FMC_KPROM_BASE+0x14, lock_ctrl);

    i32TimeOutCnt = FMC_TIMEOUT_WRITE;
    while ((i32TimeOutCnt-- > 0) && (FMC->KEYSTS & FMC_KEYSTS_KEYBUSY_Msk)) {}
    if(i32TimeOutCnt <= 0)
    {
        g_FMC_i32ErrCode = -1;
        return g_FMC_i32ErrCode;
    }

    if (!(FMC->KEYSTS & FMC_KEYSTS_KEYLOCK_Msk))
    {
        printf("Security key lock failed!\n");
        g_FMC_i32ErrCode = -4;
        return g_FMC_i32ErrCode;
    }

    if ((lock_CONFIG && !(FMC->KEYSTS & FMC_KEYSTS_CFGFLAG_Msk)) ||
            (!lock_CONFIG && (FMC->KEYSTS & FMC_KEYSTS_CFGFLAG_Msk)))
    {
        printf("CONFIG lock failed!\n");
        g_FMC_i32ErrCode = -5;
        return g_FMC_i32ErrCode;
    }

    if (((FMC->KPCNT & FMC_KPCNT_KPMAX_Msk) >> FMC_KPCNT_KPMAX_Pos) != kpmax)
    {
        printf("KPMAX failed!\n");
        g_FMC_i32ErrCode = -7;
        return g_FMC_i32ErrCode;
    }

    if (((FMC->KECNT & FMC_KECNT_KEMAX_Msk) >> FMC_KECNT_KEMAX_Pos) != kemax)
    {
        printf("KEMAX failed!\n");
        g_FMC_i32ErrCode = -8;
        return g_FMC_i32ErrCode;
    }

    return 0;
}


/**
  * @brief    Execute security key comparison.
  * @param[in] key  Key0~2 to be compared.
  * @retval   0     Key matched.
  * @retval   -1    Forbidden. Times of key comparison mismatch has reached the maximum count.
  * @retval   -2    Key mismatched.
  * @retval   -3    No security key lock. Key comparison is not required.
  *
  * @note     Global error code g_FMC_i32ErrCode
  *                 Same as the return value of this function.
  */
int32_t  FMC_SKey_Compare(uint32_t key[3])
{
    int32_t i32TimeOutCnt;

    g_FMC_i32ErrCode = 0;

    if (FMC->KEYSTS & FMC_KEYSTS_FORBID_Msk)
    {
        printf("FMC_SKey_Compare - FORBID!\n");
        g_FMC_i32ErrCode = -1;
        return -1;
    }

    if (!(FMC->KEYSTS & FMC_KEYSTS_KEYLOCK_Msk))
    {
        printf("FMC_SKey_Compare - key is not locked!\n");
        g_FMC_i32ErrCode = -3;
        return -3;
    }

    FMC->KEY0 = key[0];
    FMC->KEY1 = key[1];
    FMC->KEY2 = key[2];
    FMC->KEYTRG = FMC_KEYTRG_KEYGO_Msk | FMC_KEYTRG_TCEN_Msk;

    i32TimeOutCnt = FMC_TIMEOUT_READ;
    while ((i32TimeOutCnt-- > 0) && (FMC->KEYSTS & FMC_KEYSTS_KEYBUSY_Msk)) {}
    if (i32TimeOutCnt <= 0)
    {
        g_FMC_i32ErrCode = -1;
        return 1;
    }

    if (!(FMC->KEYSTS & FMC_KEYSTS_KEYMATCH_Msk))
    {
        printf("Key mismatched!\n");
        g_FMC_i32ErrCode = -2;
        return -2;
    }

    if (FMC->KEYSTS & FMC_KEYSTS_KEYLOCK_Msk)
    {
        printf("Key matched, but still be locked!\n");
        g_FMC_i32ErrCode = -2;
        return -2;
    }

    printf("FMC_SKey_Compare - OK.\n");
    return 0;
}


/*@}*/ /* end of group NANO103_FMC_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group NANO103_FMC_Driver */

/*@}*/ /* end of group NANO103_Device_Driver */

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/


