/**************************************************************************//**
 * @file     main.c
 * @version  V2.10
 * $Date: 16/01/20 11:18a $
 * @brief    Nano103 General Purpose I/O Driver Sample Code
 *
 * @note
 * Copyright (C) 2015 Nuvoton Technology Corp. All rights reserved.
 *
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
    if (PC->INTSRC & BIT3)
    {    /* To check if PC.3 interrupt occurred */
        PC->INTSRC = BIT3;      /* Clear PC.3 interrupt flag */
        PD6 = PD6 ^ 1;          /* Toggle PD6 for LED control */
        printf("PC.3 INT occurred. \n");
    }
    else
    {    /* Unexpected interrupt. Just clear all PORTA, PORTB, PORTC interrupts */
        /* Un-expected interrupt. Just clear all PORTA, PORTB, PORTC interrupts */
        reg = PA->INTSRC;
        PA->INTSRC = reg;
        reg = PB->INTSRC;
        PB->INTSRC = reg;
        reg = PC->INTSRC;
        PC->INTSRC = reg;
        printf("Unexpected interrupts. \n");
    }
}


/**
 * @brief       PortD/PortE/PortF IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The PortD/PortE/PortF default IRQ, declared in startup_nano103.s.
 */
void GPDEF_IRQHandler(void)
{
    uint32_t reg;
    if (PE->INTSRC & BIT5)
    {    /* To check if PE.5 interrupt occurred */
        PE->INTSRC = BIT5;      /* clear PE.5 interrupt flag */
        PD6 = PD6 ^ 1;          /* Toggle PD6 for LED control */
        printf("PE.5 INT occurred. \n");
    }
    else
    {    /* Unexpected interrupt. Just clear all PORTD, PORTE and PORTF interrupts */
        /* Un-expected interrupt. Just clear all PORTD, PORTE and PORTF interrupts */
        reg = PD->INTSRC;
        PD->INTSRC = reg;
        reg = PE->INTSRC;
        PE->INTSRC = reg;
        reg = PF->INTSRC;
        PF->INTSRC = reg;
        printf("Un-expected interrupts. \n");
    }
}


/**
 * @brief       External INT0 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The External INT0(PB.14) default IRQ, declared in startup_nano103.s.
 */
void EINT0_IRQHandler(void)
{
    PB->INTSRC = BIT14;  /* clear PB.14 interrupt flag */
    PD6 = PD6 ^ 1;      /* Toggle PD6 for LED control */
    printf("PB.14 EINT0 occurred. \n");
}


/**
 * @brief       External INT1 IRQ
 *
 * @param       None
 *
 * @return      None
 *
 * @details     The External INT1(PB.15) default IRQ, declared in startup_nano103.s.
 */
