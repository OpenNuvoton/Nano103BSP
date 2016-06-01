/****************************************************************************
*                                                                           *
* Copyright (c) 2005 - 2008 Nuvoton Technology Corp. All rights reserved.  *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   main.c
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The main program of CUnit test
*
* DATA STRUCTURES
*   None
*
* FUNCTIONS
*   AddTests
*   main
*
* HISTORY
*   2015-06-23    Ver 1.0 Modified by MS10 YCHuang
*
* REMARK
*   None
****************************************************************************/

/***************************************************************************
  HISTORY
  2015-06-01    V1.0
  1. Preliminary Version

***************************************************************************/

// Library header file
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"
#include "CUnit.h"
#include "Console.h"
#include "Nano103.h"
#include "fmc_cunit.h"


// Internal funcfion definition
void AddTests(void);


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

    /* Set PB multi-function pins for Clock Output */
    SYS->GPB_MFPL = ( SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB2MFP_Msk ) |  SYS_GPB_MFPL_PB2MFP_CLKO;

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


void exit(int32_t code)
{
    if(code)
        while(1); // Fail
    else
        while(1); // Success
}


int main()
{
    uint32_t   u32Cfg;

    SYS_Init();                        /* Init System, IP clock and multi-function I/O */
    UART0_Init();                      /* initialize UART0 */

    printf("\n\n");
    printf("+------------------------------------------+\n");
    printf("|     NANO103AN FMC CUNIT test program     |\n");
    printf("+------------------------------------------+\n");

    /* Unlock protected registers for ISP function */
    SYS_UnlockReg();

    FMC_ENABLE_ISP();
    FMC_ENABLE_CFG_UPDATE();

    u32Cfg = FMC_Read(FMC_CONFIG_BASE);

    printf("CONFIG0 = 0x%08x\n", u32Cfg);
    if (u32Cfg != TEST_CONFIG0)
    {
        FMC_Erase(FMC_CONFIG_BASE);
        FMC_Write(FMC_CONFIG_BASE, TEST_CONFIG0);
        FMC_Write(FMC_CONFIG_BASE+4, TEST_CONFIG1);

        // Perform chip reset to make new User Config take effect
        SYS->IPRST1 = SYS_IPRST1_CHIPRST_Msk;
        while(1);
    }

    FMC->ISPCTL = 0;

    if (CU_initialize_registry())
    {
        fprintf(stderr, " Initialization of Test Registry failed. ");
        exit(EXIT_FAILURE);
    }
    else
    {
        AddTests();
        CU_console_run_tests();
        CU_cleanup_registry();
    }

    while(SYS->PDID);
}


void AddTests(void)
{
	CU_get_registry();
	CU_is_test_running();

    if (CUE_SUCCESS != CU_register_suites(suites))
    {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}
