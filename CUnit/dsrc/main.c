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
#include <assert.h>
#include "CUnit.h"
#include "Console.h"
#include "Nano103.h"
#include "dsrc.h"
#include "dsrc_cunit.h"


// Internal funcfion definition
void AddTests(void);


void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable external 12MHz HXT, 32KHz LXT and HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC0EN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_HIRC0STB_Msk);
  
    /* Set HCLK source form HXT and HCLK source divide 1  */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC,CLK_HCLK_CLK_DIVIDER(1));

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HIRC,CLK_UART0_CLK_DIVIDER(1));
  
    /* Enable DSRC module clock */
    CLK_EnableModuleClock(DSRC_MODULE);
  
    /* Select DSRC clock source */
    CLK_SetModuleClock(DSRC_MODULE,CLK_CLKSEL2_DSRCSEL_HIRC,CLK_DSRC_CLK_DIVIDER(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* First, Clear PB0 and PB1 multi-function pins to '0', and then    */
    /* Set PB0 and PB1 multi-function pins to UART0 RXD and TXD         */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD );
    
    /* First, Clear PC8, PC9, PC10 and PC11 multi function pin to '0', and then      */
    /* Set PC8, PC9, PC10 and PC11 multi function pin to SS0, CLK, MISO0, M0SI       */
    SYS->GPC_MFPH &= ~(SYS_GPC_MFPH_PC8MFP_Msk | SYS_GPC_MFPH_PC9MFP_Msk | SYS_GPC_MFPH_PC10MFP_Msk | SYS_GPC_MFPH_PC11MFP_Msk);
    SYS->GPC_MFPH |= (SYS_GPC_MFPH_PC8MFP_SPI1_SS0 | SYS_GPC_MFPH_PC9MFP_SPI1_CLK | SYS_GPC_MFPH_PC10MFP_SPI1_MISO0 | SYS_GPC_MFPH_PC11MFP_SPI1_MOSI0);

    /* First Clear PD6 and PD7 multi function pin to '0', and then */
    /* Set PD6 and PD7 multi function pin to SPI1 MOSI1, MISO1     */
    SYS->GPD_MFPL &= ~(SYS_GPD_MFPL_PD6MFP_Msk | SYS_GPD_MFPL_PD6MFP_Msk);
    SYS->GPD_MFPL |= (SYS_GPD_MFPL_PD6MFP_SPI1_MOSI1 | SYS_GPD_MFPL_PD7MFP_SPI1_MISO1);
    
    /* Set PB multi-function pins for Clock Output */
    SYS->GPB_MFPL = ( SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB2MFP_Msk ) |  SYS_GPB_MFPL_PB2MFP_CLKO;

    /* Lock protected registers */
    SYS_LockReg();
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
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();
    
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
