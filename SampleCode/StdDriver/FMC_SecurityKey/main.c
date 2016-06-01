/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 5 $
 * $Date: 16/02/16 9:24a $
 * @brief    Show FMC security key function.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>

#include "Nano103.h"

#define KPMAX_VAL      3               /* KPMAX setting on setup security key (1~15) */
#define KEMAX_VAL      7               /* KEMAX setting on setup security key (1~31) */

uint32_t  good_key[3] = { 0xe29c0f71, 0x8af051ce, 0xae1f8392 };      /* Assumed correct key in this demo program. */
uint32_t  bad_key[3] =  { 0x73422111, 0xac45663a, 0xf46ac321 };      /* Assumed wrong key in this demo program. */


void SYS_Init(void)
{
    SYS_UnlockReg();                             /* Unlock protected registers */

    /* Set HCLK source form HXT and HCLK source divide 1  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT,CLK_HCLK_CLK_DIVIDER(1));

    /* Enable external 12MHz HXT, 32KHz LXT and HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_HIRC0STB_Msk | CLK_STATUS_HIRC1STB_Msk | CLK_STATUS_MIRCSTB_Msk);

    CLK_SetCoreClock(32000000);                  /*  Set HCLK frequency 32MHz */
    
    CLK_EnableModuleClock(UART0_MODULE);         /* Enable UART0 input clock */

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HIRC,CLK_UART0_CLK_DIVIDER(1));
																															
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PA multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );

    SYS_LockReg();                               /* Lock protected registers */
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    SYS_ResetModule(UART0_RST);        /* Reset UART0 */
    UART_Open(UART0, 115200);          /* Configure UART0 and set UART0 as baudrate 115200, N,8,1 */
}


void dump_key_status()
{
	printf("KEYSTS: 0x%x\n    ", FMC->KEYSTS);        /* FMC_KEYSTS register value */
	printf("%s ", (FMC->KEYSTS & FMC_KEYSTS_KEYLOCK_Msk) ? "[LOCK]" : "[UNLOCK]");  /* KEYLOCK(FMC_KEY_STS[1] */
	if (FMC->KEYSTS & FMC_KEYSTS_KEYBUSY_Msk)
		printf("[BUSY] ");                            /* KEYBUSY(FMC_KEYSTS[0]) */
	if (FMC->KEYSTS & FMC_KEYSTS_KEYMATCH_Msk)
		printf("[KEYMATCH] ");                        /* KEYMATCH(FMC_KEYSTS[2]) */
	if (FMC->KEYSTS & FMC_KEYSTS_FORBID_Msk)
		printf("[FORBID] ");                          /* FORBID(FMC_KEYSTS[3]) */
	if (FMC->KEYSTS & FMC_KEYSTS_KEYFLAG_Msk) 
		printf("[KEY LOCK] ");                        /* KEYFLAG(FMC_KEYSTS[4]) */
	if (FMC->KEYSTS & FMC_KEYSTS_CFGFLAG_Msk)
		printf("[CONFIG LOCK] ");                     /* CFGFLAG(FMC_KEYSTS[5]) */
	if (FMC->KEYSTS & FMC_KEYSTS_SPFLAG_Msk)
		printf("[SPROM LOCK] \n");                    /* SPFLAG(FMC_KEYSTS[6]) */

	printf("KPCNT: 0x%x, KPMAX: 0x%x\n", (FMC->KPCNT & FMC_KPCNT_KPCNT_Msk) >> FMC_KPCNT_KPCNT_Pos,    /* KPCNT(FMC_KPCNT[3:0])  */
	                                     (FMC->KPCNT & FMC_KPCNT_KPMAX_Msk) >> FMC_KPCNT_KPMAX_Pos);   /* KPMAX(FMC_KPCNT[11:8]) */
	printf("KECNT: 0x%x, KEMAX: 0x%x\n", (FMC->KECNT & FMC_KECNT_KECNT_Msk) >> FMC_KECNT_KECNT_Pos,    /* KECNT(FMC_KECNT[5:0])  */
										 (FMC->KECNT & FMC_KECNT_KEMAX_Msk) >> FMC_KECNT_KEMAX_Pos);   /* KEMAX(FMC_KECNT[13:8]) */
	printf("\nPress any key to continue...\n\n");     /* Wait user press any key on UART0 debug console */
	getchar();                                        /* block on gettong any one character from UART0 */
}

int main()
{
    SYS_Init();                        /* Init System, IP clock and multi-function I/O */
    UART0_Init();                      /* Initialize UART 0 */

    /*---------------------------------------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/

    printf("+--------------------------------------------+\n");
    printf("|          Nano103 FMC_RW Sample Code        |\n");
    printf("+--------------------------------------------+\n");

    SYS_UnlockReg();                   /* Unlock protected registers */

    FMC_Open();                        /* Enable FMC ISP function */

	printf("The current security key status:\n");
	dump_key_status();                 /* Dump FMC security key status. */

	/* Setup a new key */
	if (FMC_SKey_Setup(good_key, KPMAX_VAL, KEMAX_VAL, 0, 0) < 0) {
		printf("Failed to setup key!\n");   /* error message */
		FMC_SKey_Compare(good_key);         /* Enter the right key for key comparison. */
		if (FMC->KEYSTS & FMC_KEYSTS_KEYLOCK_Msk)
		{
			printf("Have no right security key!\n");
			while (1);                     /* Failed to setup security key. Program aborted. */
		}
	}
	
	printf("The security key status after key setup:\n");
	dump_key_status();                 /* Dump FMC security key status. */
	
	FMC_SKey_Compare(bad_key);         /* Enter a wrong key for key comparison. */
	printf("The security key status after enter a wrong key:\n");
	dump_key_status();                 /* Dump FMC security key status. */

	FMC_SKey_Compare(bad_key);         /* Enter a wrong key for key comparison. */
	printf("The security key status after enter a wrong key second time:\n");
	dump_key_status();                 /* Dump FMC security key status. */

	FMC_SKey_Compare(good_key);        /* Enter the right key for key comparison. */
	printf("The security key status after enter a good key.\n");
	dump_key_status();                 /* Dump FMC security key status. */
	
	printf("Security key can de erased only if it is matched and unlocked.\n"); 
	printf("Press any key to erase the security key...\n");
	getchar();
	
	/* Erase KPROM setting and keys */
	FMC_Erase(FMC_KPROM_BASE);
	FMC_Erase(FMC_KPROM_BASE+FMC_FLASH_PAGE_SIZE);
	
	printf("The security key status after erased =>\n");
	dump_key_status();                 /* Dump FMC security key status. */

    printf("Test done.\n");
    while (1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
