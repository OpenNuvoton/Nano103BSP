/**************************************************************************//**
 * @file     fmc_cunit.c
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 15/12/28 4:02p $ 
 * @brief    Mini58 series FMC CUnit test function.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *****************************************************************************/  

//* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "Nano103.h"
#include "CUnit.h"
#include "Console.h"
#include "fmc_cunit.h"


extern unsigned int STATE0;
extern unsigned int STATE1;
extern unsigned int ERR_STATE0;
extern unsigned int ERR_STATE1;
/*---------------------------------------------------------------------------------------------------------*/
/* Test function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/

int FMC_Tests_Init(void)
{
    return 0;
}
int FMC_Tests_Clean(void)
{
    return 0;
}

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
    {"FMC CONST", FMC_Tests_Init, FMC_Tests_Clean, NULL, NULL, FMC_ConstantTests},
    {"FMC MACRO", FMC_Tests_Init, FMC_Tests_Clean, NULL, NULL, FMC_MacroTests},
    {"FMC API", FMC_Tests_Init, FMC_Tests_Clean, NULL, NULL, FMC_ApiTests},
    CU_SUITE_INFO_NULL
};



void MACRO_FMC_ENABLE_ISP()
{

    FMC_ENABLE_ISP();

    CU_ASSERT_TRUE(FMC->ISPCTL & FMC_ISPCTL_ISPEN_Msk);

    FMC_DISABLE_ISP();

    CU_ASSERT_FALSE(FMC->ISPCTL & FMC_ISPCTL_ISPEN_Msk);

}

void MACRO_FMC_ENABLE_LD_UPDATE()
{
    FMC_ENABLE_ISP();
    FMC_ENABLE_LD_UPDATE();

    CU_ASSERT_TRUE(FMC->ISPCTL & FMC_ISPCTL_LDUEN_Msk);

    FMC_Erase(0x100000);
    FMC_Write(0x1001fc, 0xabcd1234);

    CU_ASSERT(FMC_Read(0x1001fc) == 0xabcd1234);

    FMC_DISABLE_LD_UPDATE();
    CU_ASSERT_FALSE(FMC->ISPCTL & FMC_ISPCTL_LDUEN_Msk);

    FMC_Erase(0x100000);
    CU_ASSERT(FMC_Read(0x1001fc) == 0xabcd1234)

    FMC_DISABLE_ISP();
}

void MACRO_FMC_ENABLE_SP_UPDATE()
{
    FMC_ENABLE_ISP();
    FMC_ENABLE_SP_UPDATE();

    CU_ASSERT_TRUE(FMC->ISPCTL & FMC_ISPCTL_SPUEN_Msk);

    FMC_Erase_SPROM();

    CU_ASSERT(FMC_Read(0x200100) == 0xFFFFFFFF);
    FMC_Write(0x200100, 0xabcd1234);
    CU_ASSERT(FMC_Read(0x200100) == 0xabcd1234);

    FMC_DISABLE_SP_UPDATE();
    CU_ASSERT_FALSE(FMC->ISPCTL & FMC_ISPCTL_SPUEN_Msk);

    FMC_Erase_SPROM();
    CU_ASSERT(FMC_Read(0x200100) == 0xabcd1234)

    FMC_DISABLE_ISP();
}

void MACRO_FMC_ENABLE_CFG_UPDATE()
{
    uint32_t u32Cfg0, u32Cfg1;

    FMC_ENABLE_ISP();
    FMC_ENABLE_CFG_UPDATE();

    CU_ASSERT_TRUE(FMC->ISPCTL & FMC_ISPCTL_CFGUEN_Msk);

    u32Cfg0 = FMC_Read(FMC_CONFIG_BASE);
    u32Cfg1 = FMC_Read(FMC_CONFIG_BASE+4);

    FMC_Erase(FMC_CONFIG_BASE);
    CU_ASSERT(FMC_Read(FMC_CONFIG_BASE) == 0xfffffffful);
    CU_ASSERT(FMC_Read(FMC_CONFIG_BASE+4) == 0xfffffffful);

    FMC_Write(FMC_CONFIG_BASE, u32Cfg0);
    FMC_Write(FMC_CONFIG_BASE+4, u32Cfg1);

    CU_ASSERT(FMC_Read(FMC_CONFIG_BASE) == u32Cfg0);
    CU_ASSERT(FMC_Read(FMC_CONFIG_BASE+4) == u32Cfg1);

    FMC_DISABLE_CFG_UPDATE();
    CU_ASSERT_FALSE(FMC->ISPCTL & FMC_ISPCTL_CFGUEN_Msk);

    FMC_Erase(FMC_CONFIG_BASE);
     
    CU_ASSERT(FMC_Read(FMC_CONFIG_BASE) == u32Cfg0);
    CU_ASSERT(FMC_Read(FMC_CONFIG_BASE+4) == u32Cfg1)

    FMC_DISABLE_ISP();
}


void MACRO_FMC_ENABLE_AP_UPDATE()
{

    FMC_ENABLE_ISP();

    FMC_ENABLE_AP_UPDATE();

    CU_ASSERT_TRUE(FMC->ISPCTL & FMC_ISPCTL_APUEN_Msk);
    
    FMC_Erase(0xA000);
    FMC_Write(0xA000, 0x55aa8822);

    CU_ASSERT(FMC_Read(0xA000) == 0x55aa8822);
    CU_ASSERT(FMC_Read(0xA004) == 0xfffffffful);

    FMC_DISABLE_AP_UPDATE();
    CU_ASSERT_FALSE(FMC->ISPCTL & FMC_ISPCTL_APUEN_Msk);
    FMC_Write(0xA004, 0xaa552288);
    CU_ASSERT(FMC_Read(0xA004) == 0xFFFFFFFFul);

    FMC_DISABLE_ISP();
}


void MACRO_FMC_GET_FAIL_FLAG()
{
    FMC_ENABLE_ISP();
    FMC_DISABLE_CFG_UPDATE();

    FMC_Read(0);
    CU_ASSERT(FMC_GET_FAIL_FLAG() == 0);

    FMC_Write(FMC_CONFIG_BASE, 0x01);

    CU_ASSERT(FMC_GET_FAIL_FLAG() == 1);
    
    FMC_Erase(FMC_CONFIG_BASE);

    CU_ASSERT(FMC_GET_FAIL_FLAG() == 0);
    
    FMC_DISABLE_ISP();
}


void MACRO_FMC_CLR_FAIL_FLAG()
{
    FMC_ENABLE_ISP();
    FMC_DISABLE_CFG_UPDATE();

    FMC_Write(FMC_CONFIG_BASE, 0x01);

    CU_ASSERT(FMC_GET_FAIL_FLAG() == 1);
    
    FMC_CLR_FAIL_FLAG();

    CU_ASSERT(FMC_GET_FAIL_FLAG() == 0);
    
    FMC_DISABLE_ISP();
}



void MACRO_FMC_SET_APROM_BOOT()
{

    FMC_ENABLE_ISP();

    FMC_SET_LDROM_BOOT();
    CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_BS_Msk) == FMC_ISPCTL_BS_Msk);

    FMC_SET_APROM_BOOT();
    CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_BS_Msk) == 0);

    FMC_DISABLE_ISP();
}



void API_FMC_Write()
{
    int32_t   i;
    uint32_t  u32Data;

    FMC_ENABLE_ISP();
    FMC_ENABLE_LD_UPDATE();

    FMC_Erase(FMC_LDROM_BASE);
    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
        FMC_Write(FMC_LDROM_BASE + i, FMC_LDROM_BASE + i);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
    {
        u32Data = FMC_Read(FMC_LDROM_BASE + i);
        if (u32Data != FMC_LDROM_BASE + i)
            CU_FAIL();
    }

    CU_PASS();

    FMC_DISABLE_LD_UPDATE();
    FMC_DISABLE_ISP();
}


void API_FMC_Read()
{
    int32_t i;
    uint32_t u32Data;

    FMC_ENABLE_ISP();

    i = 0x1000;
    CU_ASSERT(FMC_Read(i) == M32(i));
    i = 0x1004;
    CU_ASSERT(FMC_Read(i) == M32(i));
    i = 0x2000;
    CU_ASSERT(FMC_Read(i) == M32(i));
    i = 0xffc;
    CU_ASSERT(FMC_Read(i) == M32(i));


    FMC_ENABLE_AP_UPDATE();

    FMC_Erase(TEST_CONFIG1);
    for(i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
        FMC_Write(TEST_CONFIG1 + i, TEST_CONFIG1 + i);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
    {
        u32Data = FMC_Read(TEST_CONFIG1 + i);
        if(u32Data != TEST_CONFIG1 + i)
            CU_FAIL();
    }

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
    {
        u32Data = M32(TEST_CONFIG1 + i);
        if(u32Data != TEST_CONFIG1 + i)
            CU_FAIL();
    }

    CU_PASS();

    FMC_DISABLE_ISP();
    FMC_DISABLE_AP_UPDATE();
}

void API_FMC_Erase()
{
    int32_t i;
    uint32_t u32Data;

    FMC_ENABLE_ISP();
    FMC_ENABLE_AP_UPDATE();

    FMC_Erase(TEST_CONFIG1);
    for(i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
        FMC_Write(TEST_CONFIG1 + i, TEST_CONFIG1 + i);

    for (i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
    {
        u32Data = FMC_Read(TEST_CONFIG1 + i);
        if(u32Data != TEST_CONFIG1 + i)
            CU_FAIL();
    }

    FMC_Erase(TEST_CONFIG1);
    for(i = 0; i < FMC_FLASH_PAGE_SIZE; i += 4)
    {
        u32Data = FMC_Read(TEST_CONFIG1 + i);
        if(u32Data != 0xFFFFFFFFul)
            CU_FAIL();
    }

    CU_PASS();

    FMC_DISABLE_ISP();
    FMC_DISABLE_AP_UPDATE();
}

void API_FMC_Erase_SPROM()
{
	MACRO_FMC_ENABLE_SP_UPDATE();
}

void API_FMC_GetBootSource(void)
{
    FMC_ENABLE_ISP();
    
    if (FMC_GetBootSource() == 0)
    {
    	CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_BS_Msk) == 0);
    }
    else
    { 
    	if (FMC_GetBootSource() == 1)
    	{
    		CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_BS_Msk) == FMC_ISPCTL_BS_Msk);
    	}
    	else
    	{
    		CU_FAIL();
    	}
    }

    FMC_DISABLE_ISP();
}


void API_FMC_ReadUID(void)
{
    FMC_ENABLE_ISP();

    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUID(0) != 0xFFFFFFFF);
    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUID(1) != 0);
    CU_ASSERT(FMC_ReadUID(0) != 0xFFFFFFFF);
    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUID(2) != 0);
    CU_ASSERT(FMC_ReadUID(0) != 0xFFFFFFFF);

    CU_ASSERT(FMC_ReadUID(0) != FMC_ReadUID(1));
    CU_ASSERT(FMC_ReadUID(1) != FMC_ReadUID(2));

    FMC_DISABLE_ISP();
}


void API_FMC_ReadCID(void)
{
    FMC_ENABLE_ISP();

    CU_ASSERT(FMC_ReadCID() == 0xda);

    FMC_DISABLE_ISP();
}


void API_FMC_ReadPID(void)
{
    FMC_ENABLE_ISP();

    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadPID() != 0);

    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadPID() != 0xFFFFFFFFul);

    FMC_DISABLE_ISP();
}


void API_FMC_ReadUCID(void)
{
    FMC_ENABLE_ISP();

    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUCID(0) != 0);
    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUCID(1) != 0);
    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUCID(2) != 0);
    FMC->ISPDAT = 0;
    CU_ASSERT(FMC_ReadUCID(3) != 0);

    FMC_DISABLE_ISP();
}


void API_FMC_SetVectorPageAddr(void)
{
    uint32_t u32Addr;

    FMC_ENABLE_ISP();

    for(u32Addr = 0; u32Addr < TEST_CONFIG1; u32Addr += FMC_FLASH_PAGE_SIZE)
    {
        FMC_SetVectorPageAddr(u32Addr);
        if ((FMC->ISPSTS & 0x0FFFFF00ul) != u32Addr)
            CU_FAIL();
        if ((FMC->ISPSTS & 0x0FFFFF00ul) != FMC_GetVectorPageAddr())
            CU_FAIL();
    }

    FMC_SetVectorPageAddr(0);

    FMC_DISABLE_ISP();

    CU_PASS();
}


void API_FMC_Open(void)
{
    FMC->ISPCTL &= ~FMC_ISPCTL_ISPEN_Msk;

    FMC_Open();

    CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_ISPEN_Msk) == FMC_ISPCTL_ISPEN_Msk);
}


void API_FMC_Close(void)
{
    FMC->ISPCTL |= FMC_ISPCTL_ISPEN_Msk;

    FMC_Close();

    CU_ASSERT((FMC->ISPCTL & FMC_ISPCTL_ISPEN_Msk) == 0);
}



void API_FMC_ReadDataFlashBaseAddr(void)
{
    FMC_ENABLE_ISP();

    CU_ASSERT(FMC_ReadDataFlashBaseAddr() == FMC_Read(0x300004));

    FMC_DISABLE_ISP();

}



void API_FMC_ReadConfig(void)
{
    uint32_t u32Cfg[4] = {0};

    FMC_ENABLE_ISP();

    FMC_ReadConfig(u32Cfg, 2);
    CU_ASSERT(u32Cfg[0] == TEST_CONFIG0);
    CU_ASSERT(u32Cfg[1] == TEST_CONFIG1);
    CU_ASSERT(u32Cfg[2] == 0);

    FMC_DISABLE_ISP();
}


void API_FMC_WriteConfig(void)
{
    uint32_t u32Cfg[2] = {TEST_CONFIG0, TEST_CONFIG1};

    FMC_ENABLE_ISP();
    FMC_ENABLE_CFG_UPDATE();

    FMC_Erase(0x300000);

    FMC_WriteConfig(u32Cfg, 2);
    CU_ASSERT(FMC_Read(0x300000) == TEST_CONFIG0);
    CU_ASSERT(FMC_Read(0x300004) == TEST_CONFIG1);

    FMC_DISABLE_CFG_UPDATE();
    FMC_DISABLE_ISP();

}

void API_FMC_GetChkSum(void)
{
    uint32_t  i, u32LdSum, sum;

    FMC_ENABLE_ISP();
    
  	FMC_ENABLE_LD_UPDATE();  
  	
  	FMC_Erase(FMC_LDROM_BASE);
  	
  	for (i = FMC_LDROM_BASE; i < FMC_LDROM_BASE+128; i+=4)
  		FMC_Write(i, i);

	u32LdSum = 0;
    FMC_GetChkSum(FMC_LDROM_BASE, FMC_LDROM_SIZE, &u32LdSum);

	sum = 0x55;
    FMC_GetChkSum(FMC_LDROM_BASE, FMC_LDROM_SIZE, &sum);
    
    CU_ASSERT(sum == u32LdSum);
    
    FMC_Write(FMC_LDROM_BASE, 0);
	sum = 0x55;
    FMC_GetChkSum(FMC_LDROM_BASE, FMC_LDROM_SIZE, &sum);

    CU_ASSERT(sum != u32LdSum);
}

void API_FMC_CheckAllOne(void)
{
	int  ret;
	
    FMC_ENABLE_ISP();

    FMC_ENABLE_LD_UPDATE();            /* Enable LDROM update. */
    
    FMC_Erase(FMC_LDROM_BASE);         /* Erase LDROM page 0. */
    
    /* Run and check flash contents are all 0xFFFFFFFF. */
    ret = FMC_CheckAllOne(FMC_LDROM_BASE, FMC_FLASH_PAGE_SIZE);
    
    CU_ASSERT(ret == READ_ALLONE_YES);
    
    FMC_Write(FMC_LDROM_BASE, 0);      /* program a 0 to LDROM to make it not all 0xFFFFFFFF. */

    /* Run and check flash contents are not all 0xFFFFFFFF. */
    ret = FMC_CheckAllOne(FMC_LDROM_BASE, FMC_FLASH_PAGE_SIZE);

    CU_ASSERT(ret == READ_ALLONE_NOT);
}

