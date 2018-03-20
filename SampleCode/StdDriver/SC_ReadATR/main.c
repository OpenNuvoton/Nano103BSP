/******************************************************************************
 * @file     main.c
 * @version  V1.00
 * $Revision: 4 $
 * $Date: 15/12/27 1:14p $
 * @brief    Read the smartcard ATR from smartcard 0 interface.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/
#include <stdio.h>
#include "Nano103.h"
#include "sclib.h"

/**
  * @brief  The interrupt services routine of smartcard port 0
  * @param  None
  * @retval None
  */
void SC0_IRQHandler(void)
{
    /* Please don't remove any of the function calls below */
    // Card insert/remove event occurred, no need to check other event...
    if(SCLIB_CheckCDEvent(0))
        return;
    // Check if there's any timeout event occurs. If so, it usually indicates an error
    SCLIB_CheckTimeOutEvent(0);
    // Check transmit and receive interrupt, all data transmission take place in this function
    SCLIB_CheckTxRxEvent(0);
    /*
        Check if there's any transmission error occurred (e.g. parity error, frame error...)
        These errors will induce SCLIB to deactivation smartcard eventually.
    */
    SCLIB_CheckErrorEvent(0);

    return;
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable External XTAL (4~24 MHz) */
    CLK_EnableXtalRC(CLK_PWRCTL_HXTEN_Msk);

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady(CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to HXT */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HXT, CLK_HCLK_CLK_DIVIDER(1));

    /* Enable IP clock */
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_EnableModuleClock(SC0_MODULE);


    /* Select IP clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HXT, CLK_UART0_CLK_DIVIDER(1));
    // Valid smartcard clock rate is from 1MHz to 4MHz, we set it to 4MHz
    CLK_SetModuleClock(SC0_MODULE, CLK_CLKSEL2_SC0SEL_HXT, CLK_SC0_CLK_DIVIDER(3));

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate SystemCoreClock. */
    SystemCoreClockUpdate();


    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPL &= ~(SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD);
    /* Set PB.4, PA.8 ~ PA.11 for SC0 interface */
    SYS->GPA_MFPH &= ~(SYS_GPA_MFPH_PA8MFP_Msk |
                       SYS_GPA_MFPH_PA9MFP_Msk |
                       SYS_GPA_MFPH_PA10MFP_Msk |
                       SYS_GPA_MFPH_PA11MFP_Msk);
    SYS->GPB_MFPL &= ~SYS_GPB_MFPL_PB4MFP_Msk;

    SYS->GPA_MFPH |= (SYS_GPA_MFPH_PA8MFP_SC0_CLK |
                      SYS_GPA_MFPH_PA9MFP_SC0_DAT |
                      SYS_GPA_MFPH_PA10MFP_SC0_PWR |
                      SYS_GPA_MFPH_PA11MFP_SC0_RST);
    SYS->GPB_MFPL |= SYS_GPB_MFPL_PB4MFP_SC0_CD;

    /* Lock protected registers */
    SYS_LockReg();
}

int main(void)
{
    SCLIB_CARD_INFO_T s_info;
    int retval, i;

    /* Init System, IP clock and multi-function I/O
       In the end of SYS_Init() will issue SYS_LockReg()
       to lock protected register. If user want to write
       protected register, please issue SYS_UnlockReg()
       to unlock protected register if necessary */
    SYS_Init();

    /* Init UART to 115200-8n1 for print message */
    UART_Open(UART0, 115200);

    printf("\nThis sample code reads ATR from smartcard\n");

    /*
        Open smartcard interface 0. CD pin state low indicates card insert and PWR pin low raise VCC pin to card
        The second and third parameter needs to be set according to the board design
    */
    SC_Open(SC0, SC_PIN_STATE_LOW, SC_PIN_STATE_HIGH);
    NVIC_EnableIRQ(SC0_IRQn);

    // Wait 'til card insert
    while(SC_IsCardInserted(SC0) == FALSE);

    /*
        Activate slot 0, and disable EMV2000 check during card activation
        EMV is a technical standard for smart payment cards and for payment terminals and automated teller
        machines that can accept them. It has a stricter checking rule than ISO 7816-3. If the second
        parameter set as TRUE, SCLIB will report activation failure for cards comply with ISO 7816 but not EMV2000
    */
    retval = SCLIB_Activate(0, FALSE);

    if(retval == SCLIB_SUCCESS)
    {
        /*
            Use SCLIB_GetCardInfo to get information about the card, which includes ATR.

            An Answer To Reset (ATR) is a message output by a contact Smart Card conforming to
            ISO/IEC 7816 standards, following electrical reset of the card's chip by a card reader.
            The ATR conveys information about the communication parameters proposed by the card,
            and the card's nature and state.                                --Wikipedia
        */
        SCLIB_GetCardInfo(0, &s_info);
        printf("ATR: ");
        for(i = 0; i < s_info.ATR_Len; i++)
            printf("%02x ", s_info.ATR_Buf[i]);
        printf("\n");
    }
    else
        printf("Smartcard activate failed\n");

    // No operating system, so we have no where to go, just loop forever.
    while(1);
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/


