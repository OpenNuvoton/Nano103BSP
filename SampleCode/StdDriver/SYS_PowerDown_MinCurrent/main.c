/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Demonstrate how to minimize power consumption when entering power down mode.
 *
 * @note
 * Copyright (C) 2023 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "Nano103.h"


/**
 * @brief       PortA/PortB/PortC IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PortA/PortB/PortC default IRQ, declared in startup_nano103.s.
 */
void GPABC_IRQHandler(void)
{
    uint32_t reg;
    if(GPIO_GET_INT_FLAG(PB, BIT3))
    {   /* To check if PB.3 interrupt occurred */
        GPIO_CLR_INT_FLAG(PB, BIT3);/* Clear PB.3 interrupt flag */
        printf("PB.3 INT occurred. \n");
    }
    else
    {    /* Un-expected interrupt. Just clear all PORTA, PORTB, PORTC interrupts */
        reg = PA->INTSRC;
        PA->INTSRC = reg;
        reg = PB->INTSRC;
        PB->INTSRC = reg;
        reg = PC->INTSRC;
        PC->INTSRC = reg;
        printf("Un-expected interrupts. \n");
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  Function for System Entry to Power Down Mode                                                           */
/*---------------------------------------------------------------------------------------------------------*/
void PowerDownFunction(void)
{
    /* Check if all the debug messages are finished */
    UART_WAIT_TX_EMPTY(UART0);

    /* Enter to Power-down mode */
    CLK_PowerDown();
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Switch HCLK clock source to XTAL */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk) | CLK_CLKSEL0_HCLKSEL_HIRC;

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN;

    /* Select IP clock source */
    CLK->CLKSEL1 |= CLK_CLKSEL1_UART0SEL_HIRC;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD  */
    SYS->GPB_MFPL &= ~( SYS_GPB_MFPL_PB0MFP_Msk | SYS_GPB_MFPL_PB1MFP_Msk);
    SYS->GPB_MFPL |= (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD);
}

void UART0_Init()
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init(); //In the end of SYS_Init() will issue SYS_LockReg() to lock protected register. If user want to write protected register, please issue SYS_UnlockReg() to unlock protected register.

    /* Init UART for printf */
    UART0_Init();

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
    printf("+-------------------------------------------------------------+\n");
    printf("|  SYS_PowerDown_MinCurrent and Wake-up by PB.3 Sample Code   |\n");
    printf("+-------------------------------------------------------------+\n\n");

    printf("+-------------------------------------------------------------------------+\n");
    printf("| Operating sequence                                                      |\n");
    printf("|  1. Remove all continuous load, e.g. LED.                               |\n");
    printf("|  2. Must enable LVR                                                     |\n");
    printf("|  3. Disable analog function, e.g. Clock Filter and POR module.          |\n");
    printf("|  4. Enter to Power-Down                                                 |\n");
    printf("|  5. Wait for PB.3 rising-edge interrupt event to wakeup the MCU         |\n");
    printf("+-------------------------------------------------------------------------+\n\n");

    /* Check if all the debug messages are finished */
    UART_WAIT_TX_EMPTY(UART0);

    /* HIRC0, HIRC1 and MIRC Clock Filter disable */
    SYS->RCCFCTL = 0;  

    /* Set function pin to GPIO mode */
    SYS->GPA_MFPL = 0;
    SYS->GPA_MFPH = 0;
    SYS->GPB_MFPL = (SYS_GPB_MFPL_PB0MFP_UART0_RXD | SYS_GPB_MFPL_PB1MFP_UART0_TXD);// exclude debug port;
    SYS->GPB_MFPH = 0;
    SYS->GPC_MFPL = 0;
    SYS->GPC_MFPH = 0;
    SYS->GPD_MFPL = 0;
    SYS->GPD_MFPH = 0;
    SYS->GPE_MFPL = 0;
    SYS->GPF_MFPL = 0x00FFFF00;// exclude GPF5:ICE_DAT, GPF4:ICE_CLK, GPF3:HXT_OUT, GPF2:HXT_IN

    /* Enable GPIO pull up */
    PA->PUEN = 0xFFFF;
    PB->PUEN = 0xFFFF;
    PC->PUEN = 0xFFFF;
    PD->PUEN = 0xFFFF;
    PE->PUEN = 0xFFFF;
    PF->PUEN = 0x0003;/* exclude GPF5:ICE_DAT, GPF4:ICE_CLK, GPF3:HXT_OUT, GPF2:HXT_IN */

    /* Configure PB.3 as Input mode and enable interrupt by rising edge trigger */
    GPIO_SetMode(PB, BIT3, GPIO_PMD_INPUT);
    /* Enable PB.3 interrupt by rising edge trigger */
    GPIO_EnableInt(PB, 3, GPIO_INT_RISING);
    /* Enable GPABC NVIC */
    NVIC_EnableIRQ(GPABC_IRQn);

   /* Unlock protected registers before entering Power-down mode */
    SYS_UnlockReg();

    /* LVR must be enabled and the POR will be enabled automatically */
    SYS_ENABLE_LVR();
    /* Disable Power-on Reset */
    SYS_DISABLE_POR();

    /* Waiting for PB.3 rising-edge interrupt event */
    while(1)
    {
        printf("Enter to Power-Down ......\n");

        /* Enter to Power-down mode */
        PowerDownFunction();

        printf("System waken-up done. %d\n\n", PB3);
    }
}

/*** (C) COPYRIGHT 2023 Nuvoton Technology Corp. ***/
