/****************************************************************************
*                                                                           *
* Copyright (c) 2004 - 2007 Winbond Electronics Corp. All rights reserved.  *
*                                                                           *
*****************************************************************************/

/****************************************************************************
* FILENAME
*   run_test.c
*
* VERSION
*   1.0
*
* DESCRIPTION
*   The  main program of CUnit test
*
* DATA STRUCTURES
*   None
*
* FUNCTIONS
*   AddTests
*   main
*
* HISTORY
*   2007-06-28    Ver 1.0 Modified by NS21 WTLiu
*
* REMARK
*   None
****************************************************************************/

/***************************************************************************
  HISTORY
  2007-06-28    V1.0
  1. Preliminary Version

***************************************************************************/

// Library header file
#include <stdio.h>
#include <stdlib.h>
#include "CUnit.h"
#include "Console.h"
#include "Nano103.h"
#include "timer_cunit.h"

// Internal funcfion definition
void AddTests(void);


void SYS_Init(void)
{

    /* Enable HIRC */
    CLK->PWRCTL |= CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_LIRCEN_Msk | CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk;


    /* Waiting for clock ready */
    while(!(CLK->STATUS & CLK_STATUS_HIRC0STB_Msk));
    while(!(CLK->STATUS & CLK_STATUS_HXTSTB_Msk));
    while(!(CLK->STATUS & CLK_STATUS_LIRCSTB_Msk));
	  while(!(CLK->STATUS & CLK_STATUS_HIRC1STB_Msk));
	  while(!(CLK->STATUS & CLK_STATUS_MIRCSTB_Msk));
    while(!(CLK->STATUS & CLK_STATUS_LXTSTB_Msk));
    
#if 1
    CLK_EnablePLL(CLK_PLLCTL_PLL_SRC_HXT, 96000000);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_PLL, CLK_HCLK_CLK_DIVIDER(3));
    
#endif

    /* Enable UART and ACMP clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();
    
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );

}


void UART0_Init(void)
{
    UART_Open(UART0, 115200);
}


void exit(int32_t code)
{
    if(code)
        while(1); // Fail
    else
        while(1); // Success
}

int main( int argc, char *argv[] )
{

    /* Unlock protected registers */
    while(SYS->REGLCTL != 1) {
        SYS->REGLCTL = 0x59;
        SYS->REGLCTL = 0x16;
        SYS->REGLCTL = 0x88;
    }

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();
        
    printf("%d\n", SystemCoreClock);
    if(CU_initialize_registry())
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

    if(CUE_SUCCESS != CU_register_suites(suites))
    {
        fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
        exit(EXIT_FAILURE);
    }
}
