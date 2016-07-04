/******************************************************************************
* @file     main.c
* @version  V1.00
* $Revision: 8 $
* $Date: 16/02/17 4:55p $
* @brief    Demonstrate how to use LXT to trim HIRC
*
* @note
* Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include <stdio.h>
#include "Nano103.h"
/*---------------------------------------------------------------------------------------------------------*/
/*  IRCTrim IRQ Handler                                                                                    */
/*---------------------------------------------------------------------------------------------------------*/
/**
  * @brief      This function get HIRC0, HIRC1 and MIRC trim status
  * @param      None
  * @retval     BIT1 Trim Failure Interrupt 
  * @retval     BIT2 LXT Clock error Interrupt
  * @details    This macro get HIRC0, HIRC1 and MIRC trim interrupt status register.
  */
void HIRC_IRQHandler()
{
    uint32_t status_HIRC0, status_HIRC1, status_MIRC;

    status_HIRC0 = SYS_GET_IRC0TRIM_INT_FLAG(); /* Gets the HIRC0 trim status */	  
    if(status_HIRC0 & BIT1) /* Get Trim Failure Interrupt */
    {			  
        printf("HIRC0 Trim Failure Interrupt\n");               /* Display HIRC0 trim status */			  
        SYS_CLEAR_IRC0TRIM_INT_FLAG(SYS_IRCTISTS_FAIL_INT);     /* Clear Trim Failure Interrupt */
    }				
    if(status_HIRC0 & BIT2) /* Get LXT Clock Error Interrupt */
    {			  
        printf("LXT Clock Error Interrupt\n");                  /* Display HIRC0 trim status */			
        SYS_CLEAR_IRC0TRIM_INT_FLAG(SYS_IRCTISTS_32KERR_INT);   /* Clear LXT Clock Error Interrupt */				
    }

    status_HIRC1 = SYS_GET_IRC1TRIM_INT_FLAG(); /* Gets the HIRC1 trim status */		
    if(status_HIRC1 & BIT1) /* Get Trim Failure Interrupt */
    {			  			  
        printf("HIRC1 Trim Failure Interrupt\n");              /* Display HIRC1 trim status */			
        SYS_CLEAR_IRC1TRIM_INT_FLAG(SYS_IRCTISTS_FAIL_INT);    /* Clear Trim Failure Interrupt */
    }		
    if(status_HIRC1 & BIT2) /* Get LXT Clock Error Interrupt */
    {			
        printf("LXT Clock Error Interrupt\n"); /* Display HIRC1 trim status */			  
        SYS_CLEAR_IRC1TRIM_INT_FLAG(SYS_IRCTISTS_32KERR_INT);  /* Clear LXT Clock Error Interrupt */				
    }

    status_MIRC = SYS_GET_MIRCTRIM_INT_FLAG(); /* Gets the MIRC trim status */		
    if(status_MIRC & BIT1) /* Get Trim Failure Interrupt */
    {			  			  
        printf("MIRC Trim Failure Interrupt\n");              /* Display MIRC trim status */			  
        SYS_CLEAR_MIRCTRIM_INT_FLAG(SYS_IRCTISTS_FAIL_INT);   /* Clear Trim Failure Interrupt */
    }		
    if(status_MIRC & BIT2) /* Get LXT Clock Error Interrupt */
    {			  
        printf("LXT Clock Error Interrupt\n");                /* Display MIRC trim status */			  
        SYS_CLEAR_MIRCTRIM_INT_FLAG(SYS_IRCTISTS_32KERR_INT); /* Clear LXT Clock Error Interrupt */
    }		
}