uint32_t  good_key[3] = { 0xe29c0f71, 0x8af051ce, 0xae1f8392 };      /* Assumed correct key in this demo program. */
uint32_t  bad_key[3] =  { 0x73422111, 0xac45663a, 0xf46ac321 };      /* Assumed wrong key in this demo program. */

void API_FMC_Security_Key()
{
	FMC_ENABLE_ISP();
	
	/* Setup a new key */
	FMC_SKey_Setup(good_key, 7, 15, 0, 0);
	
	CU_ASSERT(FMC->KEYSTS & FMC_KEYSTS_KEYLOCK_Msk);
	
	FMC_SKey_Compare(bad_key);         /* Enter a wrong key for key comparison. */
	
	CU_ASSERT(((FMC->KECNT & FMC_KECNT_KECNT_Msk) >> FMC_KECNT_KECNT_Pos) == 1);
	CU_ASSERT(FMC->KEYSTS & FMC_KEYSTS_KEYLOCK_Msk);

	FMC_SKey_Compare(bad_key);         /* Enter a wrong key for key comparison. */

	CU_ASSERT(((FMC->KECNT & FMC_KECNT_KECNT_Msk) >> FMC_KECNT_KECNT_Pos) == 2);
	CU_ASSERT(FMC->KEYSTS & FMC_KEYSTS_KEYLOCK_Msk);

	FMC_SKey_Compare(good_key);        /* Enter the right key for key comparison. */

	CU_ASSERT(((FMC->KECNT & FMC_KECNT_KECNT_Msk) >> FMC_KECNT_KECNT_Pos) == 0);
	CU_ASSERT_FALSE(FMC->KEYSTS & FMC_KEYSTS_KEYLOCK_Msk);
	CU_ASSERT(FMC->KEYSTS & FMC_KEYSTS_KEYMATCH_Msk);

	/* Erase KPROM setting and keys */
	FMC_Erase(FMC_KPROM_BASE);
	FMC_Erase(FMC_KPROM_BASE+FMC_FLASH_PAGE_SIZE);

	CU_ASSERT(((FMC->KECNT & FMC_KECNT_KECNT_Msk) >> FMC_KECNT_KECNT_Pos) == 0);
	CU_ASSERT_FALSE(FMC->KEYSTS & FMC_KEYSTS_KEYLOCK_Msk);
	CU_ASSERT_FALSE(FMC->KEYSTS & FMC_KEYSTS_KEYMATCH_Msk);
}

