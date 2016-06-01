/******************************************************************************
* @file     main.c
* @version  V1.00
* $Revision: 4 $
* $Date: 16/02/17 4:54p $
* @brief    Demonstrate maximum operating frequency with different LDO. 
*           Output system clock to PB.2.
*
* @note
* Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano103.h"

/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{    
    SYS_UnlockReg(); /* Unlock protected registers */

    /* Enable external 12MHz HXT, 32KHz LXT, HIRC and MIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_LIRCEN_Msk | CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_LIRCSTB_Msk | CLK_STATUS_HIRC0STB_Msk | CLK_STATUS_HIRC1STB_Msk | CLK_STATUS_MIRCSTB_Msk);

    /*  Set HCLK frequency 36MHz */ 
    CLK_SetCoreClock(FREQ_36MHZ);

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_MIRC,CLK_UART0_CLK_DIVIDER(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD);

    /* Set PB multi-function pins for Clock Output */
    SYS->GPB_MFPL = ( SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB2MFP_Msk ) |  SYS_GPB_MFPL_PB2MFP_CLKO;
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/   
    SYS_ResetModule(UART0_RST); /* Reset UART0 */
    UART_Open(UART0, 115200); /* Configure UART0 and set UART0 115200-8n1 Baudrate */
}

int32_t main (void)
{	
    /* Init System, IP clock and multi-function I/O */
    SYS_Init(); //In the end of SYS_Init() will issue SYS_LockReg() to lock protected register. If user want to write protected register, please issue SYS_UnlockReg() to unlock protected register.
    
    /* Init UART0 for print message */
    UART0_Init();
	
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();
    printf("\n\nCPU @ %dHz\n", SystemCoreClock); /* Display System Core Clock */

    printf("+--------------------------------------------------------+\n");
    printf("| Nano103 Scalable LDO Sample Code                       |\n");
    printf("| LDO:1.8v, System maximum operating frequency is 36MHz. |\n");
    printf("| LDO:1.6v, System maximum operating frequency is 18MHz. |\n");
    printf("|           If user select HCLK clock source from HIRC1, |\n");
    printf("|           hardware will automatically reduce to 18MHz. |\n");
    printf("| LDO:1.2v, System maximum operating frequency is 4MHz.  |\n");
    printf("| PB.2 pin will output HCLK clock.                       |\n");
    printf("+--------------------------------------------------------+\n");
	  	
		
    /* Output selected clock to CKO */
    CLK_EnableCKO(CLK_CLKSEL2_CLKOSEL_HCLK, 0, 1);

    while(1)
    {        
        SYS->RPDBCLK |= SYS_RPDBCLK_RSTPDBCLK_Msk; // Reset pin setting, HIRC0 is selected as reset pin debounce clock.	
        CLK->CLKSEL0 &= ~(CLK_CLKSEL0_ISPSEL_Msk); // ISP Clock Source from HIRC0
        SYS->LDOCTL = (SYS->LDOCTL & ~(SYS_LDOCTL_FMCLVEN_Msk));// Flash memory low voltage(1.2V) mode Disabled

        // 1.8V				       
        printf("Please press any key to select LDO output voltage 1.8V. \n");
        getchar();
        printf("HCLK Clock Source from PLL, PB.2 will output 36MHz. \n\n");
        SYS->LDOCTL = (SYS->LDOCTL & ~(SYS_LDOCTL_LDOLVL_Msk)) | SYS_LDOCTL_LDO_LEVEL18; // LDO 1.8V			
        CLK_SetCoreClock(FREQ_36MHZ); /* Switch HCLK clock source to PLL */	

        // 1.6V				
        printf("Please press any key to select LDO output voltage 1.6V. \n");
        getchar();
        printf("HCLK Clock Source from HIRC1, PB.2 will output 18MHz. \n\n");        
        SYS->LDOCTL = (SYS->LDOCTL & ~(SYS_LDOCTL_LDOLVL_Msk)) | SYS_LDOCTL_LDO_LEVEL16; // LDO 1.6V
        CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC1, CLK_HCLK_CLK_DIVIDER(1));  /* Switch HCLK Clock Source to HIRC1 */	

        // 1.2V
        printf("Please press any key to select LDO output voltage 1.2V. \n");
        getchar();
        printf("HCLK Clock Source from MIRC, PB.2 will output 4MHz. \n\n");  

        SYS->RPDBCLK &= ~(SYS_RPDBCLK_RSTPDBCLK_Msk); // Reset pin setting, MIRC is selected as reset pin debounce clock.		
        CLK->CLKSEL0 |=  CLK_CLKSEL0_ISPSEL_Msk;      // ISP Clock Source from MIRC
        SYS->LDOCTL = (SYS->LDOCTL & ~(SYS_LDOCTL_FMCLVEN_Msk)) | SYS_LDOCTL_FMCLVEN_Msk;// Flash memory low voltage(1.2V) mode Enabled
        SYS->LDOCTL = (SYS->LDOCTL & ~(SYS_LDOCTL_LDOLVL_Msk)) | SYS_LDOCTL_LDO_LEVEL12; // LDO 1.2V			
        CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_MIRC, CLK_HCLK_CLK_DIVIDER(1));  /* Switch HCLK Clock Source to MIRC */

    }
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