/*---------------------------------------------------------------------------------------------------------*/
/* Init System Clock                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void SYS_Init(void)
{    
    SYS_UnlockReg(); /* Unlock protected registers */

    /* Enable external 12MHz HXT, 32KHz LXT, HIRC and MIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk | CLK_PWRCTL_LXTEN_Msk | CLK_PWRCTL_HIRC0EN_Msk | CLK_PWRCTL_HIRC1EN_Msk | CLK_PWRCTL_MIRCEN_Msk);

    /* Waiting for clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk | CLK_STATUS_LXTSTB_Msk | CLK_STATUS_HIRC0STB_Msk | CLK_STATUS_HIRC1STB_Msk | CLK_STATUS_MIRCSTB_Msk);
	
    CLK_SetCoreClock(32000000); /*  Set HCLK frequency 32MHz */
    
    CLK_EnableModuleClock(UART0_MODULE); /* Enable IP clock */   
    CLK_SetModuleClock(UART0_MODULE,CLK_CLKSEL1_UART0SEL_HXT,CLK_UART0_CLK_DIVIDER(1)); /* Select IP clock source */
																															
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD);

    /* Set PB multi-function pins for Clock Output */
    SYS->GPB_MFPL = ( SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB2MFP_Msk ) |  SYS_GPB_MFPL_PB2MFP_CLKO;
    
    SYS_LockReg(); /* Lock protected registers */
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/    
    SYS_ResetModule(UART0_RST); /* Reset IP */
    UART_Open(UART0, 115200); /* Configure UART0 and set UART0 Baudrate */
}

/**
  * @brief      This function enable HIRC0 Trim function, Trim failure and LXT clock error interrupts,
  *             get the HIRC0 trim status and output frequency to CLKO pin.
  * @param      IRCType is HIRC0 target frequency. Including:
  *             - \ref SYS_IRC0TCTL_TRIM_11_0592M
  *             - \ref SYS_IRC0TCTL_TRIM_12M
  *             - \ref SYS_IRC0TCTL_TRIM_12_288M
  *             - \ref SYS_IRC0TCTL_TRIM_16M
  * @return     None
  * @details    Running this function will get frequency lock or Trim Failure or LXT clock error. 
  *             Get frequency lock indicate HIRC0 output frequency is accurate within 0.5% deviation.
  *             Get Trim Failure or LXT clock error indicate HIRC0 trim failure.
  */
void TrimHIRC0(uint32_t IRCType)
{
    SYS_EnableHIRC0Trim(IRCType, SYS_IRCTIEN_32KERR_EN | SYS_IRCTIEN_FAIL_EN); /*  Enable IRC Trim, set HIRC0 clock and enable interrupt */

    CLK_SysTickDelay(2000); /* Waiting for HIRC0 Frequency Lock */

    /* Get HIRC0 Frequency Lock */
    while(1)
    {
        if(SYS_GET_IRC0TRIM_INT_FLAG() & BIT0)
        {
            printf("HIRC0 Frequency Lock\n");
            break;
        }
    }
		
    /* Enable CLKO and output frequency = HIRC0  */
    CLK_EnableCKO(CLK_CLKSEL2_CLKOSEL_HIRC, 0, 1);
}

/**
  * @brief      This function enable HIRC1 Trim function, Trim Failure and LXT clock Error interrupts,
  *             get the HIRC1 trim status and output frequency to CLKO pin.
  * @param      None
  * @return     None
  * @details    Running this function will get frequency lock or Trim Failure or LXT clock error. 
  *             Get frequency lock indicate HIRC1 output frequency is accurate within 0.5% deviation.
  *             Get Trim Failure or LXT clock error indicate HIRC1 trim failure.
  */
void TrimHIRC1(void)
{	
    /*  Enable IRC Trim, set HIRC1 clock to 36Mhz and enable interrupt */
    SYS_EnableHIRC1Trim(SYS_IRC1TCTL_TRIM_36M,  SYS_IRCTIEN_32KERR_EN | SYS_IRCTIEN_FAIL_EN); 

    CLK_SysTickDelay(2000); /* Waiting for HIRC1 Frequency Lock */

    /* Get HIRC1 Frequency Lock */ 
    while(1)
    {
        if(SYS_GET_IRC1TRIM_INT_FLAG() & BIT0) {
            printf("HIRC1 Frequency Lock\n");
            break;
        }
    }
		
    /* Enable CLKO and output frequency = HIRC1 */
    CLK_EnableCKO(CLK_CLKSEL2_CLKOSEL_HIRC, 0, 1);
}