void API_FMC_Const_Size()
{
    CU_ASSERT(FMC_FLASH_PAGE_SIZE == 512);    
    CU_ASSERT(FMC_PAGE_ADDR_MASK == 0xFFFFFE00UL);
    CU_ASSERT(FMC_LDROM_SIZE == 0x1200);
    CU_ASSERT(FMC_SPROM_SIZE == 0x200);
}

void API_FMC_Const_ISPCMD()
{
    CU_ASSERT(FMC_ISPCMD_READ == 0x00);   
    CU_ASSERT(FMC_ISPCMD_PROGRAM == 0x21);
    CU_ASSERT(FMC_ISPCMD_PAGE_ERASE == 0x22);   
    CU_ASSERT(FMC_ISPCMD_READ_CID == 0x0B);
    CU_ASSERT(FMC_ISPCMD_READ_PID == 0x0C);      
    CU_ASSERT(FMC_ISPCMD_READ_UID == 0x04);      
    CU_ASSERT(FMC_ISPCMD_RUN_CKS == 0x2D);      
    CU_ASSERT(FMC_ISPCMD_READ_CKS == 0x0D);      
    CU_ASSERT(FMC_ISPCMD_RUN_ALL1 == 0x28);      
    CU_ASSERT(FMC_ISPCMD_READ_ALL1 == 0x08);   
    CU_ASSERT(FMC_ISPCMD_VECMAP == 0x2E);  

    CU_ASSERT(READ_ALLONE_YES == 0xA11FFFFF);  
    CU_ASSERT(READ_ALLONE_NOT == 0xA1100000);  
    CU_ASSERT(READ_ALLONE_CMD_FAIL == 0xFFFFFFFF);  
}