void EINT1_IRQHandler(void)
{
    PB->INTSRC = BIT15;  /* clear PB.15 interrupt flag */
    PD6 = PD6 ^ 1;      /* Toggle PD6 for LED control */
    printf("PB.15 EINT1 occurred. \n");
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
int main (void)
{
    int32_t i32Err;

    /* Init System, IP clock and multi-function I/O */
    SYS_Init(); //In the end of SYS_Init() will issue SYS_LockReg() to lock protected register. If user want to write protected register, please issue SYS_UnlockReg() to unlock protected register.

    /* Init UART for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);   /* show CPU clock */

    /* This sample is for GPIO function */
    /* It controls GPIO input/output and interrupt.  */
    printf("+--------------------------------------+ \n");
    printf("|    NANO103 GPIO Driver Sample Code   | \n");
    printf("+--------------------------------------+ \n");

    /*-----------------------------------------------------------------------------------------------------*/
    /* GPIO Basic Mode Test --- Use Pin Data Input/Output to control GPIO pin                              */
    /*-----------------------------------------------------------------------------------------------------*/
    printf("  >> Please connect PA.0 and PD.7 first << \n");
    printf("     Press any key to start test by using [Pin Data Input/Output Control] \n\n");
    getchar();

    GPIO_SetMode(PA, BIT0, GPIO_PMD_OUTPUT);    /* Configure PA.0 as Output mode */
    GPIO_SetMode(PD, BIT7, GPIO_PMD_INPUT);     /* Configure PD.7 as Input mode */

    i32Err = 0;
    printf("  GPIO Output/Input test ...... \n");

    /* Use Pin Data Input/Output Control to pull specified I/O or get I/O pin status */
    PA0 = 0;                /* Output low */
    CLK_SysTickDelay(10);   /* wait for IO stable */
    if (PD7 != 0)           /* check if the PD7 state is low */
    {
        i32Err = 1;
    }

    PA0 = 1;                /* Output high */
    CLK_SysTickDelay(10);   /* wait for IO stable */
    if (PD7 != 1)           /* check if the PD7 state is high */
    {
        i32Err = 1;
    }

    /* show the result */
    if ( i32Err )
    {
        printf("  [FAIL] --- Please make sure PA.0 and PD.7 are connected. \n");
    }
    else
    {
        printf("  [OK] \n");
    }

    GPIO_SetMode(PA, BIT0, GPIO_PMD_INPUT);     /* Configure PB.0 to default Input mode */
    GPIO_SetMode(PD, BIT7, GPIO_PMD_INPUT);     /* Configure PD.7 to default Input mode */


    /*-----------------------------------------------------------------------------------------------------*/
    /* GPIO Interrupt Function Test                                                                        */
    /*-----------------------------------------------------------------------------------------------------*/
    printf("\n  PC3, PE5, PB14(INT0) and PB15(INT1) are used to test interrupt\n  and control LED(PD6)\n");

    GPIO_SetMode(PD, BIT6, GPIO_PMD_OUTPUT);    /* Configure PD6 to output mode for LED control */

    GPIO_SetMode(PC, BIT3, GPIO_PMD_INPUT);     /* Configure PC3 as Input mode */
    GPIO_EnableInt(PC, 3, GPIO_INT_RISING);     /* Enable PC3 interrupt by rising edge trigger */
    NVIC_EnableIRQ(GPABC_IRQn);                 /* Enable GPABC NVIC */

    GPIO_SetMode(PE, BIT5, GPIO_PMD_INPUT);     /* Configure PE5 as Input mode */
    GPIO_ENABLE_PULL_UP(PE, BIT5);              /* Enable PE5 pull-up */
    GPIO_EnableInt(PE, 5, GPIO_INT_FALLING);    /* Enable PE5 interrupt by falling edge trigger */
    NVIC_EnableIRQ(GPDEF_IRQn);                 /* Enable GPDEF NVIC */

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~0x0f000000) | 0x1000000;   /* Set PB.14 multi-function to External interrupt 0 */
    GPIO_SetMode(PB, BIT14, GPIO_PMD_INPUT);             /* Configure PB.14 as Input mode */
    GPIO_EnableEINT0(PB, 14, GPIO_INT_FALLING);          /* Enable PB.14 interrupt by falling edge trigger */
    NVIC_EnableIRQ(EINT0_IRQn);                         /* Enable EINT0 NVIC */

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~0xf0000000) | 0x10000000;   /* Set PB.15 multi-function to External interrupt 1 */
    GPIO_SetMode(PB, BIT15, GPIO_PMD_INPUT);             /* Configure PB.15 as Input mode */
    GPIO_EnableEINT1(PB, 15, GPIO_INT_BOTH_EDGE);        /* Enable PB.15 interrupt by rising and falling edge trigger */
    NVIC_EnableIRQ(EINT1_IRQn);                         /* Enable EINT1 NVIC */

    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCLKSRC_HCLK, GPIO_DBCLKSEL_1);    /* de-bounce source is HCLK, 1-clock sampling cycle */
    GPIO_ENABLE_DEBOUNCE(PC, BIT3);     /* Enable PC.3 interrupt de-bounce function */
    GPIO_ENABLE_DEBOUNCE(PE, BIT5);     /* Enable PE.5 interrupt de-bounce function */
    GPIO_ENABLE_DEBOUNCE(PB, BIT14);    /* Enable PB.14 interrupt de-bounce function */
    GPIO_ENABLE_DEBOUNCE(PB, BIT15);    /* Enable PB.15 interrupt de-bounce function */

    while (1);  /* Waiting for interrupts */

}