/**
  * @brief      This function enable MIRC Trim function, Trim failure and LXT clock error interrupts,
  *             get the MIRC trim status and output frequency to CLKO pin.
  * @param      None
  * @return     None
  * @details    Running this function will get frequency lock or Trim Failure or LXT clock error. 
  *             Get frequency lock indicate MIRC output frequency is accurate within 0.5% deviation.
  *             Get Trim Failure or LXT clock error indicate MIRC trim failure.
  */
void TrimMIRC(void)
{
    /* Enable IRC Trim, set MIRC clock to 4Mhz and enable interrupt */
    SYS_EnableMIRCTrim(SYS_MIRCTCTL_TRIM_4M, SYS_IRCTIEN_32KERR_EN | SYS_IRCTIEN_FAIL_EN); 

    CLK_SysTickDelay(2000); /* Waiting for MIRC Frequency Lock */

    /* Get MIRC Frequency Lock */ 
    while(1)
    {
        if(SYS_GET_MIRCTRIM_INT_FLAG() & BIT0) {
            printf("MIRC Frequency Lock\n");
            break;
        }
    }
		
    /* Enable CLKO and output frequency = MIRC */
    CLK_EnableCKO(CLK_CLKSEL2_CLKOSEL_MIRC, 0, 1);
}

int32_t main (void)
{    
    uint8_t u8Item;

    SYS_UnlockReg(); /* Unlock protected registers */

    /* Init System, IP clock and multi-function I/O
    In the end of SYS_Init() will issue SYS_LockReg()
    to lock protected register. If user want to write
    protected register, please issue SYS_UnlockReg()
    to unlock protected register if necessary */
    SYS_Init(); 

    UART0_Init(); /* Init UART0 for printf */

    /* User need to select IRC target frequency to configure specific trim control register. */
    printf("+----------------------------------------+\n");
    printf("|      Nano103 Trim IRC Sample Code      |\n");    		
    printf("|     Please select Trim target IRC      |\n");
    printf("+----------------------------------------+\n");
    printf("|[1]: HIRC0 11.0592MHz-------------------|\n");
    printf("|[2]: HIRC0 12MHz------------------------|\n");		
    printf("|[3]: HIRC0 12.288MHz--------------------|\n");
    printf("|[4]: HIRC0 16MHz------------------------|\n");
    printf("|[5]: HIRC1 36MHz------------------------|\n");
    printf("|[6]: MIRC  4MHz-------------------------|\n");
    printf("+----------------------------------------+\n");		
    u8Item = getchar();

    /* Enable Interrupt */
    NVIC_EnableIRQ(HIRC_IRQn);
	
    switch(u8Item)
    {
      case '1':					  
          TrimHIRC0(SYS_IRC0TCTL_TRIM_11_0592M); /* Trim HIRC to 11.0592MHz */ 
      break;
      case '2':					  
          TrimHIRC0(SYS_IRC0TCTL_TRIM_12M);      /* Trim HIRC to 12MHz */
      break;
      case '3':					  
          TrimHIRC0(SYS_IRC0TCTL_TRIM_12_288M);  /* Trim HIRC to 12.288MHz */
      break;
      case '4':					 
          TrimHIRC0(SYS_IRC0TCTL_TRIM_16M);      /* Trim HIRC to 16MHz */
      break;
      case '5':			
          TrimHIRC1();                           /* Trim HIRC to 36MHz */
      break;					
      case '6':					  
          TrimMIRC();                            /* Trim HIRC to 4MHz */
      break;

      default:					
      break;
    }
		
    /* Disable IRC Trim */
    SYS_DisableHIRC0Trim();
    SYS_DisableHIRC1Trim();
    SYS_DisableMIRCTrim();
		
    printf("Disable IRC Trim\n");
    while(1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