void API_FMC_Const_BOOT()
{
    CU_ASSERT(IS_BOOT_FROM_APROM == 0);   
    CU_ASSERT(IS_BOOT_FROM_LDROM == 1);
}


void API_FMC_Const_Flash_Addr()
{
    CU_ASSERT(FMC_APROM_BASE == 0x00000000UL); 
    CU_ASSERT(FMC_APROM_END == 0x00010000UL); 
    CU_ASSERT(FMC_LDROM_BASE == 0x00100000UL);
    CU_ASSERT(FMC_LDROM_END == 0x00101200UL);
    CU_ASSERT(FMC_SPROM_BASE == 0x00200000UL);
    CU_ASSERT(FMC_SPROM_END == 0x00200200UL);
    CU_ASSERT(FMC_CONFIG_BASE == 0x00300000UL);    
    CU_ASSERT(FMC_KPROM_BASE == 0x00301000UL);    
}

CU_TestInfo FMC_ConstantTests[] =
{
    {"FMC CONST Flash Addr", API_FMC_Const_Flash_Addr}, 
    {"FMC CONST Size", API_FMC_Const_Size},
    {"FMC CONST ISPCMD", API_FMC_Const_ISPCMD},
    {"FMC CONST BOOT", API_FMC_Const_BOOT},
    CU_TEST_INFO_NULL
};


