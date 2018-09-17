/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 5 $
 * $Date: 15/12/30 10:37a $
 * @brief    Demonstrate how to wake system up form Power-down mode by GPIO interrupt.
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
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
    if(GPIO_GET_INT_FLAG(PA, BIT1))
    {   /* To check if PA.1 interrupt occurred */
        GPIO_CLR_INT_FLAG(PA, BIT1);    /* Clear PA.1 interrupt flag */
        printf("PA.1 INT occurred. \n");

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

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable External XTAL (4~24 MHz) */
    CLK->PWRCTL = (CLK->PWRCTL & ~CLK_PWRCTL_HXTEN_Msk) | (0x1 << CLK_PWRCTL_HXTEN_Pos); // HXT Enabled

    /* Waiting for 12MHz clock ready */
    CLK_WaitClockReady( CLK_STATUS_HXTSTB_Msk);

    /* Switch HCLK clock source to XTAL */
    CLK->CLKSEL0 = (CLK->CLKSEL0 & ~CLK_CLKSEL0_HCLKSEL_Msk)|CLK_CLKSEL0_HCLKSEL_HXT;

    /* Enable IP clock */
    CLK->APBCLK |= CLK_APBCLK_UART0_EN; /* Enable UART0 Clock */

    /* Select IP clock source */
    CLK->CLKSEL1 = (CLK->CLKSEL1 & ~CLK_CLKSEL1_UART0SEL_Msk)|(0x0 << CLK_CLKSEL1_UART0SEL_Pos);  /* Clock source from external 12 MHz */

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

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);   /* show CPU clock */
    /* This sample is for GPIO function */
    /* Test System power down and wake up by GPIO */
    printf("+-------------------------------------------------------+\n");
    printf("|    GPIO Power-Down and Wake-up by PA.1 Sample Code    |\n");
    printf("+-------------------------------------------------------+\n\n");

    /* PA1 is pin 45 */
    GPIO_SetMode(PA, BIT1, GPIO_PMD_INPUT);     /* Configure PA.1 as Input mode */
    NVIC_EnableIRQ(GPABC_IRQn);                 /* Enable GPABC NVIC */

    GPIO_EnableInt(PA, 1, GPIO_INT_RISING);     /* Enable PA.1 interrupt by rising edge trigger */

    /* Waiting for PA.1 rising-edge interrupt event */
    while(1)
    {
        printf("Wait PA.1 to low\n");
        while(1)    /* wait PA.1 state to low */
            if (PA1 == 0)   /* If PA.1 state is low, get into power down mode */
                break;
        printf("Enter to Power-Down (rising-edge) ......  %d \n", PA1);
        UART_WAIT_TX_EMPTY(UART0);  /* To check if all the debug messages are finished */
        CLK_PowerDown();
        printf("System waken-up done. %d\n\n", PA1);
    }
}

/*** (C) COPYRIGHT 2015 Nuvoton Technology Corp. ***/