CU_TestInfo FMC_MacroTests[] =
{

    {"Enable/Disable ISP", MACRO_FMC_ENABLE_ISP},
    {"Enable/Disable LDROM Update", MACRO_FMC_ENABLE_LD_UPDATE},
    {"Enable/Disable SPROM Update", MACRO_FMC_ENABLE_SP_UPDATE},
    {"Enable/Disable CFG Update", MACRO_FMC_ENABLE_CFG_UPDATE},
    {"Enable/Disable APROM Update", MACRO_FMC_ENABLE_AP_UPDATE},
    {"Get ISP Fail Flag", MACRO_FMC_GET_FAIL_FLAG},
    {"Clear ISP Fail Flag", MACRO_FMC_CLR_FAIL_FLAG},
    {"Set APROM/LDROM Boot", MACRO_FMC_SET_APROM_BOOT},

    CU_TEST_INFO_NULL
};

CU_TestInfo FMC_ApiTests[] =
{
    {"FMC Write", API_FMC_Write},
    {"FMC Read", API_FMC_Read},
    {"FMC Erase", API_FMC_Erase},
    {"FMC Erase SPOROM", API_FMC_Erase_SPROM},
    {"FMC Get Boot Source", API_FMC_GetBootSource},
    {"FMC Read UID", API_FMC_ReadUID},
    {"FMC Read CID", API_FMC_ReadCID},
    {"FMC Read PID", API_FMC_ReadPID},
    {"FMC Read UCID", API_FMC_ReadUCID},
    {"VECMAP", API_FMC_SetVectorPageAddr},
    {"Open", API_FMC_Open},
    {"Close", API_FMC_Close},
    {"DFBA", API_FMC_ReadDataFlashBaseAddr},
    {"Read Config", API_FMC_ReadConfig},
    {"Write Config", API_FMC_WriteConfig},
    {"CRC32 Check Sum", API_FMC_GetChkSum},
    {"Read All One", API_FMC_CheckAllOne},
    {"Security Key", API_FMC_Security_Key},

    CU_TEST_INFO_NULL
};



